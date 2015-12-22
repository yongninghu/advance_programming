// $Id: shape.cpp,v 1.7 2014-05-08 18:32:56-07 - - $
#include <cmath>
#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"

rgbcolor shape:: border{"red"};
int shape:: thick{4};

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

text::text (void* glut_bitmap_font, const string& textdata):
      textdata(textdata) {
   DEBUGF ('c', this);
   string* font = static_cast<string*>(glut_bitmap_font);
   auto itor = fontcode.find(*font);
   this->glut_bitmap_font = itor->second;
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
   normalize_center();
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   
   vertex botleft({0, 0});
   vertex topleft({0, height});
   vertex botright({width, 0});
   vertex topright({width, height});
   
   vertices.push_back(topleft);
   vertices.push_back(topright);
   vertices.push_back(botright);
   vertices.push_back(botleft);

   normalize_center();
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond(const GLfloat width, const GLfloat height):
          polygon({}) {
   vertex left({0, height * .5f});
   vertex right({width, height * .5f});
   vertex top({width * .5f, height});
   vertex bot({width * .5f, 0});

   vertices.push_back(top);
   vertices.push_back(right);
   vertices.push_back(bot);
   vertices.push_back(left);
   normalize_center();
}

right_triangle::right_triangle(const GLfloat width, 
                const GLfloat height): polygon({}) {
   vertex left({0, 0});
   vertex rtop({width, height});
   vertex rbot({width, 0});
 
   vertices.push_back(left);
   vertices.push_back(rtop);
   vertices.push_back(rbot);

   normalize_center();
}

isosceles::isosceles(const GLfloat width, const GLfloat height):
                 polygon({}) {
   vertex left({0, 0});
   vertex rtop({width * .5f, height});
   vertex rbot({width, 0});

   vertices.push_back(left);
   vertices.push_back(rtop);
   vertices.push_back(rbot);

   normalize_center();
}

equilateral::equilateral(const GLfloat side):
                 polygon({}) {
   vertex top({side * .5f, side});
   vertex bleft({0, 0});
   vertex bright({side, 0});

   vertices.push_back(top);
   vertices.push_back(bleft);
   vertices.push_back(bright);
 
   normalize_center();
}

void text::draw (const vertex& center, const rgbcolor& color, 
                int objNum) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   int str_width = glutBitmapLength (glut_bitmap_font,
                   (GLubyte*) textdata.c_str());
   int str_height = glutBitmapHeight (glut_bitmap_font);

   glColor3ubv(color.ubvec);

   float xpos = center.xpos - str_width / 2.0;
   float ypos = center.ypos - str_height / 4.0;
   glRasterPos2f (xpos, ypos);

   for(auto ch: textdata) glutBitmapCharacter(glut_bitmap_font, ch);

   glColor3ubv(rgbcolor().ubvec);
   
   glRasterPos2f(center.xpos - 4.0f, ypos);
   int cha = objNum + '0';
   glutBitmapCharacter(GLUT_BITMAP_8_BY_13, cha);
}

void text::drawBorder(const vertex& center) const {
   glLineWidth(thick);
   glBegin(GL_LINE_LOOP);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(border.ubvec);

   int str_width = glutBitmapLength (glut_bitmap_font,
                   (GLubyte*) textdata.c_str());
   int str_height = glutBitmapHeight (glut_bitmap_font);

   vertex_list borders;
   borders.push_back({center.xpos - str_width * 0.5f,
                     center.ypos + str_height * 0.5f});
   borders.push_back({center.xpos + str_width * 0.5f,
                     center.ypos + str_height * 0.5f});
   borders.push_back({center.xpos + str_width * 0.5f,
                     center.ypos - str_height * 0.5f});
   borders.push_back({center.xpos - str_width * 0.5f,
                     center.ypos - str_height * 0.5f});

   for(auto each : borders) { glVertex2f(each.xpos, each.ypos); }

   glEnd();

}

void ellipse::draw (const vertex& center, const rgbcolor& color, 
                   int objNum) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   const float delta = 2 * M_PI / 32;
   float width = this->dimension.xpos / 2;
   float height = this->dimension.ypos / 2;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = width * cos(theta) + center.xpos;
      float ypos = height * sin(theta) + center.ypos;
      glVertex2f(xpos, ypos);
   }

   glColor3ubv(rgbcolor().ubvec);

   glEnd();

   glRasterPos2f(center.xpos - 4.0f, center.ypos - 6.5f);
   int cha = objNum + '0';
   glutBitmapCharacter(GLUT_BITMAP_8_BY_13, cha);
}

void ellipse::drawBorder(const vertex& center) const {
   glLineWidth(thick);
   glBegin(GL_LINE_LOOP);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(border.ubvec);
   const float delta = 2 * M_PI / 32;
   float width = dimension.xpos * 0.5f;
   float height = dimension.ypos * 0.5f;
   for(float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = width * cos(theta) + center.xpos;
      float ypos = height * sin(theta) + center.ypos;
      glVertex2f(xpos, ypos);
   }
   glEnd();
}

void polygon::draw (const vertex& center, const rgbcolor& color, 
                   int objNum) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   for(auto itor = vertices.begin(); itor != vertices.end(); ++itor) {
      float xpos = center.xpos + (*itor).xpos;
      float ypos = center.ypos + (*itor).ypos;
      glVertex2f(xpos, ypos);
   }
   glColor3ubv(rgbcolor().ubvec);

   glEnd();

   glRasterPos2f(center.xpos - 4.0f, center.ypos - 6.5f);
   int cha = objNum + '0';
   glutBitmapCharacter(GLUT_BITMAP_8_BY_13, cha);
}

void polygon::drawBorder(const vertex& center) const {
   glLineWidth(thick);
   glBegin(GL_LINE_LOOP);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(border.ubvec);
   for(auto v: vertices) {
      float xpos = center.xpos + v.xpos;
      float ypos = center.ypos + v.ypos;
      glVertex2f(xpos, ypos);
   }
   glEnd();

}

void polygon::normalize_center () {
   float xtotal, ytotal = 0.0f;
   float xoffset, yoffset = 0.0f;
   for(auto itor = vertices.begin(); itor != vertices.end(); ++itor) {
      xtotal += (*itor).xpos;
      ytotal += (*itor).ypos;
   }
   xoffset = xtotal / vertices.size();
   yoffset = ytotal / vertices.size();

   for(auto itor = vertices.begin(); itor != vertices.end(); ++itor) {
      (*itor).xpos -= xoffset;
      (*itor).ypos -= yoffset;
   }
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

