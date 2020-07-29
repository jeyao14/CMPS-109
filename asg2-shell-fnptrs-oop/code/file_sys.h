// $Id: file_sys.h,v 1.7 2019-07-09 14:05:44-07 - - $
// Jeffrey Yao jeyao
// Herman Wu hwwu

#ifndef __INODE_H__
#define __INODE_H__

#include <exception>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <string>
using namespace std;

#include "util.h"

// inode_t -
//    An inode is either a directory or a plain file.

enum class file_type {PLAIN_TYPE, DIRECTORY_TYPE};
class inode;
class base_file;
class plain_file;
class directory;
using inode_ptr = shared_ptr<inode>;
using base_file_ptr = shared_ptr<base_file>;
using dir_ptr = shared_ptr<directory>;
ostream& operator<< (ostream&, file_type);


// inode_state -
//    A small convenient class to maintain the state of the simulated
//    process:  the root (/), the current directory (.), and the
//    prompt.

class inode_state {
   friend class inode;
   friend ostream& operator<< (ostream& out, const inode_state&);
   private:
      inode_ptr root {nullptr};
      inode_ptr cwd {nullptr};
      string prompt_ {"% "};
   public:
      inode_state (const inode_state&) = delete; // copy ctor
      inode_state& operator= (const inode_state&) = delete; // op=
      inode_state();
      ~inode_state();

      const string& prompt() const { return prompt_; };
      inode_ptr get_root() const { return root; };
      inode_ptr get_cwd() const { return cwd; };

      void set_cwd(const inode_ptr change) { cwd = change; };
      void set_prompt(const string& change) {prompt_ = change + " "; };
};

// class inode -
// inode ctor -
//    Create a new inode of the given type.
// get_inode_nr -
//    Retrieves the serial number of the inode.  Inode numbers are
//    allocated in sequence by small integer.
// size -
//    Returns the size of an inode.  For a directory, this is the
//    number of dirents.  For a text file, the number of characters
//    when printed (the sum of the lengths of each word, plus the
//    number of words.
//    

class inode {
   friend class inode_state;
   private:
      static int next_inode_nr;
      int inode_nr;
      base_file_ptr contents;
   public:
      inode (file_type);
      int get_inode_nr() const;
      void disown();

      base_file_ptr get_contents() const { return contents; };
      inode_ptr traverse_reach(inode_ptr curr,
         wordvec& filepath, long unsigned int& inc,
         const string& command) const;
      inode_ptr traverse_make(inode_ptr curr,
         wordvec& filepath, long unsigned int& inc,
         const string& command) const;
      void lsr_traverse() const;
      void rmr_traverse();
};


// class base_file -
// Just a base class at which an inode can point.  No data or
// functions.  Makes the synthesized members useable only from
// the derived classes.

class file_error: public runtime_error {
   public:
      explicit file_error (const string& what);
};

class base_file {
   protected:
      base_file() = default;
      virtual const string error_file_type() const = 0;
   public:
      virtual ~base_file() = default;
      base_file (const base_file&) = delete;
      base_file& operator= (const base_file&) = delete;
      virtual size_t size() const
         {throw file_error ("is a " + error_file_type());};
      virtual const wordvec& readfile() const
         {throw file_error ("is a " + error_file_type());};
      virtual void writefile (const wordvec&)
         {throw file_error ("is a " + error_file_type());};
      virtual void disown()
         {throw file_error ("is a " + error_file_type());};

      virtual string get_file_type() const
         {throw file_error ("is a " + error_file_type());};
      virtual int get_dirents_size () const
         {throw file_error ("is a " + error_file_type());};
      virtual void insert_dirents (pair<string, inode_ptr>)
         {throw file_error ("is a " + error_file_type());};
      virtual inode_ptr read_dirents (const string&) const
         {throw file_error ("is a " + error_file_type());};
      virtual int count_dirents (const string&)
         {throw file_error ("is a " + error_file_type());}
      virtual void remove_dirents (const string&)
         {throw file_error ("is a " + error_file_type());};
      virtual void set_size(const size_t&)
         {throw file_error ("is a " + error_file_type());};
      virtual string get_name() const
         {throw file_error ("is a " + error_file_type());};
      virtual void set_name(const string&)
         {throw file_error ("is a " + error_file_type());};

      virtual void ls() const
         {throw file_error ("is a " + error_file_type());};
      virtual void lsr() const
         {throw file_error ("is a " + error_file_type());};
      virtual void mkdir(const string&)
         {throw file_error ("is a " + error_file_type());};
      virtual void mkfile(const string&, const wordvec&)
         {throw file_error ("is a " + error_file_type());};
      virtual void pwd_print(const bool&) const
         {throw file_error ("is a " + error_file_type());};
      virtual wordvec pwd_traverse(wordvec&) const
         {throw file_error ("is a " + error_file_type());};
      virtual void rm(const string&)
         {throw file_error ("is a " + error_file_type());};
      virtual void rmr()
         {throw file_error ("is a " + error_file_type());};
};

// class plain_file -
// Used to hold data.
// synthesized default ctor -
//    Default vector<string> is a an empty vector.
// readfile -
//    Returns a copy of the contents of the wordvec in the file.
// writefile -
//    Replaces the contents of a file with new contents.

class plain_file: public base_file {
   private:
      wordvec data;
      virtual const string error_file_type() const override {
         return "plain file";
      }
      string name;
      size_t size_val = 0;
   public:
      virtual void disown() override;
      
      virtual size_t size() const override;
      virtual const wordvec& readfile() const override;
      virtual void writefile (const wordvec& newdata) override;
      virtual string get_name() const { return name; };
      virtual void set_name(const string& change) { name = change; };

      virtual string get_file_type() const { return "plain_file"; };
      virtual void set_size(const size_t& change)
         { size_val = change; };
};

// class directory -
// Used to map filenames onto inode pointers.
// default ctor -
//    Creates a new map with keys "." and "..".
// remove -
//    Removes the file or subdirectory from the current inode.
//    Throws an file_error if this is not a directory, the file
//    does not exist, or the subdirectory is not empty.
//    Here empty means the only entries are dot (.) and dotdot (..).
// mkdir -
//    Creates a new directory under the current directory and 
//    immediately adds the directories dot (.) and dotdot (..) to it.
//    Note that the parent (..) of / is / itself.  It is an error
//    if the entry already exists.
// mkfile -
//    Create a new empty text file with the given name.  Error if
//    a dirent with that name exists.

class directory: public base_file {
   private:
      // Must be a map, not unordered_map, so printing is lexicographic
      map<string,inode_ptr> dirents;
      virtual const string error_file_type() const override {
         return "directory";
      }
      string name;
      size_t size_val = 0;
   public:
      virtual void disown() override;

      virtual size_t size() const override;
      virtual string get_file_type() const { return "directory"; };
      virtual int get_dirents_size () const { return dirents.size(); };
      virtual void insert_dirents (pair<string, inode_ptr> insert)
         override;
      virtual int count_dirents (const string& key)
         {return dirents.count(key);}
      virtual inode_ptr read_dirents (const string& key) const
         override;
      virtual void remove_dirents (const string& key) override;
      virtual string get_name() const { return name; };
      virtual void set_name(const string& change) { name = change; };
      virtual void set_size(const size_t& change)
         { size_val = change; };

      virtual void ls() const override;
      virtual void lsr() const override;
      virtual void mkdir(const string& dirname) override;
      virtual void mkfile(const string& finame, const wordvec& words)
         override;
      virtual void pwd_print(const bool& colon) const override;
      virtual wordvec pwd_traverse(wordvec& currPath) const override;
      virtual void rm(const string& dirname) override;
      virtual void rmr() override;
};

#endif

