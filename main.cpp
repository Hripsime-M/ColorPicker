#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#define radius 40
#define TAB 10
#define SIZE 10
#define PI 3.1415926535898

GLint WIDTH = 900.0, HEIGHT = 600;
GLfloat left = -45, right = 120, down = -50, top = 70;
GLfloat R, G, B, H = 0, S = 0, V = 100;
int z = 0;

struct HSV {
  HSV(GLfloat _h = H, GLfloat _s = S, GLfloat _v = V) :
    h(_h), s(_s), v(_v) {}
  GLfloat h;
  GLfloat s;
  GLfloat v;
};

struct RGB {
  RGB(GLfloat _r = 1, GLfloat _g = 1, GLfloat _b = 1) :
    r(_r), g(_g), b(_b) {}
  GLfloat r;
  GLfloat g;
  GLfloat b;
};

struct Point {
  Point(GLfloat _x = 0, GLfloat _y = 0, RGB _rgb = RGB()) :
    x(_x), y(_y) {
      rgb.r = _rgb.r;
      rgb.g = _rgb.g;
      rgb.b = _rgb.b;
    }
  GLfloat x;
  GLfloat y;
  RGB rgb;
};

std::vector<Point*> points;

RGB __rgb;

/// This function does the convertion between HSV and RGB
/// systems
void HSV2RGB(HSV hsv, RGB& rgb)
{
  GLfloat h = hsv.h, s = hsv.s, v = hsv.v;
  GLfloat r = rgb.r, g = rgb.g, b = rgb.b;
  if (h >= 360 || h < 0 || s > 100 || s < 0 || v > 100 || v < 0) {
    printf("Wrong params in hsv2rgb function!\n");
    return;
  }
  GLfloat hi, a, vmin, vinc, vdec;
  hi = (int)h / 60;
  vmin = (100 - s) * v / 100;
  a = (h - (int(h / 60)) * 60) / 60 * (v - vmin);
  vinc = vmin + a;
  vdec = v - a;
  if (hi == 0) {
    r = v;
    g = vinc;
    b = vmin;
  } else if (hi == 1) {
    r = vdec;
    g = v;
    b = vmin;
  } else if (hi == 2) {
    r = vmin;
    g = v;
    b = vinc;
  } else if (hi == 3) {
    r = vmin;
    g = vdec;
    b = v;
  } else if (hi == 4) {
    r = vinc;
    g = vmin;
    b = v;
  } else {
    r = v;
    g = vmin;
    b = vdec;
  }
  rgb.r = r / 100;
  rgb.g = g / 100;
  rgb.b = b / 100;
}

void setColor(RGB rgb) {
  glColor3f(rgb.r, rgb.g, rgb.b);
}

/// This function checks whether the given point with
/// x and y coordinates is in the drawing area
bool isInDrawingArea(GLfloat x_, GLfloat y_) {
  if (x_ <= radius + TAB + SIZE ||
      y_ >= radius + TAB) {
    return false;
  } else {
    return true;
  }
}

/// This function converts the given x and y coordinates to
/// the corresponding hue and saturation
void xy2hs(GLfloat x, GLfloat y, GLfloat& h, GLfloat& s) {
  s = sqrt(x * x + y * y);
  if (s == 0) {
    h = 0;
    return;
  }
  if (y >= 0) {
    h = acos(x/s);		
  } else {
    h = 2 * PI - acos(x / s);
  }
  s = s / radius * 100;
  h = h * 180 / PI;
  if (s - 100 < 0.01 && s - 100 >= 0) {
    s = 100;
  }
  if (h - 360 < 0.01 && h - 360 >= 0) {
    h = 0;
  }
}

/// Do some trivial initialization
void init(void) {
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glPointSize(10.0);
}

/// This function draws the points accumulated in the points vector
/// during the mouse move
void drawPoints() {
  glBegin( GL_POINTS );
  for (int i = 0; i < z; i++) {
    Point* p = points[i];
    setColor(p->rgb);
    glVertex2f(p->x, p->y);
  }
  glEnd();
}

/// This is the opengl display function
void display(void) {
  HSV hsv;
  hsv.v = V;
  RGB rgb;

  GLfloat h, s, v;
  glClear(GL_COLOR_BUFFER_BIT);
  glPushMatrix();
  glColor3f(0.0, 0.0, 0.0);

  /// Draw the circle for selecting hue and saturation
  glBegin(GL_TRIANGLE_FAN);
  xy2hs(0, 0, hsv.h, hsv.s);
  HSV2RGB(hsv, rgb);
  setColor(rgb);
  glVertex2f(0, 0);
  GLint circle_points = 1000;
  GLfloat angle;
  for(GLint i = 0; i < circle_points - 1; i++) {
    angle = 2 * PI * i / circle_points;
    xy2hs(cos(angle) * radius, sin(angle) * radius, h, s);
    HSV2RGB(HSV(h, s, V), rgb);
    setColor(rgb);
    glVertex2f(cos(angle) * radius, sin(angle) * radius);
  }
  xy2hs(cos(0.0) * radius, sin(0.0) * radius, h, s);
  HSV2RGB(HSV(h, s, V), rgb);
  setColor(rgb);
  glVertex2f(cos(0.0) * radius, sin(0.0) * radius);
  glEnd();

  glPopMatrix();

  /// Draw the rectangle for selecting the value parameter
  glBegin(GL_POLYGON);
  HSV2RGB(HSV(H, S, 100), rgb);
  setColor(rgb);
  glVertex2f(radius, radius + TAB);
  glVertex2f(radius, radius + TAB + SIZE);
  HSV2RGB(HSV(H, S, 0), rgb);
  setColor(rgb);
  glVertex2f(-1 * radius, radius + TAB + SIZE);
  glVertex2f(-1 * radius, radius + TAB);
  glEnd();

  /// Draw the points accumulated from the mouse move
  drawPoints();

  /// Draw the rectangle with resulting color
  glBegin(GL_POLYGON);
  HSV2RGB(HSV(H,S,V), rgb);	
  setColor(rgb);
  glVertex2f(radius + TAB, radius + TAB * 2);
  glVertex2f(radius + TAB * 7 + SIZE, radius + TAB * 2);
  glVertex2f(radius + TAB * 7 + SIZE, radius + TAB * 2 + SIZE);
  glVertex2f(radius + TAB, radius + TAB * 2 + SIZE);
  glEnd();

  glutSwapBuffers();
  glutPostRedisplay();
}

/// OpenGL reshape functionality
void reshape(int w, int h) {
  glViewport(0,0,(GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(left, right, down, top, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/// This function converts the mouse x and y coordinates to
/// our window coordinates
void convertXY2OpenGLXY(int x, int y,
                        GLfloat& oglX, GLfloat& oglY) {
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  oglX = ((float)x) / (float)viewport[2] * (right - left);
  oglY = ((float)(viewport[3] - y)) / viewport[3] * (top - down);
  oglX += left;
  oglY += down;
}

/// This function track the clicks of mouse buttons and 
/// if the clicked point is within the color selection ranges
/// (either HS circle or V rectangle), then it saves the color
void mouse(int button, int state, int x, int y) {
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
    GLfloat x_, y_;
    convertXY2OpenGLXY(x, y, x_, y_);
    GLfloat t = x_ * x_ + y_ * y_ - 0.001;
    if (t <= radius * radius) {
      xy2hs(x_, y_, H, S);
      HSV2RGB(HSV(H, S, V), __rgb);
      glutPostRedisplay();
    } else if (y_ > radius + TAB && y_< radius + TAB + SIZE && abs(x_) <= radius) {
      V = float(x_ + radius) / (float)(2 * radius) * 100;
      HSV2RGB(HSV(H, S, V), __rgb);
      glutPostRedisplay();
    } else {
      glutIdleFunc(NULL);
    }
  }
}

/// This function tracks the mouse movement while
/// either of buttons are kept pressed and accumulates
/// the trajectory of the movement in the points vector
void mouseMove(int x, int y) {
  GLfloat x_, y_;
  convertXY2OpenGLXY(x, y, x_, y_);
  if (!isInDrawingArea(x_, y_)) {
    return;
  }
  Point* p = new Point(x_, y_, __rgb);
  points.push_back(p);
  z++;
}

/// This function processes the keyboard events.
void windowKey(unsigned char key, int x, int y) {
  /// Exit on ESC and clear the drawing in Space
  if (key == 27) {
    exit(0);
  } else if (key == 32) {
    for (int i = 0; i < z; ++i) {
      delete points[i];
    }
    points.clear();
    z = 0;
  }
  glutPostRedisplay();
}

int main(int argc, char **argv)
{
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
  glutInitWindowSize(WIDTH,HEIGHT);
  glutInitWindowPosition(100,100);
  glutCreateWindow("Color picker");
  glutKeyboardFunc(windowKey);

  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMove);
  glutMainLoop();
  return 0;
}
