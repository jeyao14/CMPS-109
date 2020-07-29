// $Id: interp.cpp,v 1.3 2019-03-19 16:18:22-07 - - $

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define"   , &interpreter::do_define  },
   {"draw"     , &interpreter::do_draw    },
   {"border"   , &interpreter::do_border  },
   {"moveby"   , &interpreter::do_moveby  },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"       , &interpreter::make_text       },
   {"ellipse"    , &interpreter::make_ellipse    },
   {"circle"     , &interpreter::make_circle     },
   {"polygon"    , &interpreter::make_polygon    },
   {"rectangle"  , &interpreter::make_rectangle  },
   {"square"     , &interpreter::make_square     },
   {"diamond"    , &interpreter::make_diamond    },
   {"triangle"   , &interpreter::make_triangle   },
   {"equilateral", &interpreter::make_equilateral},
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("interpret syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}

// color, name, xcenter, ycenter

void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("draw syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   window::push_back (object (itor->second, where, color));
}

void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   rgbcolor color {begin[0]};
   window::set_border_colour(begin[1]); // Do something here
   window::set_border_width(from_string<GLfloat> (begin[2]));
}

void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   window::set_movement_amount(from_string<GLfloat> (begin[0])); // Do something here
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<text> (nullptr, string());
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto width = begin[0];
   auto height = begin[1];
   return make_shared<ellipse> (stod(width), stod(height));
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto diameter = begin[0];
   return make_shared<circle> (stod(diameter));
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<polygon> (vertex_list());
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto width = begin[0];
   auto height = begin[1];
   return make_shared<rectangle> (stod(width), stod(height));
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto length = begin[0];
   return make_shared<square> (stod(length));
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto width = begin[0];
   auto height = begin[1];
   return make_shared<diamond> (stod(width), stod(height));
}

shape_ptr interpreter::make_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto x0 = begin[0];
   auto y0 = begin[1];
   auto x1 = begin[2];
   auto y1 = begin[3];
   auto x2 = begin[4];
   auto y2 = begin[5];
   return make_shared<triangle> (stod(x0),stod(y0),stod(x1),stod(y1),stod(x2),stod(y2));
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto width = begin[0];
   return make_shared<equilateral> (stod(width));
}
