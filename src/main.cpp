#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "project.hpp"
#include "algebra.hpp"
#include "a2.hpp"

#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#ifdef WIN32
#include "glui.h"
#else
//#include <GL/glui.h>
#endif /* WIN32 */

//-------------------------------------------------------------------
// defines
//-------------------------------------------------------------------
#define CALLBACK_QUIT 'q'

#define MICKEY 1
//-------------------------------------------------------------------
// GLUT data 
//-------------------------------------------------------------------
int scrWidth, scrHeight;
float cur_x = -1;
float cur_y = 0;
int buttons[3] = {GLUT_UP, GLUT_UP, GLUT_UP};

Vector3D look_direction;
Vector3D right_direction;

double x_trans = 0, y_trans = 0, z_trans = -200;
double x_rot = 0, y_rot = 0;

Project *project;

void init_lights() {
  GLfloat light_position1[] = {0, 50, 0, 1};
  GLfloat light1[] = {0.5, 0.5, 0.5, 0};
  GLfloat light2[] = {0.5, 0.5, .5, 0};

  // setup 
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 25);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light2);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light1);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light2);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
}

void motion(int x, int y) {
    if(cur_x<0){
	cur_x = x;
	cur_y = y;
    }
 
    if(buttons[0] == GLUT_DOWN) {
      x_trans  += (double)(x-cur_x)/5.0f;
      y_trans  += -(double)(y-cur_y)/5.0f;
    }

    if(buttons[1] == GLUT_DOWN) {
      z_trans  += -(double)(y-cur_y)/5.0f;
    }

    if(buttons[2] == GLUT_DOWN) {
      double rot_x_change = (double)(y-cur_y)/5.0;
      double rot_y_change = (double)(x-cur_x)/5.0;

      x_rot  += rot_x_change;
      y_rot  += rot_y_change;

      x_rot = (x_rot < 0) ? x_rot + 360 : x_rot;
      x_rot = (x_rot > 360) ? x_rot - 360 : x_rot;

      y_rot = (y_rot < 0) ? y_rot + 360 : y_rot;
      y_rot = (y_rot > 360) ? y_rot - 360 : y_rot;

      look_direction = rotation(x_rot, 'x')*Vector3D(0, 0, -1);
      look_direction = rotation(y_rot, 'y')*look_direction;

      right_direction = rotation(-90, 'y')*look_direction;

    }

    cur_x = x;
    cur_y = y;
 

  //glutPostRedisplay();
}

//-------------------------------------------------------------------
// mouse 
//-------------------------------------------------------------------
void mouse(int button, int state, int x, int y) {
  if(button < 3) {
    if(buttons[0] == GLUT_UP && buttons[1] == GLUT_UP && buttons[2] == GLUT_UP) {
      cur_x = -1;
      cur_y = -1;
    }
    buttons[button] = state;
  }
}
  
void render(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glLoadIdentity(); 

  glRotatef(x_rot, 1, 0, 0);
  glRotatef(y_rot, 0, 1, 0);
  glTranslatef(x_trans, y_trans, z_trans);

  GLfloat ambient[] = {1.0f, 1.0f, 1.0f, 0.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

  GLfloat light_color[] = {1.0f, 1.0f, 1.0f, 0.0f};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);

  GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0};
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

/*
  glColor3f(0.7,0.7,0.7);

  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-10, -2, 10);
  glVertex3f(10, -2, 10);
  glVertex3f(10, -2, -10);
  glVertex3f(-10, -2, -10);
  glEnd();
*/

  project->render();
}

void display(void) {
  project->update();

  /* set up for perspective drawing */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, scrWidth, scrHeight);
  gluPerspective(40.0, (GLfloat)scrWidth/(GLfloat)scrHeight, 0.1, 3000.0);
	
  /* change to model view for drawing */
  glMatrixMode(GL_MODELVIEW);

  /* Reset modelview matrix */
  glLoadIdentity();

  // Set up lighting
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glClearDepth(1.0f);	
  glShadeModel(GL_SMOOTH);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  render();

  glFinish();
  glutSwapBuffers();

  glutPostRedisplay();
}

void keyboard(unsigned char k, int x, int y) {
  switch(k) {
    case CALLBACK_QUIT:
      project->destroy();
      exit(0);
      break;
    case 'w':
      x_trans += look_direction[0]*3.0;
      y_trans += look_direction[1]*3.0;
      z_trans -= look_direction[2]*3.0;
      break;
    case 's':
      x_trans -= look_direction[0]*3.0;
      y_trans -= look_direction[1]*3.0;
      z_trans += look_direction[2]*3.0;
      break;
    case 'd':
      x_trans -= right_direction[0]*3.0;
      z_trans += right_direction[2]*3.0;
      break;
    case 'a':
      x_trans += right_direction[0]*3.0;
      z_trans -= right_direction[2]*3.0;
      break;
    case 'b':
      project->switch_binlattice();
  }

  glutPostRedisplay();
}

void init(int argc, char** argv) {
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);

  // Black Background
  glClearColor(0.20f, 0.20f, 0.20f, 0.0f);			
	
  glEnable(GL_DEPTH_TEST);					

  init_lights();
	
}

void reshape(int w, int h) {
  scrWidth = w;
  scrHeight = h;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, w, h);
  gluPerspective(40.0, (GLfloat)w/(GLfloat)h, 0.1, 3000.0);

  /* Reset to modelview matrix mode to avoid confusion later */
  glMatrixMode(GL_MODELVIEW);
  glutPostRedisplay();

}

int main(int argc, char** argv){
    bool result;
    int main_window;

    scrWidth = 800;
    scrHeight = 600;

    look_direction = Vector3D(0, 0, -1);
    right_direction = Vector3D(1, 0, 0);

    project = new Project();

    // intialize glut and main window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH|GLUT_STENCIL);
    glutInitWindowSize(scrWidth, scrHeight);
    main_window = glutCreateWindow("Render Test");
	
    // initialize callback
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    result = project->initialize(argc, argv);
    if(!result) return 0;

    reshape(scrWidth, scrHeight);

    glutMainLoop();
  
    return 0;
}

