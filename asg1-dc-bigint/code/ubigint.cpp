// $Id: ubigint.cpp,v 1.16 2019-04-02 16:28:42-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <string>
#include <cmath>
using namespace std;

#include "ubigint.h"
#include "debug.h"

// 4629 = [9,2,6,4]

ubigint::ubigint (unsigned long that): ubig_value (0) {
   DEBUGF ('~', "that = " << that);

   unsigned int count = 1;
   unsigned long thatToCount = that;
   unsigned long thatToCopy = that;
   while( thatToCount /= 10 ) ++count;
   ubig_value.reserve(count);

   DEBUGF ('~', "count = " << static_cast<unsigned>(count));
   
   for(unsigned int i = 0; i != count; ++i){
      DEBUGF ('~', "iteration " << i);

      unsigned long digit = thatToCopy % 10;
      ubig_value.push_back(digit);
      
      DEBUGF ('~', "pushing " << static_cast<unsigned>(digit));
      
      thatToCopy /= 10;
      
      DEBUGF ('~', "thatToCopy = "
               << static_cast<unsigned>(thatToCopy));
   }
   trim_leading_zeroes();
   DEBUGF ('~', "long constructed = " << *this);
}

ubigint::ubigint (const string& that): ubig_value(0) {
   DEBUGF ('~', "string that = \"" << that << "\"");
   for (unsigned char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.insert(ubig_value.begin(), digit - '0');
      DEBUGF ('~', "inserting " << static_cast<unsigned>(digit - '0') );
   }
   trim_leading_zeroes();
   DEBUGF ('~', "string constructed = " << *this);
}

void ubigint::trim_leading_zeroes() {
   while(ubig_value.size() > 0 and ubig_value.back() == 0)
      ubig_value.pop_back();
}

ubigint ubigint::operator+ (const ubigint& that) const {
   auto small_ubv = ubig_value;
   auto big_ubv = ubig_value;
   ubigint zero {0};

   DEBUGF ('a', "this_ubv first dig = "
            << static_cast<unsigned>(ubig_value.at(0)));
   DEBUGF ('a', "that_ubv first dig = "
            << static_cast<unsigned>(that.ubig_value.at(0)));

   if(*this < that){
      small_ubv = ubig_value;
      big_ubv = that.ubig_value;

      DEBUGF ('a', "big = " << that);
      DEBUGF ('a', "small = " << *this);

      if(*this == zero){
         return that;
      }
   }
   else{
      small_ubv = that.ubig_value;
      big_ubv = ubig_value;

      DEBUGF ('a', "big = " << *this);
      DEBUGF ('a', "small = " << that);

      if(that == zero){
         return *this;
      }
   }

   DEBUGF ('a', "small_ubv first dig = "
            << static_cast<unsigned>(small_ubv.at(0)));
   DEBUGF ('a', "big_ubv first dig = "
            << static_cast<unsigned>(big_ubv.at(0)));

   ubigint result;
   result.ubig_value.reserve( result.ubig_value.size() + 1 );
   int carryover = 0;

   // Iterate up until the length of the small number
   for( auto iter = small_ubv.begin();
        iter != small_ubv.end(); ++iter ) {
      long unsigned int i = distance(small_ubv.begin(), iter);

      DEBUGF ('a', "iteration " << i);
      DEBUGF ('a', "result currently is = " << result);

      result.ubig_value.push_back(small_ubv.at(i) + big_ubv.at(i)
                                  + carryover);

      DEBUGF ('a', "adding = "
               << static_cast<unsigned>(small_ubv.at(i)
                  + big_ubv.at(i) + carryover)
               << " with carryover = "
               << static_cast<unsigned>(carryover));
      DEBUGF ('a', "result currently is = " << result);

      carryover = 0;
      if( result.ubig_value.at(i) > 9 ){
         result.ubig_value.at(i) -= 10;

         if( i == big_ubv.size() - 1 ){
            DEBUGF ('a', "pushing new digit");

            big_ubv.push_back(0);
         }

         DEBUGF ('a', "current digit is over 9, adjusting to "
                  << static_cast<unsigned>(result.ubig_value.at(i)));

         carryover = 1;

         DEBUGF ('a', "result currently is = " << result);
      }
   }

   DEBUGF ('a', "result = " << result);
   DEBUGF ('a', "this = " << *this);
   DEBUGF ('a', "that = " << that);

   for( long unsigned int i = small_ubv.size();
        i != big_ubv.size(); ++i ) {
      DEBUGF ('a', "iteration " << i);

      result.ubig_value.push_back(big_ubv.at(i) + carryover);

      DEBUGF ('a', "pushed = "
               << static_cast<unsigned>(big_ubv.at(i) + carryover)
               << " with carryover = "
               << static_cast<unsigned>(carryover));
      
      carryover = 0;
      if(result.ubig_value.at(i) > 9){
         result.ubig_value.at(i) -= 10;
         if( i == big_ubv.size() - 1 ){
            DEBUGF ('a', "pushing new digit");

            result.ubig_value.push_back(1);
         }
         DEBUGF ('a', "current digit is over 9, adjusting to "
                  << static_cast<unsigned>(result.ubig_value.at(i)));
         
         carryover = 1;
      }

      DEBUGF ('a', "result currently is = " << result);
   }
   result.trim_leading_zeroes();

   DEBUGF ('a', "result = " << result);

   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   if (*this == that) return ubigint{0};
   // this must be larger

   ubigint result;
   int carryover = 0;

   for( auto iter = that.ubig_value.begin();
        iter != that.ubig_value.end(); ++iter ){
      auto i = distance(that.ubig_value.begin(), iter);
      
      DEBUGF ('s', "iteration " << i);
      
      DEBUGF ('s', "subtracting "
               << static_cast<unsigned>(ubig_value.at(i) - carryover)
               << "and "
               << static_cast<unsigned>(that.ubig_value.at(i))
               << " with carryover = "
               << static_cast<unsigned>(carryover));
      
      if(ubig_value.at(i) - carryover >= that.ubig_value.at(i) ){
         result.ubig_value.push_back(ubig_value.at(i) - carryover
                              - that.ubig_value.at(i));
         carryover = 0;
      }
      else{
         result.ubig_value.push_back((ubig_value.at(i) - carryover + 10)
                               - that.ubig_value.at(i));
         if(result.ubig_value.at(i) > 9) result.ubig_value.at(i) -= 10;
         carryover = 1;
      }
   }

   for( long unsigned int i = that.ubig_value.size();
        i < ubig_value.size(); ++i ) {
      if(ubig_value.at(i) - carryover < 0){
         result.ubig_value.push_back(ubig_value.at(i) - carryover + 10);
         carryover = 1;
      }
      else{
         result.ubig_value.push_back(ubig_value.at(i) - carryover);
         carryover = 0;
      }
   }

   result.trim_leading_zeroes();
   
   DEBUGF ('s', "result currently " << result);
   
   return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   ubigint zero {0};
   if(*this == zero or that == zero){
      DEBUGF ('m', "result currently " << zero);
      return zero;
   }

   ubigint result;
   result.ubig_value.resize(
      ubig_value.size() + that.ubig_value.size() );

   DEBUGF ('m', "resized size "
            << ubig_value.size() + that.ubig_value.size());

   for( auto iter = ubig_value.begin();
        iter != ubig_value.end(); ++iter ){
      auto i = distance(ubig_value.begin(), iter);
      
      DEBUGF ('m', "iteration " << i);
      
      int c = 0;
      int d = 0;
      for( auto jiter = that.ubig_value.begin();
           jiter != that.ubig_value.end(); ++jiter ){
         auto j = distance(that.ubig_value.begin(), jiter);
         
         DEBUGF ('m', "jiteration " << j);
         DEBUGF ('m', "ij1 " << i + j);
         DEBUGF ('m', "result ubv " << result.ubig_value.at(i + j));
         DEBUGF ('m', "ubv " << ubig_value.at(i));
         DEBUGF ('m', "that ubv " << that.ubig_value.at(j));
         
         d = result.ubig_value.at(i + j) +
             ubig_value.at(i)*that.ubig_value.at(j) + c;
         
         DEBUGF ('m', "d = " << d);
         
         result.ubig_value.at(i + j) = d % 10;
         
         DEBUGF ('m', "result.at(" << static_cast<unsigned>(i) << " + "
                  << static_cast<unsigned>(j) << ") = " << (d % 10));
         
         c = floor( d / 10 );
         
         DEBUGF ('m', "c = " << c);
      }
      result.ubig_value.at(i + that.ubig_value.size()) = c;
      
      DEBUGF ('m', "modified "
               << static_cast<unsigned>(i + that.ubig_value.size())
               << " to " << static_cast<unsigned>(c));

      DEBUGF ('m', "result currently " << result);
   }
   
   DEBUGF ('m', "result currently " << result);
   
   result.trim_leading_zeroes();
   return result;
}

void ubigint::multiply_by_2() {
   ubigint result;
   result.ubig_value.resize( ubig_value.size() + 1 );

   for( auto iter = ubig_value.begin(); iter != ubig_value.end();
        ++iter ){
      auto i = distance(ubig_value.begin(), iter);
      int c = 0;
      int d = 0;
      d = result.ubig_value.at(i) + ubig_value.at(i)*2 + c;
      result.ubig_value.at(i) = d % 10;
      c = floor( d / 10 );
      result.ubig_value.at(i + 1) = c;
   }

   result.trim_leading_zeroes();
   ubig_value = result.ubig_value;
}

void ubigint::divide_by_2() {
   ubigint result;
   result.ubig_value.resize( ubig_value.size() );

   int carryover = 0;
   for(auto i = ubig_value.rbegin(); i != ubig_value.rend(); ++i){
      auto index = distance(i, ubig_value.rend() - 1);

      DEBUGF ('v', "div2: iteration " << index);

      result.ubig_value.at(index) =
         (ubig_value.at(index) + carryover) / 2;

      DEBUGF ('v', "div2: inserted " <<
               static_cast<unsigned>(result.ubig_value.at(index)));
      carryover = (ubig_value.at(index) % 2) * 10;
   }

   DEBUGF ('v', "div2: result " << result);

   result.trim_leading_zeroes();
   ubig_value = result.ubig_value;

   DEBUGF ('v', "div2: this " << *this);
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   // divisor is denominator
   // dividend is numberator
   // quotient is answer
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("can't udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend

   DEBUGF ('v', "now multiplying");

   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
      DEBUGF ('v', "1divisor " << divisor
               << " power_of_2 " << power_of_2);
   }

   DEBUGF ('v', "now dividing");

   while (power_of_2 > zero) {
      DEBUGF ('v', "2divisor " << divisor
               << " remainder " << remainder);
      if (divisor <= remainder) {
         DEBUGF ('v', "remainder = " << remainder
               << " - " << divisor << " quotient = "
               << quotient << " + " << power_of_2);
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
         DEBUGF ('v', "remainder = " << remainder
               << " quotient = " << quotient);
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
      DEBUGF ('v', "2divisor " << divisor
               << " power_of_2 " << power_of_2);
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   if(ubig_value.size() != that.ubig_value.size()) return false;

   for(auto i = ubig_value.begin(); i != ubig_value.end(); ++i){
      auto index = distance(ubig_value.begin(), i);
      if(ubig_value.at(index) != that.ubig_value.at(index))
         return false;
   }

   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   DEBUGF ('l', "this = " << *this);
   DEBUGF ('l', "that = " << that);
   
   if(ubig_value.size() != that.ubig_value.size()){
      if(ubig_value.size() < that.ubig_value.size()){
         DEBUGF ('l', "size is less");
         return true;
      }
      else
         DEBUGF ('l', "size is more");
         return false;
   }
   else{
      DEBUGF ('l', "size is equal");
      
      for(auto i = ubig_value.rbegin(); i != ubig_value.rend(); ++i){
         auto index = distance(i, ubig_value.rend() - 1);
         
         DEBUGF ('l', "iteration " << index);
         DEBUGF ('l', "comparing "
                   << static_cast<unsigned>(ubig_value.at(index))
                   << " and "
                   << static_cast<unsigned>(that.ubig_value.at(index)));
         
         if(ubig_value.at(index) > that.ubig_value.at(index)){
            DEBUGF ('l', "is greater than at index " << index);
            return false;
         }
         if(ubig_value.at(index) < that.ubig_value.at(index)){
            DEBUGF ('l', "is less than at index " << index);
            return true;
         }
      }
      DEBUGF ('l', "equal to");
      return false;
   }
}
ostream& operator<< (ostream& out, const ubigint& that) {
   for(auto i = that.ubig_value.rbegin();
       i != that.ubig_value.rend(); ++i){
      auto index = that.ubig_value.size() -
                   distance(i, that.ubig_value.rend() - 1);
      out << static_cast<unsigned>(*i);

      DEBUGF ('p', "pushing = " << static_cast<unsigned>(*i));
      DEBUGF ('p', "% = " << index % 70);
      DEBUGF ('p', "index = " << index);

      if(index % 69 == 0 and index != 0){
         out << "\\\n";
         DEBUGF ('p', "breaking line");
      }
   }
   return out;
}
