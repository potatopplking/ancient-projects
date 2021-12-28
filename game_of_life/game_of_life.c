#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

#define FALSE 0
#define TRUE 1
#define DEF_CELL_SIZE 4
#define DEF_WORLD_SIZE_X 100
#define DEF_WORLD_SIZE_Y 100
#define DEF_SCREEN_SIZE_X DEF_WORLD_SIZE_X*DEF_CELL_SIZE
#define DEF_SCREEN_SIZE_Y DEF_WORLD_SIZE_Y*DEF_CELL_SIZE
#define DEF_ANIME_INTERVAL 500
#define DEF_CELL_PROBABILITY 40.0 /* percents */
#define DEF_DEVIATION_PROBABILITY 1.0 /* percents */
/* cell states */
#define ALIVE 1
#define DEAD 0
#define DEVIATION_ALIVE 2 /* should be dead, is alive */
#define DEVIATION_DEAD -1 /* should be alive, is dead */

/* global vars */
char *cell;
char *new_cell;
int anime_interval = DEF_ANIME_INTERVAL;
int anim_pause = 0;
int cell_size = DEF_CELL_SIZE;
int world_size_x = DEF_WORLD_SIZE_X;
int world_size_y = DEF_WORLD_SIZE_Y;
int screen_size_x = DEF_SCREEN_SIZE_X;
int screen_size_y = DEF_SCREEN_SIZE_Y;
int cell_probability = (int) DEF_CELL_PROBABILITY;
int mark_deviations = FALSE;
int deviation_prob = (int) (RAND_MAX/100)*DEF_DEVIATION_PROBABILITY;
int rand_fd = 0;

/* pointer for random number generator function */
int (*get_rand)(void);

/* functions */

int read_rand(void)
{
	int rand_num;
	if (!(read(rand_fd,&rand_num,sizeof(int)))) {
		fprintf(stderr, "Error: read_rand(): couldn\'t read from file\n");
		exit(1);
	}
	printf("DEBUG: read_rand(): read %d\n",rand_num);
	// pozor, generuje i zaporne - fix it! <0;RAND_MAX>
	return rand_num;
}

int rand_decide(int limit)
{
	if (get_rand() > limit) {
		// printf("0\n");
		return FALSE;
	}
	// printf("1\n");
	return TRUE;
}

void redraw_marked(void)
{
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
				if (cell[i*world_size_x+j] == ALIVE) {
					glColor3f(1.0f,1.0f,1.0f);
				} else if (cell[i*world_size_x+j] == DEVIATION_ALIVE) {
					glColor3f(0.0f,0.0f,1.0f);
				} else if (cell[i*world_size_x+j] == DEVIATION_DEAD) {
					glColor3f(1.0f,0.0f,0.0f);
				} else {
					glColor3f(0.0f,0.0f,0.0f);
				}
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

void redraw(void)
{
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
				if (cell[i*world_size_x+j] > 0) {
					glColor3f(1.0f,1.0f,1.0f);
				} else {
					glColor3f(0.0f,0.0f,0.0f);
				}
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

void windowResize(int x, int y)
{
	glViewport(0,0,x,y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,screen_size_x,0,screen_size_y,-1,1);
}

void animate(int arg)
{
	int i,j,k,l;
	int cell_count;
	char *temp;
	if (!anim_pause) {
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
				if (cell_count == 3 || (cell[i*world_size_x+j] && cell_count == 2)) {
					/* cell should be alive */
					if (rand_decide(deviation_prob) == TRUE) {
						new_cell[i*world_size_x+j] = DEVIATION_DEAD;
					} else {
						new_cell[i*world_size_x+j] = ALIVE;
					}
				} else if (cell_count && rand_decide(deviation_prob) == TRUE) {
					/* deviation - only if theres more than 0 alive cells in neighbourhood */
					new_cell[i*world_size_x+j] = DEVIATION_ALIVE;
				} else {
					new_cell[i*world_size_x+j] = DEAD;
				}
					
			}
		}
		temp = cell;
		cell = new_cell;
		new_cell = temp;
		glutPostRedisplay();
	}
	glutTimerFunc(anime_interval,animate,0);
}

void cell_init(void)
{
	int i,j;
	for (i = 0; i != world_size_y; i++) {
		for (j = 0; j != world_size_x; j++) {
			if (get_rand()%100 < cell_probability) cell[i*world_size_x+j] = 1;
			else cell[i*world_size_x+j] = 0;
		}
	}
}

void kbd(unsigned char key, int x, int y)
{
	int i;
	if (key == 27 || key == 'q') exit(0);
	else if (key == ' ') anim_pause = ~anim_pause;
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

void mouse(int button, int state, int x, int y)
{
	x /= cell_size;
	y /= cell_size;
	cell[y*world_size_x+x] = (button == GLUT_LEFT_BUTTON ? 1 : 0);
	glutPostRedisplay();
}

void print_help()
{
	char *help_msg = "\t-r WxH world size\n\t-c CELL_SIZE block size of each cell (in pixels)\n\t-p PROBABILITY probability of deviation from GoL rules (in percents, default value 1)\n\t-m mark all deviated cells by blue color\n\t-g RANDOM_FILE read random data from file\n";
	printf("Probabilistic Game of Life\n%s",help_msg);
	return;
}

int main(int argc, char **argv)
{
	/* Parametry
	 * -r WxH world size
	 * -c CELL_SIZE block size of each cell (in pixels)
	 * -p PROBABILITY probability of deviation from GoL rules
	 * -m mark all deviated cells by blue color (only in first iteration)
	 * -g RANDOM_FILE read random data from file
	 * TODO pridat moznost random hondot ze souboru (moznost statickych nebo skutecne nahodnych (hw generator) hodnot)
	 */

	/* argument processing */
	int i = 1;
	int j;
	char *end;
	while (i < argc) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'r':
					if (++i >= argc) {
						fprintf(stderr, "Error: argument -r requires parameter\n");
						print_help();
						exit(1);
					}
					j = 0;
					while (argv[i][j] && argv[i][j] != 'x') j++;
					errno = 0;
					world_size_x = strtol((const char*)argv[i],&end,10);
					if (end[0] != 'x' || errno) {
						fprintf(stderr, "Error: wrong resolution form (WxH required)\n");
						print_help();
						exit(1);
					}
					errno = 0;
					world_size_y = strtol((const char*)++end,NULL,10);
					if (!(world_size_x && world_size_y) || errno) {
						fprintf(stderr, "2 Error: wrong resolution form (WxH required)\n");
						print_help();
						exit(1);
					}
					break;
				case 'c':
					if (++i >= argc) {
						fprintf(stderr, "Error: argument -c requires parameter\n");
						print_help();
						exit(1);
					}
					errno = 0;
					cell_size = (int)strtol(argv[i],NULL,10);
					if (cell_size <= 0 || errno) {
						fprintf(stderr, "Error: wrong cell size\n");
	 					exit(1);
					}
					break;
				case 'p':
					if (++i >= argc) {
						fprintf(stderr, "Error: argument -p requires parameter\n");
						print_help();
						exit(1);
					}
					errno = 0;
					deviation_prob = (int)strtol(argv[i],NULL,10);
					if (deviation_prob < 0 || deviation_prob > 100 || errno) {
						fprintf(stderr, "Error: wrong deviation probability <0;100>\n");
						print_help();
	 					exit(1);
					}
					deviation_prob *= (int) (RAND_MAX/100);
					break;
				case 'm':
					mark_deviations = TRUE;
					break;
				case 'h':
					print_help();
					exit(0);
					break;
				case 'g':
					if (++i >= argc) {
						fprintf(stderr, "Error: argument -g requires parameter\n");
						print_help();
						exit(1);
					}
					if (!(rand_fd = open((const char*)argv[i],O_RDONLY))) {
						fprintf(stderr, "Error: file %s couldn\'t be opened\n",argv[1]);
						print_help();
						exit(1);	
					}
					break;
				default:
					fprintf(stderr, "Error: unknows argument %s\n",argv[i]);
					print_help();
					exit(1);
			}
			i++;
		}
		else {
			fprintf(stderr, "wrong argument form %s\n", argv[i]);
			print_help();
			exit(1);
		}
	}
	/* compute window size */
	screen_size_x = world_size_x*cell_size;
	screen_size_y = world_size_y*cell_size;
	/* set and initialize random data source */
	if (rand_fd) {
		/* use file as random data src */
		get_rand = read_rand;
	} else {
		/* use default random generator */
		get_rand = rand;
		srand(time(NULL));
	}
	cell = (char *) malloc(world_size_x*world_size_y);
	new_cell = (char *) malloc(world_size_x*world_size_y);
	cell_init();
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Game of Life");
	glutReshapeWindow(screen_size_x,screen_size_y);
	glutPositionWindow(0,0);
	glutReshapeFunc(windowResize);
	if (mark_deviations) {
		glutDisplayFunc(redraw_marked);
	} else {
		glutDisplayFunc(redraw);
	}
	glutKeyboardFunc(kbd);
	glutMouseFunc(mouse);
	glutTimerFunc(anime_interval,animate,0);
	glutMainLoop();
	return 0;
}
