// $Id: ubigint.cpp,v 1.16 2019-04-02 16:28:42-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <cmath>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that): ubig_value (0) {
   ubig_value.push_back(that);
   trim_zeros();
   DEBUGF ('~', "constructed long " << *this);
}

ubigint::ubigint (const string& that): ubig_value(0) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.insert(ubig_value.begin(), digit - '0');
   }
   trim_zeros();
}

ubigint ubigint::operator+ (const ubigint& that) const {
   ubigint sum;
   bool carry = false;
   int dsum = 0;
   vector<udigit_t>:: const_iterator a = ubig_value.begin(); 
   vector<udigit_t>:: const_iterator b = that.ubig_value.begin();

   while(a != ubig_value.end() 
      && b != that.ubig_value.end()){ 
      //start from low order to high order until shorter number is done
      dsum = *a + *b;
      if(carry == true){
         dsum += 1;
      }
      if(dsum > 9){
         dsum -= 10;
         carry = true;
      }
      sum.ubig_value.push_back(dsum);
      a++;
      b++;
   }

   if(a != ubig_value.cend() && b == that.ubig_value.cend()){
      while(a != ubig_value.cend()){
         dsum = *a;
         if(carry == true){
            dsum += 1;
         }
         if(dsum > 9){
            dsum -= 10;
            carry = true;
         }else{
            carry = false;
         }
         sum.ubig_value.push_back(dsum);
         a++;
      }
   }else if(a == ubig_value.cend() && b != that.ubig_value.cend()){
      while(b != that.ubig_value.cend()){
         dsum = *b;
         if(carry == true){
            dsum += 1;
         }
         if(dsum > 9){
            dsum -= 10;
            carry = true;
         }else{
            carry = false;
         }
         sum.ubig_value.push_back(dsum);
         b++;
      }
   }
   if(carry == true){
      sum.ubig_value.push_back(1);
   }
   sum.trim_zeros();
   return sum;
}

ubigint ubigint::operator- (const ubigint& that) const {
   ubigint dif;
   bool borrow = false;
   int ddif = 0;
   vector<udigit_t>:: const_iterator a = ubig_value.begin(); 
   vector<udigit_t>:: const_iterator b = that.ubig_value.begin();

   while(a != ubig_value.end() && 
      b != that.ubig_value.end()){ 
      //start from low order to high order until shorter number is done
      ddif = *a - *b;
      if(borrow == true){
         ddif -= 1;
         borrow = false;
      }
      if(ddif < 0){
         ddif += 10;
         borrow = true;
      }
      dif.ubig_value.push_back(ddif);
      a++;
      b++;
   }

   if(a != ubig_value.cend() && b == that.ubig_value.cend()){
      while(a != ubig_value.cend()){
         ddif = *a;
         if(borrow == true){
            ddif -= 1;
         }
         if(ddif < 0){
            ddif += 10;
            borrow = true;
         }else{
            borrow = false;
         }
         dif.ubig_value.push_back(ddif);
         a++;
      }
   }else if(a == ubig_value.cend() && b != that.ubig_value.cend()){
      while(b != that.ubig_value.cend()){
         ddif = *b;
         if(borrow == true){
            ddif -= 1;
         }
         if(ddif < 0){
            ddif += 10;
            borrow = true;
         }else{
            borrow = false;
         }
         dif.ubig_value.push_back(ddif);
         b++;
      }
   }

   dif.trim_zeros();

   return dif;
}


ubigint ubigint::operator* (const ubigint& that) const {
   ubigint product;
   product.ubig_value.resize(ubig_value.size() 
      + that.ubig_value.size());
   int size1 = ubig_value.size();
   int size2 = that.ubig_value.size();

   for(int i = 0; i < size1; i++){
      int c = 0;
      int d = 0;
      for(int j = 0; j < size2; j++){
         d = product.ubig_value.at(i + j) 
            + ubig_value.at(i)*that.ubig_value.at(j) + c;
         product.ubig_value.at(i + j) = d % 10;
         c = floor(d/10);
      }
   product.ubig_value.at(i + size2) = c;
   }
   product.trim_zeros();
   return product;
}

void ubigint::multiply_by_2() {
   ubigint product;
   product.ubig_value = ubig_value;
   ubigint two {2};
   product = product * two;
   product.trim_zeros();
   ubig_value = product.ubig_value;
}

void ubigint::divide_by_2() {
   ubigint dividend;
   dividend.ubig_value = ubig_value;
   int size = ubig_value.size();
   DEBUGF ('t', size);

   for(int i = 0; i < size; i++){
      int d = dividend.ubig_value.at(i);
      DEBUGF ('t', "before: " << d);
      d = d/2;
      DEBUGF ('t', "after: " << d);
      int c = 0;
      DEBUGF ('t', "i: " << i);
      if(i < size - 1){
         c = dividend.ubig_value.at(i+1);
         DEBUGF ('t', "checked next");
         DEBUGF ('t', "c: " << c);
         if(c % 2 != 0){
            d += 5;
            DEBUGF ('t', "carried");
         }
      }
      DEBUGF ('t', "divided");
      dividend.ubig_value.at(i) = d;
      DEBUGF ('t', "d: " << d);
   }
   dividend.trim_zeros();
   ubig_value = dividend.ubig_value;
   DEBUGF ('t', "dividend: " << dividend);
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
      DEBUGF ('x', "power of two: " 
         << power_of_2 << " divisor: " << divisor);
      DEBUGF ('d', "kicked");
   }
   DEBUGF ('x', "Now dividing");
   while (zero < power_of_2) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
         DEBUGF ('x', "       remainder: " 
         << remainder << " quotient: " << quotient);
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
      DEBUGF ('d', "punched");
      DEBUGF ('x', "power of two: " << 
         power_of_2 << " divisor: " << divisor);
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   DEBUGF('a', *this);
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   if(ubig_value.size() != that.ubig_value.size()){
      return false;
   }else{
      for(auto i = ubig_value.rbegin(), ir = that.ubig_value.rbegin();
      i < ubig_value.rend(); i++, ir++){
         if(*i != *ir){
            return false;
         }
      }
      return true;
   }
}
// this < that
bool ubigint::operator< (const ubigint& that) const {
   if(ubig_value.size() > that.ubig_value.size()){
      DEBUGF('i', *this << " greater than " << that);
      return false;
   }else if(ubig_value.size() < that.ubig_value.size()){
      DEBUGF('i', *this << " less than because size " << that);
      return true;
   }else{
      for(auto i = ubig_value.rbegin(), ir = that.ubig_value.rbegin(); 
         i < ubig_value.rend(); i++, ir++){
         if(*i < *ir){
            DEBUGF('i', *this << " less than iter check " << that);
            return true;
         }

      }
      DEBUGF('i', *this << " equal than iter check " << that);
      return false;
   }
}

ostream& operator<< (ostream& out, const ubigint& that) {
   for(auto i = that.ubig_value.rbegin(); 
      i != that.ubig_value.rend(); i++){ 
      auto c = distance(that.ubig_value.rbegin(), i+1);

      out << static_cast<unsigned>(*i);

      if(c % 69 == 0 && c != 0){ //heheheheheh
         out << "\\\n";
      }

      DEBUGF ('p', "pushing = " <<static_cast<unsigned>(*i));

   }
   return out;
}

void ubigint::trim_zeros(){
   int z = ubig_value.size();
   DEBUGF('z', z);
   DEBUGF('z', "before: " << *this);
   while (ubig_value.size() > 1 and ubig_value.back() == 0) 
      ubig_value.pop_back();
   DEBUGF('z', "after: " << *this);
}

