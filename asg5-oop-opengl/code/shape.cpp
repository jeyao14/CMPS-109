// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $

#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({{0,0},{width, 0},{width, height},{0, height}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height): 
            polygon ({{width/2, 0},{width, height/2},{width/2, height},{0, height/2}}) {
   DEBUGF ('c', this);
}

triangle::triangle (GLfloat x0, GLfloat y0, GLfloat x1, 
            GLfloat y1, GLfloat x2, GLfloat y2): polygon ({{x0, y0},{x1, y1},{x2, y2}}) {
   DEBUGF ('c', this);
}

equilateral::equilateral (GLfloat width): triangle (0, 0, width, 0, 
            width/2, sqrt(pow(width,2) - pow(width/2, 2))) {
   DEBUGF ('c', this);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   // Used from the circles.cpp
   const size_t points = 40;
   const GLfloat theta = 2.0 * M_PI / points;
   for (size_t point = 0; point < points; ++point) {
      GLfloat angle = point * theta;
      GLfloat xpos = center.xpos + dimension.xpos * cos (angle);
      GLfloat ypos = center.ypos + dimension.ypos * sin (angle);
      glVertex2f (xpos, ypos);
   }
   glEnd();
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   for(vertex curr_vert : vertices){
      auto xpos = center.xpos + curr_vert.xpos;
      auto ypos = center.ypos + curr_vert.ypos;
      glVertex2f(xpos, ypos);
   }
   glEnd();
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

