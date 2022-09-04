#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.174533 //one degree


//------PLAYER------

float degToRoad(int a){ return a*M_PI/180;}
int FixAng(int a){ if(a>359);{a-=360;} if(a<0){ a+360;}return a;}

float px,py,pdx,pdy,pa;

void drawPlayer2D() //player themselves
{
	glColor3f(1,1,0);
	glPointSize(8);
	glLineWidth(4);
	glBegin(GL_POINTS);
	glVertex2i(px,py);
	glEnd();
	
	glBegin(GL_LINES);
	glLineWidth(3);
	glVertex2i(px,py);
	glVertex2i(px+pdx*5, py+pdy*5);
	glEnd();
}

//------MAP------

#define mapX 8 //map width
#define mapY 8 //map height
#define mapS 64 //map cube size

int map[]=
{
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,0,1,0,1,0,1,
	1,0,0,0,0,1,0,1,
	1,1,1,1,1,1,1,1,
};

void drawMap2D() //a 2d map is made as a guide for the 3d navigation
{
	int x,y,xo,yo;
	for(y=0;y<mapY;y++)
	{
		for(x=0;x<mapX;x++)
		{
			if(map[y*mapX+x]==1){glColor3f(1,1,1);} else{glColor3f(0,0,0);}
			xo=x*mapS; yo=y*mapS;
			glBegin(GL_QUADS);
			glVertex2i(xo +1, yo +1);
			glVertex2i(xo +1, yo+mapS-1);
			glVertex2i(xo+mapS-1, yo+mapS-1);
			glVertex2i(xo+mapS-1, yo +1);
			glEnd();
		}
	}
}


//------RAY CASTING------
float distance(ax,ay,bx,by,ang){ return cos(degToRad(ang))*(bx-ax)-sin(degToRad(ang))*(by-ay);}

void drawRays2D() //actual ray casting script
{
	 glColor3f(0,1,1); glBegin(GL_QUADS); glVertex2i(526,  0); glVertex2i(1006,  0); glVertex2i(1006,160); glVertex2i(526,160); glEnd();
	 glColor3f(0,0,1); glBegin(GL_QUADS); glVertex2i(526,160); glVertex2i(1006,160); glVertex2i(1006,320); glVertex2i(526,320); glEnd();	 	
	
	int r,mx,my,mp,dof,side; float vx,vy,rx,ry,ra,xo,yo,disV,disH;
	
	ra=FixAng(pa+30);//ray set back 30 degrees
	
	for(r=0;r<60;r++)
	{
		//Check horizontal lines
		dof=0;disH=1000000;
		
		Tan=1.0/Tan;
			if(sin(degToRad(ra))>0.001){ ry=(((int)py>>6)<<6) -0.0001; rx=(py-ry)*Tan+px; yo=-64; xo=-yo*Tan;}//looking up
			else if(sin(degToRad(ra))<-0.001){ ry=(((int)py>>6)<<6)+64;rx=(py-ry)*Tan+px; yo= 64; xo=yo*Tan;}//looking down
			else{ rx=px; ry=py; dof=8;}
			
			while(dof<8)
		{
			mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
			if(mp>0 && mp<mapX*mapY && map[mp]==1){ dof=8; disH=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);} //wall
			else{ rx+=xo; ry+=yo; dof+=1;}//no wall
		}
		
		
		//check vertical lines
		dof=0; side=0; disV=1000000;
		float Tan=tan(degToRad(ra));
			if(cos(degToRad(ra))>0.001){ rx=(((int)px>>6)<<6)+64;	ry=(px-rx)*Tan+py; xo= 64; yo=-xo*Tan;}//looking left
		else if(cos(degToRad(ra))<-0.001){ rx=(((int)px>>6)<<6)-0.0001; ry=(px-rx)*Tan+py; xo=-64; yo=-xo*Tan;}//looking right
		else { rx=px; ry=py; dof=8;}//looking straight up or down
		
		while(dof<8)
		
		{
			mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
			if(mp>0 && mp<mapX*mapY && map[mp]==1){ dof=8; disV=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit wall
			else{ rx+=xo; ry+=yo; dof+=1;}//no wall
		}
		
		glColor3f(0,0.8,0);
		if(disV<disH){ rx=vx; ry=vy; disH=disV; glColor3f(0,0.6,0);}//horizontal ht first
		glLineWidth(2); glBegin(GL_LINES); glVertex2i(px,py); glEnd();//draw 2d ray
		
		int ca=FixAng(pa-ra); disH=disH*cos(degToRad(ca)); //fix fisheye
		int lineH=(mapS*320)/(disH); if(lineH>320){ lineH=320;} //line height limit
		int lineOff = 160 - (lineH>>1); //line offset
		
		glLineWidth(8);glBegin(GL_LINES);glVertex2i(r*8+530,lineOff);glVertex2i(r*8+530,lineOff+lineH);glEnd();//draw vert rays
		
		ra=FixAng(ra-1);
	}
}



void init() //window size and color
{
 glClearColor(0.3,0.3,0.3,0);
 gluOrtho2D(0,1024,510,0);
 px=150; py=400; pa=90;
 pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa)); 
}

void buttons(unsigned char key,int x,int y) //controls
{
	if(key=='a'){ pa-=0.1; if(pa<0){ pa+=2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5;}
	if(key=='d'){ pa+=0.1; if(pa<0){ pa+=2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5;}
	if(key=='w'){ px+=pdx; py+=pdy;}
	if(key=='s'){ px-=pdx; py-=pdy;}
	glutPostRedisplay();
}

void display()
{   
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
 drawMap2D();
 drawPlayer2D();
 drawRays2D();
 glutSwapBuffers();  
}

int main(int argc, char* argv[])
{ 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 glutInitWindowSize(1024,510);
 glutCreateWindow("Erin Go Bragh!");
 init();
 glutDisplayFunc(display);
 glutKeyboardFunc(buttons);
 glutMainLoop();
}
