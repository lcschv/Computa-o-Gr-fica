

#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <math.h>
#include <iostream>

#define PI 3.14159265358979

#ifndef GLOBALMINE_H
#define GLOBALMINE_H



/// Variaveis
float xpos = 0, ypos = 0, zpos = 0;
float xvet = 0, yvet = 0, zvet = 0;
float yrot = 0, xrot = 90;
float olhoX = 5, olhoY = 5, olhoZ = 20;
float pontoVisaoX = 0, pontoVisaoY = 0, pontoVisaoZ = 0;
bool  isFull=false;
float lastx=250, lasty=250;
int   leftButton = 0;
int   middleButton = 0;
float constVel = 0.1;
float *snapTime;

#endif
