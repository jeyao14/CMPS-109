// $Id: util.tcc,v 1.1 2019-11-09 20:18:13-08 - - $
// Jeffrey Yao jeyao
// Herman Wu hwwu

#include <sstream>
#include <typeinfo>
using namespace std;

template <typename item_t>
ostream& operator<< (ostream& out, const list<item_t>& vec) {
   bool want_space = false;
   for (const auto& item: vec) {
      if (want_space) cout << " ";
      cout << item;
      want_space = true;
   }
   return out;
}

template <typename Type>
string to_string (const Type& that) {
   ostringstream stream;
   stream << that;
   return stream.str();
}

template <typename Type>
Type from_string (const string& that) {
   stringstream stream;
   stream << that;
   Type result;
   if (not (stream >> result and stream.eof())) {
      throw domain_error (string (typeid (Type).name())
            + " from_string (" + that + ")");
   }
   return result;
}

