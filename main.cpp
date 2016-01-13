#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>


#include "imageloader.h"


#ifndef PI
#define PI 3.14159265358979
#endif





pthread_t t1;
pthread_t t2;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cammutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t delmutex = PTHREAD_MUTEX_INITIALIZER;
/// Variaveis

int imageWidth =0, imageLength= 0;
float xpos = 0, ypos = 0, zpos = 0;
float xvet = 10, yvet = -5, zvet = 0;
float yrot = 0, xrot = 90;
float olhoX = -80, olhoY = 30, olhoZ = -350;
float pontoVisaoX = 0, pontoVisaoY = 0, pontoVisaoZ = 0;
bool  isFull=false;
float lastx=250, lasty=250;
int   leftButton = 0;
int   middleButton = 0;
float constVel = 0.3;
float ang = 15;
float dir = 1;
float *snapTime;

FILE *entrada;

FILE *entrada2;


// car 1 pos

float carX = -50;
float carY = 0;
float carZ = -350;
float carW = 0;


float raioVisao = 40;

float vel = 0;
float rotVel = 0;


// car 2 pos

float car2X = -50;
float car2Y = 0;
float car2Z = -320;
float car2W = 0;

float rot2Vel = 0;

// pos da camera 

float camx = 30;
float camy = 20;
float camz = 0;

int camnum = 1;
int comecar = 0;


GLuint  texture[6];


using namespace std;

/////////////////////////////////////////////////
//*/ Funcoes
void enable();
void mouseFunction(int Button, int state, int x, int y);
void updateVisionVector();
void motionFunc(int x, int y);
void drawSolidSphere(double tam, int slices, int stacks, double x, double y, double z);
void init();
void camera (void);
void paintGL();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void lightConfig();
GLuint loadGLTextures(const char *filename);
//void * controlTime(void * argument);
void desenhaChao();
void desenhaEixoXYZ();
void desenhaRoda(float x, float y, float z, float raio, float rotacao);
void desenhaEixo(float x, float y, float z, float tam);
void desenhaEixoTraseiro(float x, float y, float z, float tam);
void desenhaEixoDianteiro(float x, float y, float z, float tam, float rotacao );
void desenhaFundo(float x, float y, float z, float larg, float comp );
void desenhaLateral(float x, float y, float z, float yvec, float r, float g, float b); 
void desenhaVolante(float x, float y, float z, float ang);
void desenhaCarroceria(float r, float g, float b); 

void desenhaCarro(float x, float y, float z, float rot, float volRot, float r, float g, float b );

void controlaDeslocamento();
//*//////////////////////////////////////////////////

void camera (void) {
	
	float ox = carX - sin(((carW+90)/180)*PI)*raioVisao;
	float oz = carZ - cos(((carW+90)/180)*PI)*raioVisao;
	
    gluLookAt(ox,25,oz,carX,carY+10,carZ,0,1,0);
}

void cameraMouse(void) {
	
	    pontoVisaoX = olhoX + xvet;
    pontoVisaoY = olhoY + yvet;
    pontoVisaoZ = olhoZ + zvet;
    gluLookAt(olhoX,olhoY,olhoZ,pontoVisaoX,pontoVisaoY,pontoVisaoZ,0,1,0);
}



void enable (void) {
    glEnable (GL_DEPTH_TEST); //enable the depth testing
	glEnable(GL_COLOR_MATERIAL);
    glEnable (GL_LIGHTING); //enable the lighting
    glEnable (GL_LIGHT0); //enable LIGHT0, our Diffuse Light
    glShadeModel (GL_SMOOTH); //set the shader to smooth shader
    
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void mouseFunction( int Button,int state, int x, int y) {

    // cout << "Button: " << Button << " state: "<< state << endl;

    if (Button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        leftButton = 1;
        lastx = x;
        lasty = y;
    }
    if (Button == GLUT_LEFT_BUTTON && state == GLUT_UP)   {
        leftButton = 0;
    }

    if (Button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
        middleButton = 1;
        lastx = x;
        lasty = y;
    }
    if (Button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)   {
        middleButton = 0;
    }

    if (Button == 3 /*girar cima*/  && state == GLUT_DOWN) {
        olhoX+=xvet*constVel*10;
        olhoY+=yvet*constVel*10;
        olhoZ+=zvet*constVel*10;
    }
    if (Button == 4 /*girar baixo*/ && state == GLUT_DOWN) {
        olhoX-=xvet*constVel*10;
        olhoY-=yvet*constVel*10;
        olhoZ-=zvet*constVel*10;
    }
}

void updateVisionVector(){
    if(yrot  >  90){yrot  =  90;}
    if(yrot  < -90){yrot  = -90;}
    if(xrot >= 360){xrot -= 360;}
    if(xrot  <   0){xrot += 360;}

    xvet = cos(-xrot * PI / 180.0 );
    zvet = sin(-xrot * PI / 180.0 );
    yvet = sin( yrot * PI / 180.0 );
}

void motionFunc(int x, int y) {

    if(leftButton) {
        int diffx = x-lastx;
        int diffy = y-lasty;
        lastx = x;
        lasty = y;
        yrot += (float) diffy*0.1;
        xrot += (float) diffx*0.1;

        updateVisionVector();

        //cout << "x:" << xvet << "\ty:" << yvet << "\tz:" << zvet << endl;
        //cout << "xrot: " << xrot << "  yrot:" << yrot << endl;
    }
    if(middleButton) {
        int diffx = x-lastx;
        int diffy = y-lasty;
        lastx = x;
        lasty = y;

        olhoY+=diffy*constVel*0.1;
        olhoX+=diffx*zvet*constVel*0.1;
        olhoZ-=diffx*xvet*constVel*0.1;
    }

}

void drawSolidSphere(double tam, int slices, int stacks, double x, double y, double z){
    glPushMatrix();{
        glTranslated(x,y,z);
        glutSolidSphere(tam,slices,stacks);
        //glTranslated(-x,-y,-z);
    }
    glPopMatrix();
}

void init (void) {
    enable();
    lightConfig();
   // xvet = cos(-xrot * PI / 180.0 );
  //  zvet = sin(-xrot * PI / 180.0 );
  //  yvet = sin( yrot * PI / 180.0 );
    
    //texture[0] = loadGLTextures("tex/texture.bmp");
	texture[1] = loadGLTextures("pista.bmp");
	
	entrada = fopen ("posCar.txt","r");
	if (entrada==NULL) {
		printf("erro ao abrir o arquivo de entrada\n");
		exit(0);
	}
	
	entrada2 = fopen ("posCar2.txt","r");
	if (entrada2==NULL) {
		printf("erro ao abrir o arquivo de entrada\n");
		exit(0);
	}

}

void desenhaChao(){
	
	
	 glColor3f(0.8,0.8,0.8);
    int size = 1;
    int wid = 400*size;
	int hei = 640*size;
		glNormal3f( 0,1,0);
 //   glEdgeFlag(GL_FALSE);

  glEnable(GL_TEXTURE_2D);
  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	  glTexCoord2f(1.0f, 0.0f);	
	  glVertex3f(-hei, 0, -wid);
	  glTexCoord2f(0.0f, 0.0f);
	  glVertex3f(-hei, 0,  wid);
	  glTexCoord2f(0.0f, 1.0f);
	  glVertex3f( hei, 0,  wid);
	  glTexCoord2f(1.0f, 1.0f);
	  glVertex3f( hei, 0, -wid);
	glEnd(); 
	
	glDisable(GL_TEXTURE_2D);
 // glEdgeFlag(GL_TRUE);
  
  //glEnable(GL_TEXTURE_3D);
}

void desenhaFundo(float x, float y, float z, float larg, float comp ){
	glColor3f(0.7,0,0.0);
	glPushMatrix();
		glTranslatef(x,y,z);

		glBegin(GL_POLYGON);
			glNormal3f( 0,1,0);
			glVertex3f( comp/2,  0, larg/2);
			
			glNormal3f( 0,1,0);
			glVertex3f( -comp/2,  0, larg/2);
			
			glNormal3f( 0,1,0);
			glVertex3f( -comp/2,  0, -larg/2);
			
			glNormal3f( 0,1,0);
			glVertex3f( comp/2,  0, -larg/2);
				
		glEnd();
    glPopMatrix();
}

void desenhaLateral(float x, float y, float z, float yvec, float r, float g, float b){
	
	glColor3f(r,g,b);
	glPushMatrix();
		glTranslatef(x,y,z);
		
		
		glBegin(GL_POLYGON);
			glNormal3f( 0,0,yvec);
			glVertex3f( 2,  4.2, 0);
			glVertex3f( 3,  2.5, 0);
			glVertex3f( 12,  2.5, 0);
			glVertex3f( 13,  4.2, 0);
		glEnd();
		glBegin(GL_POLYGON);
			glNormal3f( 0,0.05,yvec);
			glVertex3f( 16,  4.2, 0);
			glVertex3f( 18,  2.5, 0);
			glVertex3f( 20,  2.5, 0);
			glVertex3f( 20,  3.5, 0);
			glVertex3f( 19,  5, 0);
			glVertex3f( 17,  6.0, 0);
			glVertex3f( 13,  6.0, 0);
			glVertex3f( 2.5,  6.7, 0);
			glVertex3f( -2.5,  7.0, 0);
			glVertex3f( -2.5,  4.2, 0);
		glEnd();
		
		glBegin(GL_POLYGON);
			glNormal3f( 0,0,yvec);
			glVertex3f( -2,  2.5, 0);
			glVertex3f( -1.5,  4.2, 0);
			glVertex3f( -2.5,  4.2, 0);
			glVertex3f( -2.5,  2.5, 0);
		glEnd();
    glPopMatrix();
}

void desenhaCarroceria(float r, float g, float b){
	float tam = 7;
	
	desenhaLateral(0,0, -2.5, 1, r, g, b);
	desenhaLateral(0,0, -9.5, -1, r, g, b);
	
	glColor3f(r,g,b);
	glPushMatrix();
		
		glTranslatef(0,0,-2.5);
		glBegin(GL_QUAD_STRIP);
			glNormal3f( 1,0,0);
			glVertex3f( 20,  2.5, 0);
			glVertex3f( 20,  2.5, -tam);
			
			glNormal3f( 0.7,0.4,0);
			glVertex3f( 20,  3.5, 0);
			glVertex3f( 20,  3.5, -tam);
			
			glNormal3f( 0.6,0.6,0);			
			glVertex3f( 19,  5, 0);
			glVertex3f( 19,  5, -tam);
			
			glNormal3f( 0.3,0.75,0);
			glVertex3f( 17,  6.0, 0);
			glVertex3f( 17,  6.0, -tam);
			
			glNormal3f( 0.1,0.95,0);
			glVertex3f( 13,  6.0, 0);
			glVertex3f( 13,  6.0, -tam);
		glEnd();
		
		
		glColor3f(0.7,0,0);	
		glBegin(GL_QUAD_STRIP);

			glNormal3f( 0.1,0.7,0);
			glVertex3f( 15,  6.0, 0);
			glVertex3f( 15,  6.0, -tam);
			
			
			glNormal3f( -0.1,0.7,0);
			glVertex3f( 14,  2.5, 0);
			glVertex3f( 14,  2.5, -tam);
			
			glNormal3f( 0.1,0.7,0);
			glVertex3f( 3.5,  2.5, 0);
			glVertex3f( 3.5,  2.5, -tam);
			
			
			glNormal3f( 0.1,0.7,0);
			glVertex3f( 2.5,  6.7, 0);
			glVertex3f( 2.5,  6.7, -tam);
			
		
		glEnd();
		
	glColor4f(0.5,0.5,0.5, 0.5);
		glBegin(GL_QUAD_STRIP);

			glNormal3f( 0.7,0.55,0);
			glVertex3f( 13,  6.0, 0);
			glVertex3f( 13,  6.0, -tam);
		
			glNormal3f( 0.7,0.55,0);
			glVertex3f( 12,  9.0, 0-1);
			glVertex3f( 12,  9.0, -tam+1);
			
			
		
		glEnd();
	
	glColor3f(r,g,b);
		glBegin(GL_QUAD_STRIP);

			glNormal3f( -1,0,0);
			glVertex3f( -2.5,  2.5, 0);
			glVertex3f( -2.5,  2.5, -tam);
		
			glNormal3f( -0.5,0.55,0);
			glVertex3f( -2.5,  7, 0);
			glVertex3f( -2.5,  7, -tam);
			
			glNormal3f( 0.1,0.95,0);
			glVertex3f( 2.5,  6.7, 0);
			glVertex3f( 2.5,  6.7, -tam);
		
		glEnd();
			
	glPopMatrix();
	
		glColor3f(0.9,0.9,0.9);
		glPushMatrix();
			glTranslatef(19.3,4.2,-3.5);
			glutSolidSphere(0.6,30,30);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(19.3,4.2,-8.5);
			glutSolidSphere(0.6,30,30);
		glPopMatrix();
		
		
		glColor3f(0.9,0,0);
		glPushMatrix();
			glTranslatef(-2.3,6,-3.5);
			glutSolidSphere(0.6,30,30);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-2.3,6,-8.5);
			glutSolidSphere(0.6,30,30);
		glPopMatrix();
   
}

void desenhaEixoXYZ(){
	glPushMatrix();
		GLUquadricObj *obj = gluNewQuadric();
		glPushMatrix();	
			glTranslatef(0,10,0);
			glColor3f(1.0, 0.0, 0.0);
			glRotatef(90.0,1.0,0.0,0.0);
			gluCylinder(obj, 0.2, 0.2, 10, 10, 10);
		glPopMatrix(); 

		glPushMatrix();	
			glTranslatef(0,0,0);
			glColor3f(0.0, 0.0, 1.0);
			glRotatef(90.0,0.0,1.0,0.0);
			gluCylinder(obj, 0.2, 0.2, 10, 10, 10);
		glPopMatrix(); 

		glPushMatrix();	
			glTranslatef(0,0,0);
			glColor3f(0.0, 1.0, 0.0);
			glRotatef(90.0,0.0,0.0,1.0);
			gluCylinder(obj, 0.2, 0.2, 10, 10, 10);
		glPopMatrix(); 

		
		gluDeleteQuadric(obj);
	glPopMatrix(); 
	
	
}

void desenhaRoda(float x, float y, float z, float raio, float rotacao){
	float size = raio;
	float alfa = 2.5;
	
	glPushMatrix();
		glTranslatef(x,y,z-(size/(alfa*2.0)));
		glRotatef(rotacao,0.0,1.0,0.0);
		
		glColor3f(0.2f, 0.2f, 0.2f);
		GLUquadricObj *obj = gluNewQuadric();
		gluCylinder(obj, size, size, size/alfa, 16, 16);
		
		gluDisk(obj, 0.0f, size, 16, 1);
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, size/alfa);
			gluDisk(obj, 0.0f, size, 16, 1);
		glPopMatrix();
		gluDeleteQuadric(obj);
	glPopMatrix(); 
}

void desenhaEixoDianteiro(float x, float y, float z, float tam, float rotacao) {
	desenhaRoda(x, y, z+tam/2, tam/4, rotacao);
	desenhaEixo(x, y, z, tam);
	desenhaRoda(x, y, z-tam/2, tam/4, rotacao);
}

void desenhaEixoTraseiro(float x, float y, float z, float tam) {
	desenhaRoda(x, y, z+tam/2, tam/4, 0);
	desenhaEixo(x, y, z, tam);
	desenhaRoda(x, y, z-tam/2, tam/4, 0);
}

void desenhaVolante(float x, float y, float z, float ang){
	glColor3f(0.2,0.2,0.2);
	glPushMatrix();
		glTranslatef(x,y,z);
		glRotatef(90,0,1,0);
		glRotatef(30,1,0,0);
		
		glRotatef(ang,0,0,-1);
		glutSolidTorus(0.13,1,50,50);
		
		float tam = 2;
		float raio = 0.13;
		GLUquadricObj *obj = gluNewQuadric();
			
		glPushMatrix();
			glTranslatef(-(tam/2),0,0);
			glRotatef(90,0,1,0);
			gluCylinder(obj, raio, raio, tam, 16, 16);
		glPopMatrix(); 
		
		tam = 4;
				
		glPushMatrix();
			//glTranslatef(0,0,(tam));
			//glRotatef(90,0,1,0);
			gluCylinder(obj, raio, raio, tam, 16, 16);
			
		glPopMatrix(); 
		
		gluDeleteQuadric(obj);
	
	
	glPopMatrix();
	
	
	
}

void desenhaEixo(float x, float y, float z, float tam){
	float raio = tam/30;
	
	glPushMatrix();
		glTranslatef(x,y,z-(tam/2));
		
		
		glColor3f(0.5f, 0.5f, 0.5f);
		GLUquadricObj *obj = gluNewQuadric();
		gluCylinder(obj, raio, raio, tam, 16, 16);
		
		gluDisk(obj, 0.0f, raio, 16, 1);
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, tam);
			gluDisk(obj, 0.0f, raio, 16, 1);
		glPopMatrix();
		
		
		

		gluDeleteQuadric(obj);
	glPopMatrix(); 
}

void desenhaCarro(float x, float y, float z, float rot, float volRot, float r, float g, float b ){
	glPushMatrix();
		glTranslatef(x, y, z );
		
		glRotatef(rot, 0, 1, 0);
		glTranslatef(-7.5, 0, 6 );
		
		desenhaFundo(7.5+1.25, 2.5,-6, 7, 22.5);
		desenhaEixoTraseiro(0, 2.5, -6, 10);
		desenhaEixoDianteiro(15, 2.5, -6, 10, volRot);
		desenhaVolante(11,6,-8, volRot);
		desenhaCarroceria(r, g, b);
	glPopMatrix();
}


void controlaDeslocamento() {
	
	// atualiza a rotacao
	
	if (rotVel>0 ) rotVel-=0.2;
	if (rotVel<0 ) rotVel+=0.2;
	if (vel> 0.7 ) vel-=0.02;
	if (vel< -0.7 ) vel+=0.02;
	
	carW+=rotVel/10.0;
	
	
	// atualiza a posicao do carro
	
	
	carX = carX + sin(((carW+90)/180)*PI)*vel;
	carZ = carZ + cos(((carW+90)/180)*PI)*vel;
	
	
}

void paintGL (void) {
	
    glClearColor (0.0,0.0,0.0,1.0); //clear the screen to black
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the color buffer and the depth buffer
    glLoadIdentity();
    
    
    if (comecar){
		if (!feof(entrada)){
			fscanf(entrada, "%f %f %f %f", &carX, &carZ, &carW, &rotVel);
		} else {
			
		}
		
		if (!feof(entrada2)){
			fscanf(entrada2, "%f %f %f %f", &car2X, &car2Z, &car2W, &rot2Vel);
		} else {
			
		}
	} else {
		controlaDeslocamento();
	}
    
    //printf ("%f\t%f\t%f\t%f\n", carX, carZ, carW, rotVel);
	camera();
	
	
	glColor3f(1,1,1);
	
	desenhaChao();
	desenhaEixoXYZ();
	
	desenhaCarro(carX,carY,carZ,carW,rotVel*3,0,0,0.7);
	
	
	desenhaCarro(car2X,car2Y,car2Z,car2W,rot2Vel*3,0.7,0.7,0.0);

	

	
	
	
	
	glFlush();
    glutSwapBuffers(); //swap the buffers
    
}


void reshape (int w, int h) {
    glViewport (0, 0, (GLsizei)w, (GLsizei)h); //set the viewport to the current window specifications
    glMatrixMode (GL_PROJECTION); //set the matrix to projection
    glLoadIdentity ();
    gluPerspective (60, (GLfloat)w / (GLfloat)h, 0.5, 2000.0);
    glMatrixMode (GL_MODELVIEW); //set the matrix back to model

}

void keyboard (unsigned char key, int x, int y) {
    if (key=='w') {
        vel+=0.2;
    }
    if (key=='s') {
        vel-=0.2;
    }
    if (key=='d') {
        rotVel-=2.0;
        if (rotVel < -12) rotVel = -12;
    }
    if (key=='a') {
        rotVel+=2.0;
        
        if (rotVel > 12) rotVel = 12;
    }
    
    if (key=='j') {
        carW+=5;
    }
    
    if (key=='l') {
       carW-=5;
     }
    
    if (key=='i') {
        comecar=1;
    }
    
    if (key==27)  {
        exit(0);
    }
}

void specialKeyboard(int key, int x, int y){
    if (key == GLUT_KEY_F5) {
        if (!isFull){
            glutFullScreen();
        } else {
            glutReshapeWindow(800, 600);
            glutPositionWindow(100,0);
        }
        isFull=!isFull;
    }//*
    if (key == GLUT_KEY_UP) {
        
    }
    if (key == GLUT_KEY_DOWN) {
       
    }//*/

    if (key == GLUT_KEY_RIGHT) {
        
    }
    if (key == GLUT_KEY_LEFT) {
        
    }


}

void * moveToUp(void * argument) {
	
	
	float desx = 0;
   float desy = 100;
   float desz = 100;
   
   	pthread_mutex_lock( &cammutex ); 
   float antx = camx;
   float anty = camy;
   float antz = camz;
   	pthread_mutex_unlock( &cammutex ); 

   float frames = 300;
   
   for(int i = 0; i<frames;i++){
	   
		pthread_mutex_lock( &delmutex );  
			camx=antx+(desx-antx)*(i/frames) ;
			camy=anty+(desy-anty)*(i/frames)  ;
			camz=antz+(desz-antz)*(i/frames)  ;
		pthread_mutex_unlock( &delmutex );

			
		usleep(20*1000); // fall alseep here for 2 seconds...
   }
	return NULL;
}


int main (int argc, char **argv) {
  //  srand (time(NULL));
   // pthread_create( &t1, NULL, controlTime,NULL);
  
    
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE  | GLUT_DEPTH);
   // glutInitDisplayMode(GLUT_SINGLE| GLUT_RGBA | GLUT_DEPTH);
	
    glutInitWindowSize (800, 600);
    glutInitWindowPosition (100, 0);
    glutCreateWindow ("Trabalho CG");
    init ();
    glutDisplayFunc (paintGL);
    glutIdleFunc (paintGL);
    glutReshapeFunc (reshape);
    glutMouseFunc(mouseFunction);
    glutMotionFunc(motionFunc);


    glutKeyboardFunc (keyboard);
    glutSpecialFunc	(specialKeyboard);
	
    glutMainLoop ();
    return 0;
}

void lightConfig() {
    // Definindo posiçoes e intensidade das luzes
    
	
	GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0}; 
	GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};	   // "cor" 
	GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};// "brilho" 
	GLfloat posicaoLuz[4]={0.0, 20.0, 20.0, 1.0};

	// Capacidade de brilho do material
	GLfloat especularidade[4]={1.0,1.0,1.0,1.0}; 
	GLint especMaterial = 60;


    // Especifica que a cor de fundo da janela será preta
    glClearColor(0.9f, 0.1f, 0.1f, 1.0f);

    // Habilita o modelo de colorização de Gouraud

    //glShadeModel(GL_FLAT);
    glShadeModel(GL_SMOOTH);

    // Define a refletância do material
    glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
    // Define a concentração do brilho
    glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

    // Ativa o uso da luz ambiente
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

    // Define os parâmetros da luz de número 0
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );

    // Habilita a definição da cor do material a partir da cor corrente
    glEnable(GL_COLOR_MATERIAL);
    //Habilita o uso de iluminação
    glEnable(GL_LIGHTING);
    // Habilita a luz de número 0
    glEnable(GL_LIGHT0);
    // Habilita o depth-buffering
    glEnable(GL_DEPTH_TEST);
}

 
GLuint loadGLTextures(const char *filename){
	GLuint result;
    Image* image = loadBMP(filename);
    // create the texture
    glGenTextures(1, &result);   
	glBindTexture(GL_TEXTURE_2D, result);
	// actually generate the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image->width, image->height, 0,
	GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	//cout << "w: " << image->width << " h: " << image->height << endl;
	//enable linear filtering
    return result;
}
