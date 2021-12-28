#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>

#define DEF_CELL_SIZE 8
#define DEF_SCREEN_SIZE_X 1366
#define DEF_SCREEN_SIZE_Y 768
#define DEF_WORLD_SIZE_X DEF_SCREEN_SIZE_X/DEF_CELL_SIZE
#define DEF_WORLD_SIZE_Y DEF_SCREEN_SIZE_Y/DEF_CELL_SIZE
#define DEF_ANIME_INTERVAL 500
#define DEF_CELL_PROBABILITY 40 /* percents */

/* global vars */
char *cell;
unsigned char *cell_R;
unsigned char *cell_G;
unsigned char *cell_B;
char *new_cell;
int anime_interval = DEF_ANIME_INTERVAL;
int pause = 0;
int cell_size = DEF_CELL_SIZE;
int world_size_x = DEF_WORLD_SIZE_X;
int world_size_y = DEF_WORLD_SIZE_Y;
int screen_size_x = DEF_SCREEN_SIZE_X;
int screen_size_y = DEF_SCREEN_SIZE_Y;
int cell_probability = DEF_CELL_PROBABILITY;

void redraw(void) {
	int i,j;
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f,screen_size_y,0.0f);
	glRotatef(180.0f,1.0f,0.0f,0.0f);
	glBegin(GL_QUADS);
		for (i = 0; i != world_size_y; i++) {
			for (j = 0; j != world_size_x; j++) {
				if (cell[i*world_size_x+j]) glColor3f(1.0f,1.0f,1.0f);
				else glColor3f(0.0f,0.0f,0.0f);
				glVertex2i(j*cell_size,i*cell_size);
				glVertex2i((j+1)*cell_size,i*cell_size);
				glVertex2i((j+1)*cell_size,(i+1)*cell_size);
				glVertex2i(j*cell_size,(i+1)*cell_size);
			}
		}
	glEnd();
	glFlush();
	glutSwapBuffers();
}

void windowResize(int x, int y) {
	glViewport(0,0,x,y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,screen_size_x,0,screen_size_y,-1,1);
}

void animate(int arg) {
	int i,j,k,l;
	int cell_count;
	char *temp;
	if (!pause) {
		for (i = 0; i != world_size_y; i++) {
			for (j = 0; j != world_size_x; j++) {
				cell_count = 0;
				for (k = -1; k != 2; k++) {
					for (l = -1; l != 2; l++) {
						if (k == 0 && l == 0) continue;
						if (i+k < world_size_y && i+k >= 0 && j+l >= 0 && j+l < world_size_x) {
							if (cell[(i+k)*world_size_x+j+l]) cell_count++;
						}
					}
				}
				if (cell_count == 3 || (cell[i*world_size_x+j] && cell_count == 2)) new_cell[i*world_size_x+j] = 1;
				else new_cell[i*world_size_x+j] = 0;
			}
		}
		temp = cell;
		cell = new_cell;
		new_cell = temp;
		glutPostRedisplay();
	}
	glutTimerFunc(anime_interval,animate,0);
}

void cell_init(void) {
	int i,j;
	for (i = 0; i != world_size_y; i++) {
		for (j = 0; j != world_size_x; j++) {
			cell_R[i*world_size_x+j] = ~0;
			cell_G[i*world_size_x+j] = ~0;
			cell_B[i*world_size_x+j] = ~0;
			if (rand()%100 < cell_probability) cell[i*world_size_x+j] = 1;
			else cell[i*world_size_x+j] = 0;
		}
	}
}

void grow_older(int x, int y) {
	if (col_B[y*world_size_x+j]) col_B[y*world_size_x+j]--;
	else {
		col_B[y*world_size_x+j] = ~0;
		if (col_G[y*world_size_x+j]) col_G[y*world_size_x+j]--;
		else {
			
		}
	}
}

void kbd(unsigned char key, int x, int y) {
	int i;
	if (key == 27 || key == 'q') exit(0);
	else if (key == ' ') pause = ~pause;
	else if (key == 'n') {
		cell_init();
		printf("Recreating world...\n");
		fflush(stdout);
		glutPostRedisplay();
	}
	else if (key == '+' && anime_interval-5 > 0) anime_interval -= 50;
	else if (key == '-') anime_interval += 50;
	else if (key == 'e') {
		for (i = 0; i != world_size_y*world_size_x; i++) cell[i] = 0; 
		glutPostRedisplay();
	}
}

void mouse(int button, int state, int x, int y) {
	x /= cell_size;
	y /= cell_size;
	cell[y*world_size_x+x] = (button == GLUT_LEFT_BUTTON ? 1 : 0);
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	srand(time(NULL));
	cell = (char *) malloc(world_size_x*world_size_y);
	cell_R = (unsigned char *) malloc(world_size_x*world_size_y);
	cell_G = (unsigned char *) malloc(world_size_x*world_size_y);
	cell_B = (unsigned char *) malloc(world_size_x*world_size_y);
	new_cell = (char *) malloc(world_size_x*world_size_y);
	cell_init();
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Game of Life");
	glutReshapeWindow(screen_size_x,screen_size_y);
	glutPositionWindow(0,0);
	glutReshapeFunc(windowResize);
	glutDisplayFunc(redraw);
	glutKeyboardFunc(kbd);
	glutMouseFunc(mouse);
	glutTimerFunc(anime_interval,animate,0);
	glutMainLoop();
	return 0;
}
