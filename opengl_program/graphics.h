// $Id: graphics.h,v 1.9 2014-05-15 16:42:55-07 - - $

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <memory>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "rgbcolor.h"
#include "shape.h"

class object {
   private:
      shared_ptr<shape> pshape;
      vertex center;
      rgbcolor color;
      static int nextObj;
   public:
      int serial;
      // Default copiers, movers, dtor all OK.
      object(const shared_ptr<shape>& pshape, const vertex& center, 
            const rgbcolor& color) :pshape(pshape), center(center), 
            color(color), serial(nextObj++){};
      void draw() { pshape->draw (center, color, serial); }
      void drawBorder() { pshape->drawBorder(center); }
      void move (GLfloat delta_x, GLfloat delta_y, int width, 
                int height) {
         center.xpos += delta_x;
         if(center.xpos > width) center.xpos = 0;
         if(center.xpos < 0) center.xpos = width;
         center.ypos += delta_y;
         if(center.ypos > height) center.ypos = 0;
         if(center.ypos < 0) center.ypos = height;
      }
      size_t getNext() {
         return nextObj;
      }
};

class mouse {
      friend class window;
   private:
      int xpos {0};
      int ypos {0};
      int entered {GLUT_LEFT};
      int left_state {GLUT_UP};
      int middle_state {GLUT_UP};
      int right_state {GLUT_UP};
   private:
      void set (int x, int y) { xpos = x; ypos = y; }
      void state (int button, int state);
      void draw();
};


class window {
      friend class mouse;
   private:
      static float pixel;
      static int width;         // in pixels
      static int height;        // in pixels
      static vector<object> objects;
      static size_t selected_obj;
      static mouse mus;
   private:
      static void close();
      static void entry (int mouse_entered);
      static void display();
      static void reshape (int width, int height);
      static void keyboard (GLubyte key, int, int);
      static void special (int key, int, int);
      static void motion (int x, int y);
      static void passivemotion (int x, int y);
      static void mousefn (int button, int state, int x, int y);
   public:
      static void setpixel(float pixel_) {pixel = pixel_;};
      static void push_back (const object& obj) {
                  objects.push_back (obj); }
      static void setwidth (int width_) { width = width_; }
      static void setheight (int height_) { height = height_; }
      static void main();
};

#endif

