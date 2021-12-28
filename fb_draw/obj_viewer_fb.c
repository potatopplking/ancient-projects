#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include "fb_draw.h"

#define CAM_FOV 70.0 /* camera field-of-view angle */

double camLeft, camRight, camBottom, camTop, camNear, camFar;
double x_rot = 0.0;
double y_rot = 0.0;
double cam_dist = -300.0;
double win_aspect;
int window_width;
int window_height;
int object_count = 0;
double *vertex;
double *normals;
int *faces;
int *faces_count;
fbDrawContext *c;

void draw_axis()
{
	fbBegin(c,FB_LINES);
		fbColor(c,255,0,0);
		fbVertex(c,0.0,0.0,0.0);
		fbVertex(c,20.0,0.0,0.0);
		fbColor(c,0,255,0);
		fbVertex(c,0.0,0.0,0.0);
		fbVertex(c,0.0,20.0,0.0);
		fbColor(c,0,0,255);
		fbVertex(c,0.0,0.0,0.0);
		fbVertex(c,0.0,0.0,20.0);
	fbEnd(c);
}

void resize_action(int x, int y) {
	camNear = 20.0;
	camFar = 10000.0;

	window_width = x;
	window_height = y;
	win_aspect = ((double)x)/y;
	/* Setting viewport - visible part of window */
	//glViewport(0,0,x,y);

	/* Setting values for projection matrix - camera */
	camRight = camNear*tan(CAM_FOV*M_PI/360.0);
	camLeft = -1.0*camRight;
	camTop = camRight/win_aspect;
	camBottom = -1.0*camTop;
}

void redraw()
{
	int i,j,prev_vert;
	/* Setting perspective camera (for 3D scene) */
	fbMatrixMode(c,FB_PROJECTION_MATRIX);
	fbLoadIdentity(c);
	fbFrustum(c,camLeft,camRight,camBottom,camTop,camNear,camFar);

	/* Using modelview matrix */
	fbMatrixMode(c,FB_MODELVIEW_MATRIX);
	fbLoadIdentity(c);

	/* Rotating and translating scene - for TESTING purposes only */
	fbRotate(c,y_rot,1.0,0.0,0.0);
	fbRotate(c,x_rot,0.0,0.0,1.0);
	fbTranslate(c,0.0,0.0,cam_dist); /* pozor - upraveno oproti originalu, translate ma byt nad rotacemi */
	draw_axis();

	fbColor(c,100,100,100);
	prev_vert = 0;
	for (i = 0; i < object_count; i++) {
		fbBegin(c,FB_TRIANGLES);
		for (j = 0; j < faces_count[i]; j++) {
			fbVertex(c,vertex[3*faces[3*j+prev_vert]],vertex[3*faces[3*j+prev_vert]+1],vertex[3*faces[3*j+prev_vert]+2]);
			fbVertex(c,vertex[3*faces[3*j+prev_vert+1]],vertex[3*faces[3*j+prev_vert+1]+1],vertex[3*faces[3*j+prev_vert+1]+2]);
			fbVertex(c,vertex[3*faces[3*j+prev_vert+2]],vertex[3*faces[3*j+prev_vert+2]+1],vertex[3*faces[3*j+prev_vert+2]+2]);
		}
		prev_vert += 3*faces_count[i];
		fbEnd(c);
	}
	fbFlush(c);
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
	int ret_code = 0;
	char command;

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
	/* fb lib init */
	c = fbDrawInit((const char *)"/dev/fb0");
	if (c == NULL) {
		fprintf(stderr,"error: couldn\'t open framebuffer device\n");
		ret_code = 1;
		goto cleanup_exit;
	}
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
	free(buffer);
	printf("Entering main loop...\n");
	resize_action(c->var_info.xres,c->var_info.yres);
	/* set tty not to buffer (do not wait for enter) */
	system("stty raw");
	while (1) {
		redraw();
		switch (getchar()) {
			case '8':
				y_rot += 10;
				break;
			case '5':
				y_rot -= 10;
				break;
			case '4':
				x_rot -= 10;
				break;
			case '6':
				x_rot += 10;
				break;
			case '+':
				cam_dist += 100;
				break;
			case '-':
				cam_dist -= 100;
				break;
			case 'q':
			case 27:
				goto cleanup_exit;
		}
	}
	system("stty cooked");
cleanup_exit:
	fbDrawClose(c);
	free(faces);
	free(vertex);
	free(normals);
	free(faces_count);
	exit(ret_code);
}
