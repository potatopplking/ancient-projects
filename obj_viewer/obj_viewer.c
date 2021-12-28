#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>

#define CAM_FOV 70.0 /* camera field-of-view angle */

double camLeft, camRight, camBottom, camTop, camNear, camFar;
GLfloat x_rot = 0.0;
GLfloat y_rot = 0.0;
GLfloat cam_dist = -1000.0f;
GLfloat win_aspect;
int window_width;
int window_height;
int object_count = 0;
float light_0_pos[4] = {0.0f,0.0f,2.0f,0.0f};

void drawCube(double posX, double posY, double posZ, double x, double y, double z)
{
	posX -= x/2;
	posY -= y/2;
	posZ -= z;
	glBegin(GL_QUAD_STRIP);
		glNormal3f(.0f,.0f,-1.0f);
		glVertex3f(posX+x,posY,posZ);
		glVertex3f(posX+x,posY+y,posZ);
		glVertex3f(posX,posY,posZ);
		glVertex3f(posX,posY+y,posZ);
		glNormal3f(-1.0f,.0f,.0f);
		glVertex3f(posX,posY,posZ+z);
		glVertex3f(posX,posY+y,posZ+z);
		glNormal3f(.0f,.0f,1.0f);
		glVertex3f(posX+x,posY,posZ+z);
		glVertex3f(posX+x,posY+y,posZ+z);
		glNormal3f(1.0f,.0f,.0f);
		glVertex3f(posX+x,posY,posZ);
		glVertex3f(posX+x,posY+y,posZ);
	glEnd();
	glBegin(GL_QUADS);
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

void kbd_action(unsigned char key, int x, int y) {
	switch (key) {
		case 'q':
		case 27:  /* ESC key */
			exit(0);
		case '8':
			y_rot += 10;
			glutPostRedisplay();
			break;
		case '5':
			y_rot -= 10;
			glutPostRedisplay();
			break;
		case '4':
			x_rot -= 10;
			glutPostRedisplay();
			break;
		case '6':
			x_rot += 10;
			glutPostRedisplay();
			break;
		case '+':
			cam_dist += 100.0f;
			glutPostRedisplay();
			break;
		case '-':
			cam_dist -= 100.0f;
			glutPostRedisplay();
			break;
	}
}

void resize_action(int x, int y) {
	camNear = 20.0;
	camFar = 10000.0;

	window_width = x;
	window_height = y;
	win_aspect = ((double)x)/y;
	/* Setting viewport - visible part of window */
	glViewport(0,0,x,y);

	/* Setting values for projection matrix - camera */
	camRight = camNear*tan(CAM_FOV*M_PI/360.0);
	camLeft = -1.0*camRight;
	camTop = camRight/win_aspect;
	camBottom = -1.0*camTop;
}

void setRenderParams(void) {
	/* Setting z-buffer test ("Painter's algorithm") */
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/* Set clear color for framebuffer */
	glClearColor(0.1f,0.4f,0.7f,0.0f);

	/* Setting shade and polygon model */
	glShadeModel(GL_FLAT);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	/* Disabling face culling (TODO: fix face normals and enable culling) */
	glDisable(GL_CULL_FACE);

	/* Enabling light 0 (lighting globally enabled directly in redraw() func) */
	glEnable(GL_LIGHT0);

	/* Enable lighting */
	glEnable(GL_LIGHTING);

	/* Enabling color tracking - with it we can set material with simple glColor* */
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

	/* Setting mouse cursor */
	glutSetCursor(GLUT_CURSOR_NONE);

	/* Define how are pixels stored in memory */
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
}

void redraw()
{
	int i;
	/* Setting perspective camera (for 3D scene) */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(camLeft,camRight,camBottom,camTop,camNear,camFar);

	/* Clearing frame buffer(s) and z-buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Using modelview matrix */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	/* Set static lights */
	glLightfv(GL_LIGHT0,GL_POSITION,light_0_pos);
 
	/* Rotating and translating scene - for TESTING purposes only */
	glTranslatef(0.0f,0.0f,cam_dist);
	glRotatef(y_rot,1.0f,0.0f,0.0f);
	glRotatef(x_rot,0.0f,0.0f,1.0f);

	//glScalef(100.0f,100.0f,100.0f);
	for (i = 1; i != object_count+1; i++) {
		glCallList(i);
	}
	glFlush();
	/* Switching buffers */
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	int fd;
	int filesize;
	char *buffer;
	double *vertex;
	double *normals;
	int vertex_count = 0;
	int normals_count = 0;
	int faces_count;
	int vertex_pos = 0;
	int normal_pos = 0;
	int object_dlist = 1;
	int i,j;
	int rem;
	int temp;

	if (argc != 2) {
		fprintf(stderr,"error: wrong arguments number\n");
		exit(1);
	}

	fd = open(argv[1],O_RDONLY);
	if (fd < 0) {
		fprintf(stderr,"error: couldn't open file %s\n",argv[1]);
		exit(1);
	}
	/* getting filesize */
	filesize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	/* allocating memory for buffer */
	buffer = (char *) malloc(filesize);
	/* reading whole file to buffer */
	if (filesize != read(fd, buffer, filesize)) {
		fprintf(stderr,"error: couldn't read file %s\n",argv[1]);
		exit(1);
	}
	close(fd);
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("OBJ Viewer");
	glutReshapeWindow(800,600);
	glutPositionWindow(100,100);
	glutReshapeFunc(resize_action);
	glutDisplayFunc(redraw);
	glutKeyboardFunc(kbd_action);
	setRenderParams();
	/* reading file & creating display lists */
	for (i = 0; i < filesize-3; i++) {
		if (buffer[i] == '\n' && buffer[i+1] == 'o' && buffer[i+2] == ' ') {
			vertex_count = 0;
			normals_count = 0;
			faces_count = 0;
			vertex_pos = normal_pos = 0;
			i += 3;
			printf("Object: ");
			object_count++;
			while (buffer[i] != '\n') putchar((int) buffer[i++]);
			putchar((int) '\n');
			rem = i;
			while (1) {
				if (i+3 < filesize && buffer[i] == '\n') {
					if (buffer[i+1] == 'v' && buffer[i+2] == ' ') {
						vertex_count++;
					}
					else if (buffer[i+1] == 'v' && buffer[i+2] == 'n' && buffer[i+3] == ' ') {
						normals_count++;
					}
					else if (buffer[i+1] == 'f' && buffer[i+2] == ' ') break;
				}
				i++;
			}
			vertex = (double *) malloc(sizeof(double)*3*vertex_count);
			normals = (double *) malloc(sizeof(double)*3*normals_count);
			i = rem;
			vertex_pos = 0;
			while (i < filesize-3) {
				if (buffer[i] == '\n') {
					if (buffer[i+1] == 'v' && buffer[i+2] == ' ') {
						i += 3;
						sscanf(buffer+i,"%lf %lf %lf",vertex+vertex_pos,vertex+vertex_pos+1,vertex+vertex_pos+2);
						vertex_pos += 3;
					}
					else if (buffer[i+1] == 'v' && buffer[i+2] == 'n' && buffer[i+3] == ' ') {
						i += 4;
						sscanf(buffer+i,"%lf %lf %lf",normals+normal_pos,normals+normal_pos+1,normals+normal_pos+2);
						normal_pos += 3;
					}
					else if (buffer[i+1] == 'f' && buffer[i+2] == ' ') break;
				}
				i++;
			}
			normal_pos = vertex_pos = 0;
			glNewList(object_dlist++,GL_COMPILE);
			printf("at pos %d:\n",i);
			printf("dlist number: %d\n",object_dlist);
			//printf("glBegin(GL_TRIANGLES);\n");
			glBegin(GL_TRIANGLES);
			while (i < filesize) {
				if (buffer[i] == '\n' && buffer[i+1] == 'f' && buffer[i+2] == ' ') {
					faces_count++;
					i += 3;
					for (j = 0; j != 3; j++,i++) {
						sscanf(buffer+i,"%d",&vertex_pos);
						vertex_pos--;
						while (isdigit(buffer[i])) i++;
						if (buffer[i] == '/') {
							i++;
							while (isdigit(buffer[i]) || buffer[i] == '.') i++; /* we don't care about texture coordinates */
							if (buffer[i] != '/') {
								fprintf(stderr, "error: found texture coordinates but no normal vector\n");
								exit(1);
							}
							i++;
							sscanf(buffer+i,"%d",&normal_pos);
							normal_pos--;
							//printf("glNormal3f(%lf,%lf,%lf);\n",normals[3*normal_pos],normals[3*normal_pos+1],normals[3*normal_pos+2]);
							glNormal3f(normals[3*normal_pos],normals[3*normal_pos+1],normals[3*normal_pos+2]);
							while (isdigit(buffer[i])) i++;
						}
						//printf("glVertex3f(%lf,%lf,%lf);\n",vertex[3*vertex_pos],vertex[3*vertex_pos+1],vertex[3*vertex_pos+2]);
						glVertex3f(vertex[3*vertex_pos],vertex[3*vertex_pos+1],vertex[3*vertex_pos+2]);
					}
					i--;
				}
				else if (buffer[i] == '\n' && buffer[i+1] == 'o' && buffer[i+2] == ' ') {
					i--;
					break;
				}
				else i++;
			}
			glEnd();
			printf("glEnd();\n");
			glEndList();
			printf("vertex count: %d\nnormals count: %d\nfaces count: %d\n",vertex_count,normals_count,faces_count);
			free(vertex);
			free(normals);
		}
	}
	glutMainLoop();
}
