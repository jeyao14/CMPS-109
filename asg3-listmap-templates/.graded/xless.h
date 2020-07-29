// $Id: xless.h,v 1.1 2019-11-09 20:18:13-08 - - $
// Jeffrey Yao jeyao
// Herman Wu hwwu

#ifndef __XLESS_H__
#define __XLESS_H__

//
// We assume that the type type_t has an operator< function.
//

template <typename Type>
struct xless {
   bool operator() (const Type& left, const Type& right) const {
      return left < right;
   }
};

#endif

