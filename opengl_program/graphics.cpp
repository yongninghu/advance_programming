// $Id: graphics.cpp,v 1.11 2014-05-15 16:42:55-07 - - $

#include <iostream>
using namespace std;

#include <GL/freeglut.h>

#include "graphics.h"
#include "util.h"

int window::width = 640; // in pixels
int window::height = 480; // in pixels
vector<object> window::objects;
size_t window::selected_obj = 0;
mouse window::mus;
float window::pixel = 4.0f;
int object::nextObj = 0;

// Executed when window system signals to shut down.
void window::close() {
   DEBUGF ('g', sys_info::execname() << ": exit ("
           << sys_info::exit_status() << ")");
   exit (sys_info::exit_status());
}

// Executed when mouse enters or leaves window.
void window::entry (int mouse_entered) {
   DEBUGF ('g', "mouse_entered=" << mouse_entered);
   window::mus.entered = mouse_entered;
   if (window::mus.entered == GLUT_ENTERED) {
      DEBUGF ('g', sys_info::execname() << ": width=" << window::width
           << ", height=" << window::height);
   }
   glutPostRedisplay();
}

// Called to display the objects in the window.
void window::display() {
   glClear (GL_COLOR_BUFFER_BIT);
   for (auto& object: window::objects) {
      object.draw();
      if(object.serial == (int) selected_obj) {
         object.drawBorder();
      }
   }
   mus.draw();
   glutSwapBuffers();
}

// Called when window is opened and when resized.
void window::reshape (int width, int height) {
   DEBUGF ('g', "width=" << width << ", height=" << height);
   window::width = width;
   window::height = height;
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0, window::width, 0, window::height);
   glMatrixMode (GL_MODELVIEW);
   glViewport (0, 0, window::width, window::height);
   glClearColor (0.25, 0.25, 0.25, 1.0);
   glutPostRedisplay();
}


// Executed when a regular keyboard key is pressed.
enum {BS=8, TAB=9, ESC=27, SPACE=32, DEL=127};
void window::keyboard (GLubyte key, int x, int y) {
   DEBUGF ('g', "key=" << (unsigned)key << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   size_t nextObj = 0;
   auto itor = objects.begin();
   if(itor != objects.end()) {
      nextObj = (*itor).getNext();
   }
   switch (key) {
      case 'Q': case 'q': case ESC:
         window::close();
         break;
      case 'H': case 'h':
         (itor[selected_obj]).move(-pixel, 0, width, height);
         break;
      case 'J': case 'j':
         (itor[selected_obj]).move(0, -pixel, width, height);
         break;
      case 'K': case 'k':
         (itor[selected_obj]).move(0, pixel, width, height);
         break;
      case 'L': case 'l':
         (itor[selected_obj]).move(pixel, 0, width, height);
         break;
      case 'N': case 'n': case SPACE: case TAB:
         if(selected_obj == 0) {
            size_t obj = nextObj - 1;
            selected_obj = obj;
         } else --selected_obj;
         break;
      case 'P': case 'p': case BS:
         if(selected_obj == nextObj - 1) {
            selected_obj = 0;
         } else ++selected_obj;
         break;
      case '0'...'9':
         if(key - '0' < (int)nextObj){ selected_obj = key - '0';}
         break;
      default:
         cerr << (unsigned)key << ": invalid keystroke" << endl;
         break;
   }
   glutPostRedisplay();
}


// Executed when a special function key is pressed.
void window::special (int key, int x, int y) {
   DEBUGF ('g', "key=" << key << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   size_t nextObj = 0;
   auto itor = objects.begin();
   if(itor != objects.end()) {
      nextObj = (*itor).getNext();
   }
   switch (key) {
      case GLUT_KEY_LEFT: 
           (itor[selected_obj]).move(-pixel, 0, width, height); break;
      case GLUT_KEY_DOWN: 
           (itor[selected_obj]).move(0, -pixel, width, height); break;
      case GLUT_KEY_UP: 
           (itor[selected_obj]).move(0, pixel, width, height); break;
      case GLUT_KEY_RIGHT: 
           (itor[selected_obj]).move(pixel, 0, width, height); break;
      case GLUT_KEY_F1: 
           if(1 < nextObj){ selected_obj = 1;} break;
      case GLUT_KEY_F2: 
           if(2 < nextObj){ selected_obj = 2;} break;
      case GLUT_KEY_F3:
           if(3 < nextObj){ selected_obj = 3;} break;
      case GLUT_KEY_F4:
           if(4 < nextObj){ selected_obj = 4;} break;
      case GLUT_KEY_F5:
           if(5 < nextObj){ selected_obj = 5;} break;
      case GLUT_KEY_F6: 
           if(6 < nextObj){ selected_obj = 6;} break;
      case GLUT_KEY_F7: 
           if(7 < nextObj){ selected_obj = 7;} break;
      case GLUT_KEY_F8:
           if(8 < nextObj){ selected_obj = 8;} break;
      case GLUT_KEY_F9:
           if(9 < nextObj){ selected_obj = 9;} break;
      case GLUT_KEY_F10:
           if(10 < nextObj){ selected_obj = 10;} break;
      case GLUT_KEY_F11: 
           if(11 < nextObj){ selected_obj = 11;} break;
      case GLUT_KEY_F12:
           if(12 < nextObj){ selected_obj = 12;} break;
      default:
         cerr << (unsigned)key << ": invalid function key" << endl;
         break;
   }
   glutPostRedisplay();
}


void window::motion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::passivemotion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::mousefn (int button, int state, int x, int y) {
   DEBUGF ('g', "button=" << button << ", state=" << state
           << ", x=" << x << ", y=" << y);
   window::mus.state (button, state);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::main () {
   static int argc = 0;
   glutInit (&argc, nullptr);
   glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize (window::width, window::height);
   glutInitWindowPosition (128, 128);
   glutCreateWindow (sys_info::execname().c_str());
   glutCloseFunc (window::close);
   glutEntryFunc (window::entry);
   glutDisplayFunc (window::display);
   glutReshapeFunc (window::reshape);
   glutKeyboardFunc (window::keyboard);
   glutSpecialFunc (window::special);
   glutMotionFunc (window::motion);
   glutPassiveMotionFunc (window::passivemotion);
   glutMouseFunc (window::mousefn);
   DEBUGF ('g', "Calling glutMainLoop()");
   glutMainLoop();
}


void mouse::state (int button, int state) {
   switch (button) {
      case GLUT_LEFT_BUTTON: left_state = state; break;
      case GLUT_MIDDLE_BUTTON: middle_state = state; break;
      case GLUT_RIGHT_BUTTON: right_state = state; break;
   }
}

void mouse::draw() {
   static rgbcolor color ("green");
   ostringstream text;
   text << "(" << xpos << "," << window::height - ypos << ")";
   if (left_state == GLUT_DOWN) text << "L"; 
   if (middle_state == GLUT_DOWN) text << "M"; 
   if (right_state == GLUT_DOWN) text << "R"; 
   if (entered == GLUT_ENTERED) {
      void* font = GLUT_BITMAP_HELVETICA_18;
      glColor3ubv (color.ubvec);
      glRasterPos2i (10, 10);
      glutBitmapString (font, (GLubyte*) text.str().c_str());
   }
}

