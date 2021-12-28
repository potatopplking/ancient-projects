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
double *vertex;
double *normals;
int *faces;
int *faces_count;

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
	int i,j,prev_vert;
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

	prev_vert = 0;
	for (i = 0; i < object_count; i++) {
		glBegin(GL_TRIANGLES);
		for (j = 0; j < faces_count[i]; j++) {
			glVertex3f(vertex[3*faces[3*j+prev_vert]],vertex[3*faces[3*j+prev_vert]+1],vertex[3*faces[3*j+prev_vert]+2]);
			glVertex3f(vertex[3*faces[3*j+prev_vert+1]],vertex[3*faces[3*j+prev_vert+1]+1],vertex[3*faces[3*j+prev_vert+1]+2]);
			glVertex3f(vertex[3*faces[3*j+prev_vert+2]],vertex[3*faces[3*j+prev_vert+2]+1],vertex[3*faces[3*j+prev_vert+2]+2]);
		}
		prev_vert += 3*faces_count[i];
		glEnd();
	}


	glFlush();
	/* Switching buffers */
	glutSwapBuffers();
}

/* this will crash if file won't end with newline. Don't care */
void skip_line(const char *str, int *index)
{
	while (str[(*index)++] != '\n');
}

int main(int argc, char **argv)
{
	int fd;
	int filesize;
	char *buffer;
	int vertex_count = 0;
	int normals_count = 0;
	int vertex_pos = 0;
	int normal_pos = 0;
	int faces_pos = 0;
	int i,j,obj_i;
	int rem;
	int temp;
	char *endptr;

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
	/* reading whole file to buffer (because fuck you, thats why) */
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
	/* get number of objects, vertices & faces */
	obj_i = 0;
	temp = 0;
	i = 0;
	while (i < filesize-3) {
		/* comment */
		if (buffer[i] == '#') {
			skip_line(buffer,&i);
		}
		/* object */
		else if (buffer[i] == 'o' && buffer[i+1] == ' ') {
			obj_i++;
			skip_line(buffer,&i);
		}
		/* vertex */
		else if (buffer[i] == 'v' && buffer[i+1] == ' ') {
			vertex_count++;
			skip_line(buffer,&i);
		}
		/* normals */
		else if (buffer[i] == 'v' && buffer[i+1] == 'n' && buffer[i+2] == ' ') {
			normals_count++;
			skip_line(buffer,&i);
		}
		/* faces */
		else if (buffer[i] == 'f' && buffer[i+1] == ' ') {
			temp++;
			skip_line(buffer,&i);
		}
		else {
			skip_line(buffer,&i);
		}
	}
	object_count = obj_i;
	printf("Number of objects: %d\n",object_count);
	printf("Total number of faces: %d\nTotal number of vertices: %d\n",temp,vertex_count);
	fflush(stdout);
	faces = (int*)malloc(sizeof(int)*3*temp);
	faces_count = (int*)calloc(object_count,sizeof(int));
	vertex = (double*)malloc(sizeof(double*)*3*vertex_count);
	normals = (double*)malloc(sizeof(double*)*3*normals_count);
	/* reading file & creating display lists */
	for (i = 0, obj_i = -1; i < filesize-3;) {
		if (buffer[i] == '#') {
			skip_line(buffer,&i);
		}
		else if (buffer[i] == 'o' && buffer[i+1] == ' ') {
			obj_i++;
			skip_line(buffer,&i);
		}
		else if (buffer[i] == 'v' && buffer[i+1] == ' ') {
			i += 2;
			sscanf(buffer+i,"%lf %lf %lf",vertex+vertex_pos,vertex+vertex_pos+1,vertex+vertex_pos+2);
			vertex_pos += 3;
			skip_line(buffer,&i);
		}
		else if (buffer[i] == 'n' && buffer[i+1] == 'v' && buffer[i+2] == ' ') {
			i += 2;
			sscanf(buffer+i,"%lf %lf %lf",normals+normal_pos,normals+normal_pos+1,normals+normal_pos+2);
			normal_pos += 3;
			skip_line(buffer,&i);
		}
		else if (buffer[i] == 'f' && buffer[i+1] == ' ') {
			i += 2;
			faces_count[obj_i]++;
			for (j = 0; j < 3; j++) {
				faces[faces_pos++] = strtol(buffer+i,NULL,10)-1;
				/* we don't care about texture and normal coords */
				while (isgraph(buffer[i])) i++;
				i++;
			}
		}
		else {
			skip_line(buffer,&i);
		}
	}
	printf("DONE\n");
	// temp = 0;
	// for (i = 0; i < object_count; i++) {
	// 	printf("Object %d: faces: %d\n",i,faces_count[i]);
	// 	for (j = 0; j < faces_count[i]; j++) {
	// 		printf("\t%d %d %d\n", faces[3*j+temp],faces[3*j+1+temp],faces[3*j+2+temp]);
	// 		printf(" : (%lf %lf %lf) ",vertex[3*faces[3*j+temp]],vertex[3*faces[3*j+temp]+1],vertex[3*faces[3*j+temp]+2]);
	// 		printf(" (%lf %lf %lf) ",vertex[3*faces[3*j+temp+1]],vertex[3*faces[3*j+temp+1]+1],vertex[3*faces[3*j+temp+1]+2]);
	// 		printf(" (%lf %lf %lf)\n",vertex[3*faces[3*j+temp+2]],vertex[3*faces[3*j+temp+2]+1],vertex[3*faces[3*j+temp+2]+2]);
	// 	}
	// 	temp += 3*faces_count[i];
	// }
	printf("Entering main loop...\n");
	glutMainLoop();
	exit(0);
}
