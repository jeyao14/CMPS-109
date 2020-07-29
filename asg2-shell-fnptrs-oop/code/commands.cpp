// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $
// Jeffrey Yao jeyao
// Herman Wu hwwu

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat    },
   {"cd"    , fn_cd     },
   {"echo"  , fn_echo   },
   {"exit"  , fn_exit   },
   {"ls"    , fn_ls     },
   {"lsr"   , fn_lsr    },
   {"make"  , fn_make   },
   {"mkdir" , fn_mkdir  },
   {"prompt", fn_prompt },
   {"pwd"   , fn_pwd    },
   {"rm"    , fn_rm     },
   {"rmr"   , fn_rmr    },
   {"#"     , fn_comment},
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
      exec::status(1);
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words){
   if( words.size() == 1){
      throw command_error("cat: file not specified");
   }
   for(auto words_iter = ++words.begin();
      words_iter != words.end(); ++words_iter){
      long unsigned int words_index =
         distance(words.begin(), words_iter);

      DEBUGF('a', "words size = " << words.size());

      auto destpath = split(words.at(words_index), "/");

      DEBUGF('a', "destpath =  " << destpath);

      string destfi = destpath.at(destpath.size()-1);

      DEBUGF('a', "destfile =  " << destfi);

      long unsigned int inc = 0;
      //if filepath is defined
      if(destpath.size() > 1){
         auto cat = state.get_cwd()->traverse_make(state.get_cwd(), 
            destpath, inc, "cat");
         string ftype = cat->get_contents()->get_file_type();
         int exifile = dynamic_pointer_cast<directory>(cat->
            get_contents())->count_dirents(destfi);

         DEBUGF('a', "file name =  " << destfi);

         if(exifile == 0){
            string my_error = "cat: " + destfi + " does not exist";
            throw command_error (my_error);
            exec::status(1);
         }
         if(ftype.compare("directory") == 0){
            string my_error = "cat: " + destfi + " is directory";
            throw command_error (my_error);
            exec::status(1);
         }
      //if rming file in cwd
      }else{
         auto curr = state.get_cwd();

         int exifile = dynamic_pointer_cast<directory>(curr->
            get_contents())->count_dirents(destfi);

         DEBUGF('a', "file name =  " << destfi);

         if(exifile == 0){
            string my_error = "cat: " + destfi + " does not exist";
            throw command_error (my_error);
            exec::status(1);
         }

         string ftype = dynamic_pointer_cast<directory>(curr->
            get_contents())->read_dirents(destfi)->
            get_contents()->get_file_type();

         if(ftype.compare("directory") == 0){
            string my_error = "cat: " + destfi + " is directory";
            throw command_error (my_error);
            exec::status(1);
         }

         wordvec printed = curr->get_contents()->
            read_dirents(destfi)->get_contents()->readfile();
         for(auto iter = printed.rbegin();
            iter != printed.rend(); ++iter){
            long unsigned int index =
               distance(iter, printed.rend() - 1);
            cout << printed.at(index) << " "; 
         }
      }
      cout << endl;
   }
}

// traverse_reach handles error if filepath doesn't exist
void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if(words.size() > 2){
      throw command_error("cd: invalid arguments");
      exec::status(1);
   }
   else if(words.size() == 1){
      DEBUGF('d', "setting root");

      auto root = state.get_root();
      state.set_cwd(root);
   } else {
      string s = "";
      long unsigned int inc = 0;
      auto destpath = split(words.at(1), "/");
      if(does_start_with_slash(words.at(1))){
         DEBUGF('d', "starting at root");

         auto root = state.get_root();
         state.set_cwd(root);
      }
      if(destpath.size() != 0){
         DEBUGF('d', "navigating to destination");
         DEBUGF('d', "size of despath = " << destpath.size());

         auto change = state.get_cwd()->
            traverse_reach(state.get_cwd(), destpath, inc, "cd");
         string ftype = change->get_contents()->get_file_type();
         string fname = change->get_contents()->get_name();

         DEBUGF('d', "file type = " << ftype);
         DEBUGF('d', "file name = " << fname);

         if(ftype.compare("plain_file") == 0){
            string my_error = "cd: " + fname + " is plain file type";
            throw command_error (my_error);
            exec::status(1);
         }else{
            state.set_cwd(change);
         }
      }
   }
}

void fn_comment (inode_state&, const wordvec&){
   //do nothing
   return;
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string s = "";
   DEBUGF ('e', words.size());
   if(words.size() == 1){
      DEBUGF ('e', "exit 0");
      exec::status(0);
   }else if(words.size() > 1){
      for(long unsigned int i = 0; i < words.size()-1; ++i){
         s += words.at(i+1);
      }
      DEBUGF('e', "string = " << s);
      for(long unsigned int i = 0; i < words.size()-1; ++i){
         if(isalpha(s[i])){ 
            DEBUGF ('e', "exit 127");
            exec::status(127);
            break;
         }
      }
      int exit_status = stoi(s);
      DEBUGF ('e', "exit user");
      DEBUGF('e', "exit status = " << exit_status);
      exec::status(exit_status);
   }
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   DEBUGF('d', "starting ls");

   if(words.size() > 2){
      throw command_error("ls: invalid arguments");
      exec::status(1);
   }
   else if(words.size() == 1){
      DEBUGF('d', "runnign ls on root");

      state.get_cwd()->get_contents()->ls();
   }
   else{
      DEBUGF('d', "constructing string");
      string s = "";
      long unsigned int inc = 0;
      auto destpath = split(words.at(1), "/");
      
      DEBUGF('d', "checking if starting with slash");

      //Determine which is the starting directory
      inode_ptr starting_dir;
      if(does_start_with_slash(words.at(1))){
         DEBUGF('d', "starting at root");

         starting_dir = state.get_root();
      }
      else{
         starting_dir = state.get_cwd();
      }
      
      DEBUGF('d', "navigating to destination");

      if(destpath.size() != 0){
         auto change =
            starting_dir->traverse_reach(starting_dir,
            destpath, inc, "ls");
         string ftype = change->get_contents()->get_file_type();
         string fname = change->get_contents()->get_name();

         DEBUGF('d', "file type = " << ftype);
         DEBUGF('d', "file name = " << fname);

         if(ftype.compare("plain_file") == 0){
            string my_error = "ls: " + fname + " is file type";
            throw command_error (my_error);
            exec::status(1);
         }else{
            change->get_contents()->ls();
         }
      }
      else{
         starting_dir->get_contents()->ls();
      }
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if(words.size() == 1){
      DEBUGF('d', "runnign lsr on root");

      state.get_cwd()->lsr_traverse();
      return;
   }
   // if multiple filepaths are provided
   for(auto iter = ++words.begin(); iter != words.end(); ++iter){
      long unsigned int index = distance(words.begin(), iter);

      DEBUGF('d', "iteration " << index);
      DEBUGF('d', "constructing string");

      string s = "";
      long unsigned int inc = 0;
      auto destpath = split(words.at(index), "/");
      
      DEBUGF('d', "checking if starting with slash");

      //Determine which is the starting directory
      inode_ptr starting_dir;
      if(does_start_with_slash(words.at(index))){
         DEBUGF('d', "starting at root");

         starting_dir = state.get_root();
      }
      else{
         starting_dir = state.get_cwd();
      }
      
      DEBUGF('d', "navigating to destination");

      //Check if there's a filepath to traverse
      if(destpath.size() != 0){
         auto destination_inode =
            starting_dir->traverse_reach(starting_dir,
            destpath, inc, "lsr");
         string ftype = destination_inode->
            get_contents()->get_file_type();
         string fname = destination_inode->
            get_contents()->get_name();

         DEBUGF('d', "file type = " << ftype);
         DEBUGF('d', "file name = " << fname);

         if(ftype.compare("plain_file") == 0){
            string my_error = "lsr: " + fname + " is file type";
            throw command_error (my_error);
            exec::status(1);
         }else{
            destination_inode->lsr_traverse();
         }
      }
      //if none
      else{
         starting_dir->lsr_traverse();
      }
   }
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if( words.size() == 1){
      throw command_error("make: file not specified");
      exec::status(1);
   }

   auto filepath = split(words.at(1), "/");

   DEBUGF('k', "checking filename");
   string filename = filepath.at(filepath.size()-1);

   DEBUGF('k', "filename = " << filename);
   DEBUGF('k', "size of words = " << filepath.size());

   inode_ptr dest_inode;
   if(filepath.size() > 1){
      long unsigned int inc = 0;
      dest_inode = state.get_cwd()
         ->traverse_make(state.get_cwd(), filepath, inc, "make");
   }else{
      dest_inode = state.get_cwd();
   }

   auto if_exists = dynamic_pointer_cast<directory>(dest_inode->
      get_contents())->count_dirents(filename);

   if(if_exists != 0){
      string file_type = dynamic_pointer_cast<directory>(dest_inode->
         get_contents())->read_dirents(filename)->
         get_contents()->get_file_type();
      string file_name = dynamic_pointer_cast<directory>(dest_inode->
         get_contents())->read_dirents(filename)->
         get_contents()->get_name();

      DEBUGF('k', "checking directory");

      if(file_type.compare("directory") == 0){
         string my_error = "make: " + file_name + " is a directory";
         throw command_error(my_error);
         exec::status(1);
      }
   }

   dest_inode->get_contents()->
         mkfile(filename, words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if( words.size() != 2){
      throw command_error("mkdir: invalid number of arguments");
      exec::status(1);
   }

   auto filepath = split(words.at(1), "/");
   auto filename = filepath.at(filepath.size() - 1);

   DEBUGF('m', "size of words = " << filepath.size());
   DEBUGF('m', "filename = " << filename);

   if(filepath.size() > 1){
      long unsigned int inc = 0;
      auto make_dir_inode = state.get_cwd()
         ->traverse_make(state.get_cwd(), filepath, inc, "mkdir");

      auto doesExist = make_dir_inode->get_contents()->
         count_dirents(filename);
      if(doesExist == 1){
         string my_error = "mkdir: " + filename + " already exists";
         throw command_error(my_error);
         exec::status(1);
      }

      make_dir_inode->get_contents()
         ->mkdir(filename);
   }else{

      auto doesExist = state.get_cwd()->get_contents()->
         count_dirents(filename);
      if(doesExist == 1){
         string my_error = "mkdir: " + filename + " already exists";
         throw command_error(my_error);
         exec::status(1);
      }

      state.get_cwd()->get_contents()
         ->mkdir(filename);
   }
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string new_prompt;
   for(auto iter = ++words.begin(); iter != words.end(); ++iter){
      new_prompt += *iter;
      if(iter != --words.end()){
         new_prompt += " ";
      }
   }
   state.set_prompt(new_prompt);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   state.get_cwd()->get_contents()->pwd_print(false);
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   DEBUGF('d', "starting ls");

   inode_ptr destination_dir;

   if(words.size() != 2){
      throw command_error("rm: invalid arguments");
      exec::status(1);
   }
   DEBUGF('d', "constructing string");

   string s = "";
   long unsigned int inc = 0;
   auto destpath = split(words.at(1), "/");
   
   DEBUGF('d', "checking if starting with slash");

   //Determine which is the starting directory
   inode_ptr starting_dir;
   if(does_start_with_slash(words.at(1))){
      DEBUGF('d', "starting at root");

      starting_dir = state.get_root();
   }
   else{
      starting_dir = state.get_cwd();
   }
   
   DEBUGF('d', "navigating to destination");

   string destfi;
   if(destpath.size() != 1){
      DEBUGF('d', "traversing");

      destination_dir =
         starting_dir->traverse_make(starting_dir,
         destpath, inc, "rm");
      string ftype = destination_dir->
         get_contents()->get_file_type();
      string fname = destination_dir->get_contents()->get_name();

      DEBUGF('d', "file type = " << ftype);
      DEBUGF('d', "file name = " << fname);

      DEBUGF('d', "getting destfi");

      destfi = destpath.at(destpath.size()-1);
      DEBUGF('d', "destfi = " << destfi);
   }
   else{
      DEBUGF('d', "starting in starting_dir");
      destination_dir = starting_dir;
      destfi = words.at(words.size()-1);

      DEBUGF('d', "destfi = " << destfi);
   }

   DEBUGF('d', "does dir name exist = " <<
      destination_dir->get_contents()->get_name());

   auto doesExist = destination_dir->get_contents()->
      count_dirents(destfi);
   DEBUGF('d', "doesExist = " << doesExist);
   if(doesExist == 0){
      string my_error = "rm: " + destfi + " does not exist";
      throw command_error(my_error);
      exec::status(1);
   }

   DEBUGF('d', "checking if directory");

   if(destination_dir->get_contents()->read_dirents(destfi)->
      get_contents()->get_file_type().compare("directory") == 0){
      DEBUGF('d', "checking directory size");

      DEBUGF('d', "directory size is " <<
         destination_dir->get_contents()->read_dirents(destfi)->
         get_contents()->get_dirents_size());

      if(destination_dir->get_contents()->read_dirents(destfi)->
         get_contents()->get_dirents_size() > 2){
         string my_error = "rm: " + destfi + " is non-empty directory";
         throw command_error(my_error);
         exec::status(1);
      }

      DEBUGF('d', "directory is empty");
   }

   destination_dir->get_contents()->rm(destfi);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if(words.size() == 1){
      throw command_error("rmr: no file path specified");
      exec::status(1);
   }
   // if multiple filepaths are provided
   for(auto iter = ++words.begin(); iter != words.end(); ++iter){
      long unsigned int index = distance(words.begin(), iter);

      DEBUGF('d', "iteration " << index);
      DEBUGF('d', "constructing string");

      string s = "";
      long unsigned int inc = 0;
      auto destpath = split(words.at(index), "/");
      auto filename = destpath.at(destpath.size()-1);
      
      DEBUGF('d', "checking if starting with slash");

      //Determine which is the starting directory
      inode_ptr starting_dir;
      if(does_start_with_slash(words.at(index))){
         DEBUGF('d', "starting at root");

         starting_dir = state.get_root();
      }
      else{
         starting_dir = state.get_cwd();
      }
      
      DEBUGF('d', "navigating to destination");

      //Check if there's a filepath to traverse
      if(destpath.size() != 1){
         DEBUGF('d', "now traversing");

         auto destination_inode =
            starting_dir->traverse_make(starting_dir,
            destpath, inc, "rmr");

         string ftype = destination_inode->
            get_contents()->read_dirents(filename)->
            get_contents()->get_file_type();
         string fname = destination_inode->
            get_contents()->read_dirents(filename)->
            get_contents()->get_name();
         DEBUGF('d', "file type = " << ftype);
         DEBUGF('d', "file name = " << fname);

         if(ftype.compare("plain_file") == 0){
            destination_inode->get_contents()->rm(filename);
         }else{
            destination_inode->get_contents()
               ->read_dirents(filename)->rmr_traverse();
            destination_inode->get_contents()->rm(filename);
         }
      }
      //if none
      else{
         DEBUGF('d', "not traversing");

         string ftype = starting_dir->
            get_contents()->read_dirents(filename)->
            get_contents()->get_file_type();
         if(ftype.compare("plain_file") == 0){
            starting_dir->get_contents()->rm(filename);
         }else{
            starting_dir->get_contents()
               ->read_dirents(filename)->rmr_traverse();
            starting_dir->get_contents()->rm(filename);
         }
         
      }
   }
}

