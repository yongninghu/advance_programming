// $Id: interp.cpp,v 1.14 2014-05-08 18:32:56-07 - - $

#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

map<string,interpreter::interpreterfn> interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"moveby" , &interpreter::do_moveby },
   {"border" , &interpreter::do_border },
};

map<string,interpreter::factoryfn> interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"diamond"  , &interpreter::make_diamond  },
   {"triangle" , &interpreter::make_polygon  },
   {"right_triangle" , &interpreter::make_right_triangle },
   {"isosceles", &interpreter::make_isosceles},
   {"equilateral" , &interpreter::make_equilateral},
};

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
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   (this->*func) (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.insert ({name, make_shape (++begin, end)});
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   rgbcolor color {begin[0]};
   object newShape = object(itor->second, where, color);
   window::push_back(newShape);
}

void interpreter::do_border(param begin, param end) {
   DEBUGF ('f', range (begin, end));
   rgbcolor color{begin[0]};
   shape::border = color;
   int thickness = stoi(begin[1]);
   shape::thick = thickness; 
}

void interpreter::do_moveby(param begin, param end) {
   DEBUGF ('f', range (begin, end));
   float pixel = stof(begin[0]);
   window::setpixel(pixel);
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return (this->*func) (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin < 2) throw runtime_error("syntax error");

   string font = *begin++;
   string textdata = *begin++;
   void* fontname = &font;
   for(; begin != end; ++begin) {
      textdata = textdata + " " + *begin;
   }
   return make_shared<text> (fontname, textdata);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = stoi(*begin++);
   GLfloat height = stoi(*begin);
   return make_shared<ellipse> (width, height);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat diam = stoi(*begin);
   return make_shared<circle> (diam);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list result;
   while (begin != end) {
      float x = stof(*begin++);
      float y = stof(*begin++);
      result.push_back({x, y});
   }
   return make_shared<polygon> (result);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = stoi(*begin++);
   GLfloat height = stoi(*begin);
   return make_shared<rectangle> (width, height);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat side = stoi(*begin);
   return make_shared<square> (side);
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = stoi(*begin++);
   GLfloat height = stoi(*begin);
   return make_shared<diamond> (width, height);
}

shape_ptr interpreter::make_right_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = stoi(*begin++);
   GLfloat height = stoi(*begin);
   return make_shared<right_triangle> (width, height);
}

shape_ptr interpreter::make_isosceles (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = stoi(*begin++);
   GLfloat height = stoi(*begin);
   return make_shared<isosceles> (width, height);
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat side = stoi(*begin);
   return make_shared<equilateral> (side);
}







