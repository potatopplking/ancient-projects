#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define MAX_ITERATIONS 200
#define ZOOM_CONST 300

typedef struct {
	double r,i;
} complex;

typedef struct {
	double x,y;
} pos;

typedef struct {
	int x,y;
} pos_d;

double step_x;
double step_y;
char *buffer;
int x_size = 800;
int y_size = 600;
pos zoom_rec_start;
pos zoom_rec_end;
pos left_corner;
pos_d temp_left;
pos_d temp_right;

int mandelbrot(complex num) {
	complex c = num;
	int i;
	double temp;

	for (i = 0; (fabs(num.r*num.r+num.i*num.i) < 4) && i != MAX_ITERATIONS; i++) {
		temp = num.r*num.r-num.i*num.i+c.r;
		num.i = 2*num.r*num.i+c.i;
		num.r = temp;
	}
	if (i != MAX_ITERATIONS) return i+i/5;   // pozor, tento vypocet je zavisly na MAX_ITERATIONS (i+i/5 = 1.2 ~= 1.22 = 255 / 200)
	return 255;
}

void recalc_buf(void) {
	int i,j;
	complex position;

	printf("prepocitavam... ");
	fflush(stdout);
	step_x = fabs(zoom_rec_end.x-zoom_rec_start.x)/x_size;
	step_y = -1.0*fabs(zoom_rec_end.y-zoom_rec_start.y)/y_size;
	left_corner.x = zoom_rec_start.x > zoom_rec_end.x ? zoom_rec_end.x : zoom_rec_start.x;
	left_corner.y = zoom_rec_start.y > zoom_rec_end.y ? zoom_rec_start.y : zoom_rec_end.y;
	position.i = left_corner.y;
	for (i = 0; i != y_size; i++) {
		position.r = left_corner.x;
		for (j = 0; j != x_size; j++) {
			buffer[i*x_size+j] = mandelbrot(position);
			position.r += step_x;
		}
		position.i += step_y;
	}
	printf("hotovo\n");
}

void mouse_scroll(int button, int dir, int x, int y)
{
	
}

void mouse_click(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			temp_left.x = x;
			temp_left.y = y;
			//printf("temp_left.x = %d\ttemp_left.y = %d\n",temp_left.x,temp_left.y);
		}
		else {
			zoom_rec_start.x = left_corner.x + step_x*temp_left.x;
			zoom_rec_start.y = left_corner.y + step_y*(y_size-temp_left.y);
			zoom_rec_end.x = left_corner.x + step_x*x;
			zoom_rec_end.y = left_corner.y + step_y*(y_size-y);
			temp_left.x = temp_left.y = temp_right.x = temp_right.y = 0;
			//printf("zoom_rec_start.x = %lf\tzoom_rec_start.y = %lf\nzoom_rec_end.x = %lf\tzoom_rec_end.y = %lf\n",zoom_rec_start.x,zoom_rec_start.y,zoom_rec_end.x,zoom_rec_end.y);
			recalc_buf();
			glutPostRedisplay();
		}
	}
}

void mouse_motion(int x, int y) {
	temp_right.x = x;
	temp_right.y = y;
	//printf("temp_right.x = %d\ttemp_right.y = %d\n",temp_right.x,temp_right.y);
	glutPostRedisplay();
}

void redraw()
{
	int i,j;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(1.0f);

	glBegin(GL_POINTS);
		for (i = 0; i != y_size; i++) {
			for (j = 0; j != x_size; j++) {
				glColor3ub(buffer[i*x_size+j],buffer[i*x_size+j],buffer[i*x_size+j]);
				glVertex2i(j,i);
			}
		}
	glEnd();

	glColor4ub(120,0,0,128);
	glBegin(GL_LINE_LOOP);
		glVertex2i(temp_left.x,y_size-temp_left.y);
		glVertex2i(temp_left.x,y_size-temp_right.y);
		glVertex2i(temp_right.x,y_size-temp_right.y);
		glVertex2i(temp_right.x,y_size-temp_left.y);
	glEnd();
	
	glFlush();
	glutSwapBuffers();
}

void reshape(int x, int y)
{
	buffer = (char *) realloc(buffer,x*y);
	y_size = y;
	x_size = x;
	recalc_buf();
	glViewport(0,0,x,y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,x,0,y,-1,1);
}

void kbd(unsigned char key, int x, int y)
{
	if (key == 'q' || key == 27) exit(0);
	if (key == 'r') {
		zoom_rec_start.x = -2.0;
		zoom_rec_start.y = 1.0;
		zoom_rec_end.x = 1.0;
		zoom_rec_end.y = -1.0;
		recalc_buf();
		glutPostRedisplay();
	}
}

int main(int argc, char **argv)
{
	buffer = (char *) malloc(x_size*y_size);
	zoom_rec_start.x = -2.0;
	zoom_rec_start.y = 1.0;
	zoom_rec_end.x = 1.0;
	zoom_rec_end.y = -1.0;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Mandelbrot zoom");
	glutReshapeWindow(x_size, y_size);
	glutPositionWindow(100,100);
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse_click);
	glutMotionFunc(mouse_motion);
	glutMouseWheelFunc(mouse_scroll);
	glutKeyboardFunc(kbd);
	glutMainLoop();
	return 0;
}