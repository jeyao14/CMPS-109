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
   DEBUGF ('~', this << " -> " << ubig_value)
}

bigint::bigint (const ubigint& ubig_value_, bool is_negative_):
                ubig_value(ubig_value_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   ubig_value = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {ubig_value, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
  //ubigint result = ubig_value + that.ubig_value;
  //return result;
  if(that.is_negative && is_negative){ //if both signs are negative
      return {ubig_value + that.ubig_value, true};//return the result
  }else if(that.is_negative || is_negative){ 
  //if one sign is negative and the other is not
      if(is_negative){ //if left side is negative
        if(ubig_value <= that.ubig_value){ //if the right side is larger
          return that.ubig_value - ubig_value;
        }else{
          return {ubig_value-that.ubig_value, true};
        }
      }else if(that.is_negative){ //if right side is negative
        if(ubig_value <= that.ubig_value){ //if the right side is larger
          return {that.ubig_value - ubig_value, true};
        }else{
          return ubig_value - that.ubig_value;
        }
      }
  }
  return ubig_value + that.ubig_value;
}

//-5 + 8 = 8 - 5
//-8 + 5 = 5 - 8
//5 + -8 = -(8 - 5)


bigint bigint::operator- (const bigint& that) const {
  if(that.is_negative && is_negative){ //if both signs are negative
        if(ubig_value <= that.ubig_value){ //if the right side is larger
          return that.ubig_value - ubig_value;
        }else{
          return {ubig_value - that.ubig_value, true};
        }
  }else if(!is_negative && !that.is_negative){
  //if both signs are positive
    if(ubig_value >= that.ubig_value){ //if the left side is larger
      return ubig_value - that.ubig_value;//return the result
    }else{
      return {that.ubig_value - ubig_value, true};
    }
  }else if(that.is_negative || is_negative){
  //if one sign is negative and the other is not
    if(is_negative){
      return {ubig_value + that.ubig_value, true};
    }else if(that.is_negative){
      return ubig_value + that.ubig_value;
    }
  }
  return ubig_value - that.ubig_value;
}

//-5 - -8 = 8 - 5
//5 - 8 = 8 - 5
//-8 - -5 = - (8 - 5)
// -3 - 4 = -(4 + 3)
// 3 - -4 = 4 + 3


bigint bigint::operator* (const bigint& that) const {
  //bigint result = ubig_value * that.ubig_value;
  //return result;
  if(is_negative && that.is_negative){//if both signs are negative
    return ubig_value * that.ubig_value; //both are positive
  }else if(that.is_negative || is_negative){
    return {ubig_value * that.ubig_value, true};
  }
  return ubig_value * that.ubig_value;
}

bigint bigint::operator/ (const bigint& that) const {
  if(is_negative == that.is_negative){
    return {ubig_value/that.ubig_value};
  }
  return {ubig_value/that.ubig_value, true};
}

bigint bigint::operator% (const bigint& that) const {
  return {ubig_value % that.ubig_value, is_negative};
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == 
    that.is_negative and ubig_value == that.ubig_value;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? ubig_value > that.ubig_value
                      : ubig_value < that.ubig_value;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << (that.is_negative ? "-" : "")
              << that.ubig_value;
}

