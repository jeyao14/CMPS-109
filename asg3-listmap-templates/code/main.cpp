// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $
// Jeffrey Yao jeyao
// Herman Wu hwwu

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

//code used from /misc/matchlines.cpp
void do_operations (istream& input, string filename) {
   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};

   str_str_map op_map;

   int operation_num = 0;
   for (;;) {
      ++operation_num;

      string line;
      getline (input, line);

      if (input.eof()) break;

      cout << filename << ": " 
         << operation_num << ": " << line << endl;

      smatch result;
      if (regex_search (line, result, comment_regex)) {
         // comment function
         // prints nothing
         continue;
      }
      if (regex_search (line, result, key_value_regex)) {
         if(result[1].length() != 0){
            if(result[2].length() != 0){
               // key = value function
               // prints key and value
               str_str_pair new_node(result[1],result[2]);
               op_map.insert(new_node);
               cout << result[1] << " = " << result[2] << endl;
            }
            else{
               // key = function
               // prints nothing
               auto to_erase = op_map.find(result[1]);
               op_map.erase(to_erase);
            }
         }
         else{
            if(result[2].length() != 0){
               // = value function
               // prints all pairs 
               //with the given value
               for(auto iter = op_map.begin(); 
                  iter != op_map.end(); ++iter){
                  if(result[2].compare(iter.get_value()) == 0){
                     cout << iter.get_key() << 
                        " = " << iter.get_value() << endl;
                  }
               }
            }
            else{
               // = function
               // prints entire listmap
               for(auto iter = op_map.begin(); 
                  iter != op_map.end(); ++iter){
                  cout << iter.get_key() << 
                     " = " << iter.get_value() << endl;
               }
            }
         }
      }
      else if (regex_search (line, result, trimmed_regex)) {
         // key function
         // prints the key and value
         auto value_iter = op_map.find(result[1]);

         if(value_iter == op_map.end()){
            cout << result[1] << ": key not found" << endl;
            continue;
         }

         cout << result[1] << " = " << value_iter.get_value() << endl;
      }
      else {
         assert (false and "This can not happen.");
      }
   }
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   sys_info::exit_status(0);
   // checks for debug flags
   scan_options (argc, argv);

   // check for file arguments
   if(argc != 1){
      for(int iter = 1; iter != argc; ++iter){
         string filename = argv[iter];

         // if - is encountered, do standard in
         if(filename.compare("-") == 0){
            do_operations(cin, filename);
            continue;
         }

         ifstream filestream(filename);

         if(filestream.fail()){
            cerr << filename << ": file not found" << endl;
            sys_info::exit_status(1);
            continue;
         }
         do_operations(filestream, filename);
      }
   }
   // if nothing is specified, do standard in
   else{
      do_operations(cin, "-");
   }

   return EXIT_SUCCESS;
}

