#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.141592654
#define MOVE_INTERVAL 30
#define TERRAIN_DL_ID 32

// Global variables
int fp = 0;
unsigned char* buffer;
int mapResX = 0;
int mapResY = 0;
int maxVal = 0;
int screenWidth = 640;
int screenHeight = 480;
int shadeMode = GL_SMOOTH;
int drawMode = GL_FILL;
float* heightMap;
float maxHeight = 50.0;
float heightStep;
// Global vars for movement
int moveForward = 0;
int moveBackward = 0;
int moveLeft = 0;
int moveRight = 0;
int moveUp = 0;
int moveDown = 0;
// Global vars for mouse
int xAngleNew = 0;
int yAngleNew = 0;
int xAngleOld = 0;
int yAngleOld = 0;
int xAngleTemp;
int yAngleTemp;
// Global vars for camera
float camPos[] = {0.0f, 0.0f, 0.0f, 1.0f};
double camSpeed = 2.0;
double fov = 70.0;
double camNear = 20.0;
double camFar = 1000.0;
// Global vars for lights
float light0pos[] = {0.0f,10.0f, -10.0f, 1.0f};
float lightCol[] = {2.0f,2.0f,2.0f};
// Global vars for material
float terrainColor[] = {0.9f, 0.5f, 0.3f};

//Functions

void drawCube(double posX, double posY, double posZ, double x, double y, double z)
{
	posX -= x/2;
	posY -= y/2;
	posZ -= z/2;
	glFrontFace(GL_CW);
	glBegin(GL_QUADS);           // I'm not using quad strip because of front face problems
	glNormal3f(.0f,.0f,-1.0f);
	glVertex3f(posX,posY,posZ);
	glVertex3f(posX,posY+y,posZ);
	glVertex3f(posX+x,posY+y,posZ);
	glVertex3f(posX+x,posY,posZ);
	glNormal3f(-1.0f,.0f,.0f);
	glVertex3f(posX,posY,posZ);
	glVertex3f(posX,posY,posZ+z);
	glVertex3f(posX,posY+y,posZ+z);
	glVertex3f(posX,posY+y,posZ);
	glNormal3f(.0f,.0f,1.0f);
	glVertex3f(posX,posY,posZ+z);
	glVertex3f(posX+x,posY,posZ+z);
	glVertex3f(posX+x,posY+y,posZ+z);
	glVertex3f(posX,posY+y,posZ+z);
	glNormal3f(1.0f,.0f,.0f);
	glVertex3f(posX+x,posY,posZ+z);
	glVertex3f(posX+x,posY,posZ);
	glVertex3f(posX+x,posY+y,posZ);
	glVertex3f(posX+x,posY+y,posZ+z);
	glNormal3f(.0f,1.0f,.0f);
	glVertex3f(posX,posY+y,posZ);
	glVertex3f(posX,posY+y,posZ+z);
	glVertex3f(posX+x,posY+y,posZ+z);
	glVertex3f(posX+x,posY+y,posZ);
	glNormal3f(.0f,-1.0f,.0f);
	glVertex3f(posX,posY,posZ);
	glVertex3f(posX+x,posY,posZ);
	glVertex3f(posX+x,posY,posZ+z);
	glVertex3f(posX,posY,posZ+z);
	glEnd();
}

void createTerrain(float* map, int resX, int resZ)
{
	glFrontFace(GL_CCW);
	glNewList(TERRAIN_DL_ID,GL_COMPILE);
		int i,j;
		for (i = 0; i < resZ-1; i++) {
			glBegin(GL_TRIANGLE_STRIP);
			for (j = 0; j < resX; j++) {
				glVertex3f(j,map[i*resX+j],i);
				glVertex3f(j,map[(i+1)*resX+j],i+1);
			}
			glVertex3f(j,map[(i+1)*resX-1],i);
			glVertex3f(j,map[(i+2)*resX-1],i+1);	
			glEnd();
		}
		glBegin(GL_TRIANGLE_STRIP);
		for (i = 0; i < resX; i++) {
				glVertex3f(i,map[(resZ-1)*resX+i],resZ-1);
				glVertex3f(i,map[(resZ-1)*resX+i],resZ);
		}
		glVertex3f(i,map[resZ*resX-1],resZ-1);
		glVertex3f(i,map[resZ*resX-1],resZ);
		glEnd();
	glEndList();
}

void setRenderingParams(void)
{
	// Setting z-buffer (depth) test, enabling "painter's algorithm"
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Setting shading model
	glShadeModel(shadeMode);

	// Setting polygon mode
	glPolygonMode(GL_FRONT_AND_BACK,drawMode);

	// Enabling face culling
	// temporarily disabled
	glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Enabling lights
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}

void nextWhitespace(void)
{
	while (buffer[fp] != ' ' && buffer[fp] != '\n') fp++;
}

void nextLine(void)
{
	while (buffer[fp++] != '\n');
}

int str2int(int pointer)
{
	int power = 1;
	int result = 0;

	while (buffer[pointer] != ' ' && buffer[pointer] != '\n') {
		result += (buffer[pointer]-0x30)*power;
		power *= 10;
		pointer--;
	}
	return result;
}

void moveCam(int arg)
{
	if (moveUp) camPos[1] += camSpeed;
	if (moveDown) camPos[1] -= camSpeed;
	if (moveForward) {
		camPos[0] += camSpeed*sin(xAngleNew*PI/180.0);
		camPos[2] -= camSpeed*cos(xAngleNew*PI/180.0);
	}
	if (moveBackward) {
		camPos[0] -= camSpeed*sin(xAngleNew*PI/180.0);
		camPos[2] += camSpeed*cos(xAngleNew*PI/180.0);
	}
	if (moveLeft) {
		camPos[2] -= camSpeed*sin(xAngleNew*PI/180.0);
		camPos[0] -= camSpeed*cos(xAngleNew*PI/180.0);
	}
	if (moveRight) {
		camPos[2] += camSpeed*sin(xAngleNew*PI/180.0);
		camPos[0] += camSpeed*cos(xAngleNew*PI/180.0);
	}
	glutPostRedisplay();
	glutPostRedisplay();
	glutTimerFunc(MOVE_INTERVAL,moveCam,0);
}

void kbd(unsigned char key, int x, int y)
{
	int i;
	switch (key) {
		case 'q':
		case 27:
			exit(0);
		case 'w':
			moveForward = 1;
			break;
		case 's':
			moveBackward = 1;
			break;
		case 'a':
			moveLeft = 1;
			break;
		case 'd':
			moveRight = 1;
			break;
		case ' ':
			moveUp = 1;
			break;
		case 'c':
			moveDown = 1;
			break;
		case 'p':	// Change polygon shading mode
			if (shadeMode == GL_FLAT) shadeMode = GL_SMOOTH;
			else shadeMode = GL_FLAT;
			setRenderingParams();
			break;
		case 'o':	// Change polygon draw mode
			if (drawMode == GL_FILL) drawMode = GL_POINT;
			else if (drawMode == GL_POINT) drawMode = GL_LINE;
			else drawMode = GL_FILL;
			setRenderingParams();
			break;
		case 'e':
			printf("xAngleNew = %d\tcamPos = (%lf , %lf , %lf)\n",xAngleNew,camPos[0],camPos[1],camPos[2]);
			fflush(stdout);
			break;
		case '9':
			xAngleNew--;
			break;
		case '0':
			xAngleNew++;
			break;
		case 'i':
			printf("Inverting terrain... ");
			fflush(stdout);
			for (i = 0; i != mapResX*mapResY; i++) heightMap[i] = maxVal-heightMap[i];
			printf("done.\nRecreating display list and rendering... ");
			createTerrain(heightMap,mapResX,mapResY);
			printf("done.\n");
			fflush(stdout);
			break;
		case 'l':
			printf("Setting light 1 position to current cam position: (%lf, %lf, %lf)\n",camPos[0],camPos[1],camPos[2]);
			fflush(stdout);
			glLightfv(GL_LIGHT1,GL_POSITION,camPos);
			break;
	}
	glutPostRedisplay();
}

void kbdUp(unsigned char key, int x, int y)
{
	switch (key) {
		case 'w':
			moveForward = 0;
			break;
		case 's':
			moveBackward = 0;
			break;
		case 'a':
			moveLeft = 0;
			break;
		case 'd':
			moveRight = 0;
		break;
		case ' ':
			moveUp = 0;
			break;
		case 'c':
			moveDown = 0;
			break;
	}
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			xAngleTemp = x;
			yAngleTemp = y;
		}
		else {
			xAngleOld = xAngleNew;
			yAngleOld = yAngleNew;
		}
	}
	glutPostRedisplay();
}

void mouseMotion(int x, int y)
{
	xAngleNew = xAngleOld+x-xAngleTemp;
	yAngleNew = yAngleOld+y-yAngleTemp;
	glutPostRedisplay();
}

void windowResize(int w, int h)
{
	double left, right, bottom, top, aspect;
	// Setting viewport
	glViewport(0,0,w,h);

	aspect = w/h;
	right = camNear*tan(fov*PI/360.0);
	left = -1.0*right;
	top = right/aspect;
	bottom = -1.0*top;
	// Setting projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(left,right,bottom,top,camNear,camFar);
	//glOrtho(0,w,0,h,-1000,1000);

	screenHeight = h;
	screenWidth = w;
}

void windowRedraw()
{     
	// Erasing all buffers
	glClearColor(0.2f,0.4f,0.6f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setting lights
	glLightfv(GL_LIGHT0,GL_POSITION,light0pos);
	glLightfv(GL_LIGHT1,GL_POSITION,camPos);
	glLightfv(GL_LIGHT0,GL_AMBIENT_AND_DIFFUSE,lightCol);
	glLightfv(GL_LIGHT1,GL_AMBIENT_AND_DIFFUSE,lightCol);

	// Setting modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(yAngleNew,1.0f,0.0f,0.0f);
	glRotatef(xAngleNew,0.0f,1.0f,0.0f);
	glTranslatef(-1.0f*camPos[0],-1.0f*camPos[1],-1.0f*camPos[2]);

	// Setting material and drawing terrain (calling display list)
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,terrainColor);
	drawCube(200,0,0,1,1,1);
	glCallList(TERRAIN_DL_ID);

	// Flushing and swapping front and back color buffer
	glFlush();
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	FILE* srcFile;
	int fileSize;
	int i;

	if (argc != 2) {
		printf("This programme requires one parameter.\n");
		exit(1);
	}

	if (!(srcFile = fopen(argv[1],"rb"))) {
		printf("Could not open file %s.\n",argv[1]);
		exit(1);
	}

	fseek(srcFile,0,SEEK_END);
	fileSize = ftell(srcFile);
	rewind(srcFile);
	buffer = (char*) malloc(fileSize);

	fread(buffer,1,fileSize,srcFile);
	fclose(srcFile);
	if (buffer[fp] != 'P' || buffer[fp+1] != '5') {
		printf("This file is not binary PGM.\n");
		exit(1);
	}
	fp += 3;
	// Header processing
	while (buffer[fp] == '#') nextLine();                   // skipping commentars	
	nextWhitespace();
	fp--;
	mapResX = str2int(fp);
	fp += 2;
	nextWhitespace();
	fp--;
	mapResY = str2int(fp);
	fp += 2;
	while (buffer[fp] == '#') nextLine();                   // skipping commentars
	nextWhitespace();
	fp--;
	maxVal = str2int(fp);
	fp += 2;
	printf("PGM header processing succeeded:\n\tHorizontal (X) resolution =  %d\n\t",mapResX);
	printf("Vertical (Y) resolution = %d\n\t",mapResY);
	printf("Max grayscale value (equal to white) = %d\n",maxVal);

	heightStep = maxHeight / maxVal;                        // Calculating height step
	heightMap = (float*) malloc(mapResX*mapResY*sizeof(float));
	printf("Converting pixel values to array of floating-point variables... ");
	fflush(NULL);          // flushing stdout so that header info is visible during converting
	for (i = 0; i != mapResX*mapResY; i++) {
		heightMap[i] = heightStep*buffer[fp+i];
	}
	printf("done.\nInitiating openGL rendering...\n");
	fflush(stdin);

	// Glut init
	argc = 1;
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(screenWidth,screenHeight);
	glutCreateWindow("Height Map Viewer");
	glutKeyboardFunc(kbd);
	glutKeyboardUpFunc(kbdUp);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowRedraw);
	setRenderingParams();
	createTerrain(heightMap,mapResX,mapResY);
	glutTimerFunc(MOVE_INTERVAL,moveCam,0);
	glutMainLoop();
	printf("Window closed. Bye.\n");
	return 0;
}
