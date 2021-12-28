#include "fb_draw.h"

fbDrawContext* fbDrawInit(const char *fb_dev_filename)
{
	struct fb_cursor cursor;
	fbDrawContext *c = (fbDrawContext*) malloc(sizeof(fbDrawContext));
	/* open framebuffer */
	c->fb_device = open(fb_dev_filename,O_RDWR);
	if (c->fb_device == -1) {
		perror("Couldn\'t open framebuffer device");
		return NULL;
	}
	/* get info structs */
	if (ioctl(c->fb_device,FBIOGET_VSCREENINFO,&(c->var_info))) {
		perror("Couldn\'t obtain info");
		close(c->fb_device);
		free(c);
		return NULL;
	}
	if (ioctl(c->fb_device,FBIOGET_FSCREENINFO,&(c->fix_info))) {
		perror("Couldn\'t obtain info");
		close(c->fb_device);
		free(c);
		return NULL;
	}
	/* IMPORTANT NOTICE: we assume 24 BPP true color (not palette!) mode is used;
 	 * framebuffer must be set to correct mode, eg. 1024x768 24 BPP True Color
 	 * can be set using kernel parameter 'vga=792'  
 	 */
	/* attempt to set color order */
	c->var_info.bits_per_pixel = 24;
	c->var_info.red.length = 8;
	c->var_info.red.offset = 0;
	c->var_info.red.msb_right = 0;
	c->var_info.green.length = 8;
	c->var_info.green.offset = 8;
	c->var_info.green.msb_right = 0;
	c->var_info.blue.length = 8;
	c->var_info.blue.offset = 16;
	c->var_info.blue.msb_right = 0;
	c->var_info.transp.length = 0;
	c->var_info.transp.offset = 24;
	c->var_info.transp.msb_right = 0;
	c->var_info.vmode = FB_VMODE_NONINTERLACED;
	c->var_info.xres = c->var_info.xres_virtual = 1024;
	c->var_info.yres = c->var_info.yres_virtual = 768;
	c->vertex[0][3] = 1;
	c->vertex[1][3] = 1;
	c->vertex[2][3] = 1;
	if (ioctl(c->fb_device,FBIOPUT_VSCREENINFO,&(c->var_info))) {
		perror("Failed to set framebuffer");
		close(c->fb_device);
		free(c);
		return NULL;
	}
	/* turn off cursor 
	cursor.enable = 0;
	cursor.mask = NULL;
	cursor.set = FB_CUR_SETALL;
	if (ioctl(c->fb_device,FBIO_CURSOR,&cursor)) {
		perror("Failed to turn off cursor");
		close(c->fb_device);
		free(c);
		return NULL;
	}*/
	/* display warning if unexpected mode is used */
	if (c->fix_info.type != FB_TYPE_PACKED_PIXELS) {
		fprintf(stderr,"Warning: pixels not packed, shit may happen\n");
	}
	if (c->fix_info.visual != FB_VISUAL_TRUECOLOR) {
		fprintf(stderr,"Warning: color mode not TrueColor, shit may happen\n");
	}
	/* mmap pixels */
	c->pixels = (char*) mmap(0,c->fix_info.smem_len,PROT_READ|PROT_WRITE,MAP_SHARED,c->fb_device,0);
	if (c->pixels == MAP_FAILED) {
		perror("Couldn\'t map pixels");
		close(c->fb_device);
		free(c);
	}
	/* create buffer for double-buffering */
	c->backbuff = (char*) malloc(c->fix_info.smem_len);
	/* set various default values */
	c->line_width = 1;
	return c;
}

/* fbFlush(); switch buffers and zero backbuffer (flush changes) */
void fbFlush(fbDrawContext *c)
{
	int i;
	char *tmp;

	//tmp = c->pixels;
	//c->pixels = c->backbuff;
	//c->backbuff = tmp;
	for (i = 0; i < c->fix_info.smem_len; i++) {
		c->pixels[i] = c->backbuff[i]; // so effective... switch buffers moron!
		c->backbuff[i] = 0;
	}
}

void fbDrawClose(fbDrawContext *c)
{
	if (c) {
		munmap(c->pixels,c->fix_info.smem_len);
		close(c->fb_device);
		free(c);
	}
}

void fbSetPixel(fbDrawContext *c, int x, int y, char R, char G, char B)
{
	int index = y*c->fix_info.line_length+x*3; /* 24 BPP */
	if (x < 0 || x > c->var_info.xres || y < 0 || y > c->var_info.yres) {
		return;
	}
	c->backbuff[index] = R;
	c->backbuff[index+1] = G;
	c->backbuff[index+2] = B;
}

void fbBegin(fbDrawContext *c, int type)
{
	c->begin_type = type;
	c->vertex[0][0] = c->vertex[1][0] = c->vertex[2][0] = 0.0/0.0;
}

void fbEnd(fbDrawContext *c)
{
	c->begin_type = 0;
}

void fbColor(fbDrawContext *c, char R, char G, char B)
{
	c->colbuf[0] = B;
	c->colbuf[1] = G;
	c->colbuf[2] = R;
}

void fbVertex(fbDrawContext *c, double x, double y, double z)
{
	double tp[3][4]; /* transformed point */
	double tpp[3][4]; /* perspective transformed point */
	int sc[3][2]; /* screen coords */
	if (c->begin_type == FB_POINTS) {
		fbSetPixel(c,(int)round(x),(int)round(y),c->colbuf[0],c->colbuf[1],c->colbuf[2]);
	}
	else if (c->begin_type == FB_LINES) {
		/* first point */
		if (isnan(c->vertex[0][0])) {
			c->vertex[0][0] = x;
			c->vertex[0][1] = y;
			c->vertex[0][2] = z;
			c->color[0][0] = c->colbuf[0];
			c->color[0][1] = c->colbuf[1];
			c->color[0][2] = c->colbuf[2];
		}
		/* second point */
		else {
			/* here we draw! and NaN the vertex buffer */
			/* if first and last color are the same, use draw func without interpolation */
			if (colcmp(c->color[0],c->colbuf)) {
				c->vertex[1][0] = x;
				c->vertex[1][1] = y;
				c->vertex[1][2] = z;
				c->vertex[0][3] = 1;
				c->vertex[1][3] = 1;
				/* tp = P*T*p */
				/* first point */
				cblas_dgemv(CblasRowMajor,CblasNoTrans,4,4,1.0,c->modelview_matrix,4,c->vertex[0],1,0.0,tp[0],1);
				cblas_dgemv(CblasRowMajor,CblasNoTrans,4,4,1.0,c->projection_matrix,4,tp[0],1,0.0,tpp[0],1);	
				/* second point */
				cblas_dgemv(CblasRowMajor,CblasNoTrans,4,4,1.0,c->modelview_matrix,4,c->vertex[1],1,0.0,tp[1],1);	
				cblas_dgemv(CblasRowMajor,CblasNoTrans,4,4,1.0,c->projection_matrix,4,tp[1],1,0.0,tpp[1],1);
				if (!((clip(tpp[0]) && clip(tpp[1])) && clip(tpp[2]))) {
					/* normalize x,y,z (divide by w) and get screen coordinates */
					normalize_coords(tpp[0]);
					normalize_coords(tpp[1]);
					screen_coords(c,tpp[0],sc[0]);
					screen_coords(c,tpp[1],sc[1]);
					fbDrawLine(c,sc[0],sc[1],c->colbuf);
				}
			}
			/* else use interpolating version */
			else {
				
			}
			c->vertex[0][0] = 0.0/0.0;
		}
	}
	else if (c->begin_type == FB_TRIANGLES) {
		/* first point */
		if (isnan(c->vertex[0][0])) {
			c->vertex[0][0] = x;
			c->vertex[0][1] = y;
			c->vertex[0][2] = z;
			c->color[0][0] = c->colbuf[0];
			c->color[0][1] = c->colbuf[1];
			c->color[0][2] = c->colbuf[2];
		}
		/* second point */
		else if (isnan(c->vertex[1][0])) {
			c->vertex[1][0] = x;
			c->vertex[1][1] = y;
			c->vertex[1][2] = z;
			c->color[1][0] = c->colbuf[0];
			c->color[1][1] = c->colbuf[1];
			c->color[1][2] = c->colbuf[2];

		}
		/* third point */
		else {
			/* here we draw! and NaN the vertex buffer */
			/* if first and last color are the same, use draw func without interpolation */
			if (colcmp(c->color[0],c->colbuf) && colcmp(c->color[1],c->colbuf)) {
				c->vertex[2][0] = x;
				c->vertex[2][1] = y;
				c->vertex[2][2] = z;
				c->vertex[0][3] = 1;
				c->vertex[1][3] = 1;
				c->vertex[2][3] = 1;
				/* tp = P*T*p */
				cblas_dgemv(CblasRowMajor,CblasNoTrans,4,4,1.0,c->modelview_matrix,4,c->vertex[0],1,0.0,tp[0],1);
				cblas_dgemv(CblasRowMajor,CblasNoTrans,4,4,1.0,c->projection_matrix,4,tp[0],1,0.0,tpp[0],1);	
				cblas_dgemv(CblasRowMajor,CblasNoTrans,4,4,1.0,c->modelview_matrix,4,c->vertex[1],1,0.0,tp[1],1);	
				cblas_dgemv(CblasRowMajor,CblasNoTrans,4,4,1.0,c->projection_matrix,4,tp[1],1,0.0,tpp[1],1);	
				cblas_dgemv(CblasRowMajor,CblasNoTrans,4,4,1.0,c->modelview_matrix,4,c->vertex[2],1,0.0,tp[2],1);	
				cblas_dgemv(CblasRowMajor,CblasNoTrans,4,4,1.0,c->projection_matrix,4,tp[2],1,0.0,tpp[2],1);
				/*printf("tp:\n");
				printf("%lf %lf %lf %lf\n",tp[0][0],tp[0][1],tp[0][2],tp[0][3]);
				printf("%lf %lf %lf %lf\n",tp[1][0],tp[1][1],tp[1][2],tp[1][3]);
				printf("%lf %lf %lf %lf\n",tp[2][0],tp[2][1],tp[2][2],tp[2][3]);
				printf("tpp:\n");
				printf("%lf %lf %lf %lf\n",tpp[0][0],tpp[0][1],tpp[0][2],tpp[0][3]);
				printf("%lf %lf %lf %lf\n",tpp[1][0],tpp[1][1],tpp[1][2],tpp[1][3]);
				printf("%lf %lf %lf %lf\n",tpp[2][0],tpp[2][1],tpp[2][2],tpp[2][3]);*/
				/* clipping - do not draw objects with vertices out of view frustum */
				if (!((clip(tpp[0]) && clip(tpp[1])) && clip(tpp[2]))) {
					/* normalize x,y,z (divide by w) and get screen coordinates */
					normalize_coords(tpp[0]);
					normalize_coords(tpp[1]);
					normalize_coords(tpp[2]);
					screen_coords(c,tpp[0],sc[0]);
					screen_coords(c,tpp[1],sc[1]);
					screen_coords(c,tpp[2],sc[2]);
					fbDrawLine(c,sc[0],sc[1],c->colbuf);
					fbDrawLine(c,sc[0],sc[2],c->colbuf);
					fbDrawLine(c,sc[1],sc[2],c->colbuf);
				}
			}
			/* else use interpolating version */
			else
			{

			}
			c->vertex[0][0] = 0.0/0.0;
			c->vertex[1][0] = 0.0/0.0;
		}
	}
}

/* helper functions */
int colcmp(char *c1, char *c2)
{
	int i;
	for (i = 0; i < 3; i++) {
		if (c1[i] != c2[i]) return 0;
	}
	return 1;
}

/* draw line, no antialias, no interpolation */
//void fbDrawLine(fbDrawContext *c, int x1, int y1, int x2, int y2, char R, char G, char B)
void fbDrawLine(fbDrawContext *c, int *u, int *v, char *col)
{
	const int dx = u[0]-v[0];
	const int dy = u[1]-v[1];
	int i;
	int control_coord;
	double second_coord;
	double slope;
	int *temp;

	/* control axis X */
	if (dy == 0 || fabs((double)dy/dx) < 1) {
		if (v[0] < u[0]) {
			temp = v;
			v = u;
			u = temp;
		}
		slope = (double)dy/dx;
		for (control_coord = u[0], second_coord = (double)u[1]; control_coord <= v[0]; control_coord++) {
			/* very crude and not exact, but who cares */
			for (i = 0; i < c->line_width; i++) { 
				fbSetPixel(c,control_coord,(double)second_coord+i,col[0],col[1],col[2]);
			}
			second_coord += slope;
		} 
	}
	/* control axis Y */
	else
	{
		if (v[1] < u[1]) {
			temp = v;
			v = u;
			u = temp;
		}
		slope = (double)dx/dy;
		for (control_coord = u[1], second_coord = (double)u[0]; control_coord <= v[1]; control_coord++) {
			for (i = 0; i < c->line_width; i++) { 
				fbSetPixel(c,(double)second_coord+i,control_coord,col[0],col[1],col[2]);
			}
			second_coord += slope;	
		} 
	}
}

int fbLineWidth(fbDrawContext *c, int width)
{
	if (width <= 0) {
		return 0;
	}
	return c->line_width = width;
}

void fbMatrixMode(fbDrawContext *c, int matrix_mode)
{
	if (matrix_mode == FB_MODELVIEW_MATRIX || matrix_mode == FB_PROJECTION_MATRIX) {
		c->matrix_mode = matrix_mode;
	}
}

void fbPrintMatrix(fbDrawContext *c)
{
	int i,j;
	double *matrix;
	if (c->matrix_mode == FB_PROJECTION_MATRIX) {
		matrix = c->projection_matrix;
	} else if (c->matrix_mode == FB_MODELVIEW_MATRIX) {
		matrix = c->modelview_matrix;
	}
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("%lf ",matrix[4*i+j]);
		}
		putchar('\n');
	}
}

void fbLoadIdentity(fbDrawContext *c)
{
	int i;
	double *matrix;
	if (c->matrix_mode == FB_PROJECTION_MATRIX) {
		matrix = c->projection_matrix;
	} else if (c->matrix_mode == FB_MODELVIEW_MATRIX) {
		matrix = c->modelview_matrix;
	}
	for (i = 0; i < 16; i++) {
		matrix[i] = 0.0;
	}
	matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0;
}

void fbFrustum(fbDrawContext *c, double left, double right, double bottom, double top, double near, double far)
{
	/* NOTE! both near and far must be positive, bottom != top, near != far, left != far,
	 * this function does not check these conditions! */
	/* perspective projection */
	double P[16];
	double temp[16];
	double *matrix;

	/* get matrix mode */
	if (c->matrix_mode == FB_PROJECTION_MATRIX) {
		matrix = c->projection_matrix;
	} else if (c->matrix_mode == FB_MODELVIEW_MATRIX) {
		matrix = c->modelview_matrix;
	}
	/* set perspective matrix - row major */
	P[0] = 2.0*near/(right-left);
	P[1] = 0;
	P[2] = (right+left)/(right-left);
	P[3] = 0.0;
	P[4] = 0.0;
	P[5] = 2.0*near/(top-bottom);
	P[6] = (top+bottom)/(top-bottom);
	P[7] = 0.0;
	P[8] = 0.0;
	P[9] = 0.0;
	P[10] = -(far+near)/(far-near);
	P[11] = -2*(far*near)/(far-near);
	P[12] = 0.0;
	P[13] = 0.0;
	P[14] = -1.0;
	P[15] = 0.0;
	/* multiply P with current matrix */
	cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,4,4,4,1.0,P,4,matrix,4,0.0,temp,4);
	memcpy(matrix,temp,sizeof(double)*16);
}

void fbTranslate(fbDrawContext *c, double x, double y, double z)
{
	double T[16];
	double temp[16];
	double *matrix;

	/* get matrix mode */
	if (c->matrix_mode == FB_PROJECTION_MATRIX) {
		matrix = c->projection_matrix;
	} else if (c->matrix_mode == FB_MODELVIEW_MATRIX) {
		matrix = c->modelview_matrix;
	}
	/* set translation matrix - row major */
	T[0] = 1; T[1] = 0; T[2] = 0; T[3] = x;
	T[4] = 0; T[5] = 1; T[6] = 0; T[7] = y;
	T[8] = 0; T[9] = 0; T[10] = 1; T[11] = z;
	T[12] = 0; T[13] = 0; T[14] = 0; T[15] = 1;
	/* multiply P with current matrix */
	cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,4,4,4,1.0,T,4,matrix,4,0.0,temp,4);
	memcpy(matrix,temp,sizeof(double)*16);
}

void fbRotate(fbDrawContext *c, double angle, double x, double y, double z)
{
	double R[16];
	double temp[16];
	double *matrix;
	double cs = cos(angle/180.0*M_PI);
	double sn = sin(angle/180.0*M_PI);
	double len = sqrt(x*x+y*y+z*z);

	/* get matrix mode */
	if (c->matrix_mode == FB_PROJECTION_MATRIX) {
		matrix = c->projection_matrix;
	} else if (c->matrix_mode == FB_MODELVIEW_MATRIX) {
		matrix = c->modelview_matrix;
	}
	/* normalize vector */
	x /= len;
	y /= len;
	z /= len;

	/* Set rotation matrix - row major */
	R[0] = x*x*(1.0-cs)+cs;
	R[1] = x*y*(1.0-cs)-z*sn;
	R[2] = x*z*(1.0-cs)+y*sn;
	R[3] = 0.0;
	
	R[4] = y*x*(1.0-cs)+z*sn;
	R[5] = y*y*(1.0-cs)+cs;
	R[6] = y*z*(1.0-cs)-x*sn;
	R[7] = 0.0;

	R[8] = x*z*(1.0-cs)-y*sn;
	R[9] = y*z*(1.0-cs)+x*sn;
	R[10] = z*z*(1.0-cs)+cs;
	R[11] = 0.0;

	R[12] = 0.0;
	R[13] = 0.0;
	R[14] = 0.0;
	R[15] = 1.0;

	/* multiply P with current matrix */
	cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,4,4,4,1.0,R,4,matrix,4,0.0,temp,4);
	memcpy(matrix,temp,sizeof(double)*16);
}

int clip(const double *v)
{
	/* return 1 if x or y or z does not lie in interval [-w;+w] (assuming vector has form (x,y,z,w) ) */
	double w = fabs(v[3]);
	return (fabs(v[0]) > w || fabs(v[1]) > w || fabs(v[2]) > w);
}

void normalize_coords(double *v)
{
	v[0] /= v[3];
	v[1] /= v[3];
	v[2] /= v[3];
	v[3] = 1.0;
}

void screen_coords(fbDrawContext *c, const double *v, int *sc)
{
	sc[0] = 0.5*(v[0]*c->var_info.xres+c->var_info.xres);
	sc[1] = 0.5*(v[1]*c->var_info.xres+c->var_info.yres);
}

void fbClearColor(fbDrawContext *c, char R, char G, char B)
{
	c->clear_color[0] = B;
	c->clear_color[1] = G;
	c->clear_color[2] = R;
}
