// $Id: debug.cpp,v 1.13 2019-10-08 13:55:31-07 - - $
// Jeffrey Yao jeyao
// Herman Wu hwwu

#include <climits>
#include <iostream>
#include <vector>

using namespace std;

#include "debug.h"
#include "util.h"

debugflags::flagset debugflags::flags {};

void debugflags::setflags (const string& initflags) {
   for (const unsigned char flag: initflags) {
      if (flag == '@') flags.set();
                  else flags.set (flag, true);
   }
}

// getflag -
//    Check to see if a certain flag is on.

bool debugflags::getflag (char flag) {
   // WARNING: Don't TRACE this function or the stack will blow up.
   return flags.test (static_cast<unsigned char> (flag));
}

void debugflags::where (char flag, const char* file, int line,
                        const char* pretty_function) {
   cout << exec::execname() << ": DEBUG(" << flag << ") "
        << file << "[" << line << "] " << pretty_function << endl;
}

