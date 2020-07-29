// $Id: bigint.cpp,v 1.78 2019-04-03 16:44:33-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): ubig_value (that), is_negative (that < 0) {
   DEBUGF ('%', this << " -> " << ubig_value);
}

bigint::bigint (const ubigint& ubig_value_, bool is_negative_):
                ubig_value(ubig_value_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   ubig_value = ubigint (that.substr (is_negative ? 1 : 0));
   DEBUGF ('%', "sign = " << is_negative);
   DEBUGF ('%', "ubig_value = " << that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {ubig_value, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   ubigint result;
   bool result_neg;
   if(is_negative == that.is_negative){
      result = ubig_value + that.ubig_value;
      result_neg = is_negative;
   }
   else{
      if(ubig_value < that.ubig_value){
         result = that.ubig_value - ubig_value;
         result_neg = that.is_negative;
      }
      else{
         result = ubig_value - that.ubig_value;
         result_neg = is_negative;
      }
   }
   return {result, result_neg};
}

bigint bigint::operator- (const bigint& that) const {
   ubigint result;
   bool result_neg;
   if(is_negative == that.is_negative){
      if(ubig_value > that.ubig_value){
         result = ubig_value - that.ubig_value;
         result_neg = is_negative;
      }
      else{
         result = that.ubig_value - ubig_value;
         result_neg = not is_negative;
      }
   }
   else{
      result = ubig_value + that.ubig_value;
      result_neg = is_negative;
   }
   return {result, result_neg};
}


bigint bigint::operator* (const bigint& that) const {
   ubigint result = ubig_value * that.ubig_value;
   bool result_neg = is_negative == that.is_negative ? false : true;
   return {result, result_neg};
}

bigint bigint::operator/ (const bigint& that) const {
   ubigint result = ubig_value / that.ubig_value;
   bool result_neg = is_negative == that.is_negative ? false : true;
   return {result, result_neg};
}

bigint bigint::operator% (const bigint& that) const {
   ubigint result = ubig_value % that.ubig_value;
   bool result_neg = is_negative;
   return {result, result_neg};
}

bool bigint::operator== (const bigint& that) const {
   if(is_negative != that.is_negative) return false;
   return ubig_value == that.ubig_value;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? that.ubig_value < ubig_value
                      : ubig_value < that.ubig_value;
}

bool bigint::operator> (const bigint& that) const {
   return not (*this < that or *this == that);
}

bool bigint::operator!= (const bigint& that) const {
   return not (*this == that);
}

bool bigint::operator<= (const bigint& that) const {
   return *this < that or *this == that;
}

bool bigint::operator>= (const bigint& that) const {
   return not (*this < that) or *this == that;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << (that.is_negative ? "-" : "")
              << that.ubig_value;
}

