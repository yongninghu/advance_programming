// $Id: shape.h,v 1.6 2014-05-21 15:44:26-07 - - $

#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <iomanip>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
using namespace std;

#include "rgbcolor.h"

//
// Shapes constitute a single-inheritance hierarchy, summarized
// here, with the superclass listed first, and subclasses indented
// under their immediate superclass.
//
// shape
//    text
//    ellipse
//       circle
//    polygon
//       rectangle
//          square
//       diamond
//       triangle
//          right_triangle
//          isosceles
//          equilateral
//

class shape;
typedef struct {GLfloat xpos; GLfloat ypos; } vertex;
typedef vector<vertex> vertex_list;
typedef shared_ptr<shape> shape_ptr; 

//
// Abstract base class for all shapes in this system.
//

class shape {
   friend ostream& operator<< (ostream& out, const shape&);
   private:
      shape (const shape&) = delete; // Prevent copying.
      shape& operator= (const shape&) = delete; // Prevent copying.
      shape (shape&&) = delete; // Prevent moving.
      shape& operator= (shape&&) = delete; // Prevent moving.
   protected:
      inline shape(); // Only subclass may instantiate.
   public:
      static rgbcolor border;
      static int thick;
      virtual ~shape() {}
      virtual void draw (const vertex&, const rgbcolor&, 
                        int objNum) const = 0;
      virtual void drawBorder(const vertex&) const = 0;
      virtual void show (ostream&) const;
};


//
// Class for printing text.
//

class text: public shape {
   protected:
      void* glut_bitmap_font = nullptr;
      // GLUT_BITMAP_8_BY_13
      // GLUT_BITMAP_9_BY_15
      // GLUT_BITMAP_HELVETICA_10
      // GLUT_BITMAP_HELVETICA_12
      // GLUT_BITMAP_HELVETICA_18 
      // GLUT_BITMAP_TIMES_ROMAN_10
      // GLUT_BITMAP_TIMES_ROMAN_24
      string textdata;
   public:
      text (void* glut_bitmap_font, const string& textdata);
      virtual void draw (const vertex&, const rgbcolor&, 
                        int objNum) const override;
      virtual void show (ostream&) const override;
      virtual void drawBorder(const vertex&) const override;
};

//
// Classes for ellipse and circle.
//

class ellipse: public shape {
   protected:
      vertex dimension;
   public:
      ellipse (GLfloat width, GLfloat height);
      virtual void draw (const vertex&, const rgbcolor&, 
                        int objNum) const override;
      virtual void show (ostream&) const override;
      virtual void drawBorder(const vertex&) const override;
};

class circle: public ellipse {
   public:
      circle (GLfloat diameter);
};

//
// Class polygon.
//

class polygon: public shape {
   protected:
      vertex_list vertices;
   public:
      polygon (const vertex_list& vertices);
      virtual void draw (const vertex&, const rgbcolor&, 
                        int objNum) const override;
      virtual void show (ostream&) const override;
      virtual void drawBorder(const vertex&) const override;
      void normalize_center();
};


//
// Classes rectangle, square, etc.
//

class rectangle: public polygon {
   public:
      rectangle (GLfloat width, GLfloat height);
};

class square: public rectangle {
   public:
      square (GLfloat width);
};

class diamond: public polygon {
   public:
      diamond (const GLfloat width, const GLfloat height);
};

class right_triangle: public polygon {
   public:
      right_triangle (const GLfloat width, const GLfloat height);
};

class isosceles: public polygon {
   public:
      isosceles (const GLfloat width, const GLfloat height);
};

class equilateral: public polygon {
   public:
      equilateral (const GLfloat side);
};

ostream& operator<< (ostream& out, const shape&);

#endif

