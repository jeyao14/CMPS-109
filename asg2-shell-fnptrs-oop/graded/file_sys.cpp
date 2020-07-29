// $Id: file_sys.cpp,v 1.15 2019-10-30 20:19:15-07 - - $
// Jeffrey Yao jeyao
// Herman Wu hwwu

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <iomanip>

using namespace std;

#include "debug.h"
#include "file_sys.h"
#include "commands.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   // Create initial root inode
   // Add entries to the inode's content's dirents
   // where those entries are keyed by "." and ".."
   // and their values will be the root inode_ptr for both
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root;
   root->contents->insert_dirents(pair<string, inode_ptr>(".",cwd));
   root->contents->insert_dirents(pair<string, inode_ptr>("..",cwd));
   root->contents->set_size(2);
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

inode_state::~inode_state() {
   DEBUGF('o', "disowning root");

   root->disown();

   DEBUGF('o', "done disowning root");
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

void inode::disown() {
   DEBUGF('o', "disowning inode");

   contents->disown();
   contents = nullptr;

   DEBUGF('o', "done disowning inode");
}

inode_ptr inode::traverse_reach(inode_ptr curr, 
   wordvec& filepath, long unsigned int& inc,
   const string& command) const {
   DEBUGF ('t', "inc = " << inc << " \nfilepathsize = " 
      << filepath.size() - 1 << " \nfilepath = " << filepath);
   DEBUGF ('t', "filepath.at(inc) = " << filepath.at(inc));

   // Test if directory exists
   int exidirect = curr->get_contents()->
      count_dirents(filepath.at(inc));

   DEBUGF('t', "does directory exists: " << exidirect);

   // If directory doesn't exists
   if(exidirect == 0){
      string my_error = command + ": " + filepath.at(inc) +
         " does not exist";
      throw command_error (my_error);
      exec::status(1);
   }
   // If reached the end of file path
   if(inc == filepath.size() - 1){
      DEBUGF ('t', "end of line");
      DEBUGF ('t', "getContents->read = " 
         << curr->get_contents()->read_dirents("."))

      return curr->get_contents()->read_dirents(filepath.at(inc));
   }

   DEBUGF ('t', "contents = " << 
      curr->get_contents()->read_dirents(filepath.at(inc)));

   auto next_inode = curr->get_contents()->
      read_dirents(filepath.at(inc));
   auto destination = traverse_reach(next_inode, filepath, ++inc, 
      command);
   return destination;
}

inode_ptr inode::traverse_make(inode_ptr curr, 
      wordvec& filepath, long unsigned int& inc, 
      const string& command) const {
   DEBUGF ('t', "inc = " << inc << " \nfilepathsize = "
       << filepath.size() - 1 << " \nfilepath = " << filepath);
   DEBUGF ('t', "filepath.at(inc) = " << filepath.at(inc));

   wordvec filepathCopy = filepath;

   int exidirect = curr->get_contents()->
      count_dirents(filepathCopy.at(inc));

   DEBUGF('t', "filepathCopy: " << filepathCopy);
   DEBUGF('t', "does directory exists: " << exidirect);

   //If directory exists
   if(exidirect == 0){
      string my_error = command + ": " + filepathCopy.at(inc) +
         " does not exist";
      throw command_error (my_error);
      exec::status(1);
   }

   // If at the end of filepath
   DEBUGF('t', "size of filepath = " << filepath.size());
   if(inc == filepath.size() - 2){
      DEBUGF ('t', "end of line");
      DEBUGF ('t', "getContents->read = " << 
         curr->get_contents()->read_dirents(filepathCopy.at(inc)))

      return curr->get_contents()->read_dirents(filepathCopy.at(inc));
   }
   DEBUGF ('t', "contents = " << curr->
      get_contents()->read_dirents(filepath.at(inc)));

   auto next_inode = curr->get_contents()->
      read_dirents(filepath.at(inc));
   auto destination = traverse_make(next_inode, filepath, ++inc,
      command);
   return destination;
}

void inode::lsr_traverse() const {
   if(get_contents()->get_file_type().compare("plain_file") == 0){
      DEBUGF('l', "found plain_file");
      return;
   }

   DEBUGF('l', "now running ls");
   get_contents()->ls();
   DEBUGF('l', "continueing search in a directory");
   get_contents()->lsr();
}

void inode::rmr_traverse() {
   if(get_contents()->get_file_type().compare("plain_file") == 0){
      DEBUGF('l', "found plain_file");
      return;
   }
   DEBUGF('l', "continueing search in a directory");
   get_contents()->rmr();
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

void plain_file::disown(){
   DEBUGF('o', "disowning plain_file " << get_name());
}


size_t plain_file::size() const {
   DEBUGF ('i', "size = " << size_val);
   return size_val;
}
const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   data = words;
}

size_t directory::size() const {
   DEBUGF ('i', "size = " << size_val);
   return size_val;
}

void directory::insert_dirents (pair<string, inode_ptr> insert){
   dirents.insert(insert);
}

inode_ptr directory::read_dirents (const string& key) const {
   return dirents.at(key);
}

void directory::remove_dirents (const string& key) {
   dirents.erase(key);
}

void directory::disown(){
   DEBUGF('o', "disowning directory " << get_name());
   for(auto iter = dirents.begin(); iter != dirents.end();){
      DEBUGF('o', "begin checking " << iter->first);
      if(iter->first.compare(".") == 0 ||
         iter->first.compare("..") == 0){
         DEBUGF('o', "skipped " << iter->first);
         ++iter;
      }
      else{
         DEBUGF('o', "beginning to disown dirent " << iter->first)
         iter->second->disown();
         dirents.erase(iter++);
         DEBUGF('o', "done disowning dirent");
      }
   }
   DEBUGF('o', "done disowning directory " << get_name());
}

void directory::ls() const {
   pwd_print(true);

   //iter->second is inode_ptr
   for(auto iter = dirents.begin(); iter != dirents.end();++iter){
      auto contentsResult = iter->second->get_contents();
      if(contentsResult->get_file_type().compare("plain_file") == 0 ||
         iter->first.compare(".") == 0 ||
         iter->first.compare("..") == 0){

         cout << setw(6);
         cout << iter->second->get_inode_nr() << "  " << setw(6)
            << contentsResult->size() << "  "
            << iter->first << endl;
      }
      else{
         cout << setw(6);
         cout << iter->second->get_inode_nr() << "  " << setw(6)
            << contentsResult->size() << "  "
            << iter->first << "/" << endl;
      }
   }
}

void directory::lsr() const {
   DEBUGF('l', "inside " << get_name());
   for(auto const& [dirname, inode] : dirents){
      if(dirname.compare(".") == 0 or dirname.compare("..") == 0){
         DEBUGF('l', "encountering " << dirname);
      }
      else{
         DEBUGF('l', "traverseing " << dirname);
         inode->lsr_traverse();
      }
   }
}

void directory::mkfile(const string& filename, const wordvec& words) {
   DEBUGF('m', "size of wordvec: " <<  words.size());
   DEBUGF('m', "filename: " <<  filename);

   auto new_file = make_shared<inode>(file_type::PLAIN_TYPE);

   // remove the desired file
   remove_dirents(filename);

   // Copy words except without the command
   wordvec wordsCopy;
   for(long unsigned int i = 1; i < words.size()-1; ++i){
      wordsCopy.insert(wordsCopy.begin(), words.at(i+1));

      auto space_size = 0;
      if(i != words.size()-2){
         space_size = 1;
      }

      new_file->get_contents()->set_size(
         new_file->get_contents()->size() +
         words.at(i+1).length() + space_size
         );

      DEBUGF('m', "size = " << new_file->get_contents()->size());
      DEBUGF('m', "word is: " << words.at(i+1));
      DEBUGF('m', "word is: " << wordsCopy.at(i-1));
   }

   new_file->get_contents()->set_name(filename);
   
   // Set the data
   new_file->get_contents()->writefile(wordsCopy);

   //Only for debug
   for(auto iter = wordsCopy.begin(); iter != wordsCopy.end(); ++iter){
      long unsigned int index = distance(wordsCopy.begin(), iter);
      DEBUGF('m', "wordsCopy: " << wordsCopy.at(index));
   }

   DEBUGF('m', "result is: " << new_file);

   insert_dirents(pair<string, inode_ptr>(filename,new_file));

   set_size(size() + 1);
}

void directory::mkdir(const string& dirname) {
   auto result = make_shared<inode>(file_type::DIRECTORY_TYPE);
   result->get_contents()->
      insert_dirents(pair<string, inode_ptr>(".",result));
   result->get_contents()->
      insert_dirents(pair<string, inode_ptr>("..",read_dirents(".")));

   result->get_contents()->set_size(2);

   dynamic_pointer_cast<directory>(result->
      get_contents())->set_name(dirname);

   DEBUGF ('p', "pushing = " << dirname)

   insert_dirents(pair<string, inode_ptr>(dirname,result));
   set_size(size() + 1);
}

void directory::pwd_print(const bool& colon) const {
   wordvec path;
   path = pwd_traverse(path);

   DEBUGF('p', "in " << get_name());;

   cout << "/";
   for(auto inc = path.rbegin(); inc != path.rend(); ++inc){
      auto index = distance(inc, path.rend() - 1);

      cout << path.at(index);

      if(index != 0){
         cout << "/";
      }
   }
   if(colon == true){
      cout << ":";
   }
   cout << endl;
}

wordvec directory::pwd_traverse(wordvec& currPath) const {
   if(read_dirents(".") == read_dirents("..")){
      DEBUGF('p', "found root");
      return currPath;
   }
   currPath.push_back(get_name());

   DEBUGF ('p', "pushing = " << get_name())

   auto next_dir = read_dirents("..")->get_contents();
   return next_dir->pwd_traverse(currPath);
}

void directory::rm(const string& dirname) {
   DEBUGF('r', "disowning " << dirname);
   read_dirents(dirname)->disown();

   DEBUGF('r', "removing " << dirname);
   remove_dirents(dirname);

   DEBUGF('r', "setting size for " << dirname);
   set_size(size() - 1);

   DEBUGF('r', "exiting rm for " << dirname);
}

void directory::rmr() {
   DEBUGF('l', "inside " << get_name());
   for(auto iter = dirents.begin(); iter != dirents.end();){
      if(iter->first.compare(".") == 0 ||
         iter->first.compare("..") == 0){
         DEBUGF('l', "encountering " << iter->first);
         ++iter;
      }
      else{
         DEBUGF('l', "traverseing " << iter->first);
         iter->second->rmr_traverse();

         DEBUGF('l', "reading " << iter->first);
         read_dirents(iter->first);

         DEBUGF('l', "begining to removing " << iter->first);
         rm(iter++->first);

         DEBUGF('l', "exiting 1 loop of rmr for " <<
            iter->first << " in " << get_name());
      }
   }
}
