#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <math.h>
#include <string.h>
#include <cblas.h>

#define FB_POINTS 1
#define FB_LINES 2
#define FB_LINE_STRIP 3
#define FB_TRIANGLES 4
#define FB_PROJECTION_MATRIX 1
#define FB_MODELVIEW_MATRIX 2

typedef struct {
	int fb_device;
	char *pixels;
	char *backbuff;
	int begin_type;
	double vertex[3][4]; /* 3 points x, y, z, w */
	char color[3][3]; /* 3 points R G B */
	char colbuf[3];
	int color_pointer;
	struct fb_var_screeninfo var_info;
	struct fb_fix_screeninfo fix_info;
	int line_width;
	char clear_color[3];
	/* matrices are row-major */
	int matrix_mode;
	double projection_matrix[16];
	double modelview_matrix[16];
} fbDrawContext;

fbDrawContext* fbDrawInit(const char *fb_dev_filename);
void fbDrawClose(fbDrawContext *c);
void fbFlush(fbDrawContext *c);
void fbSetPixel(fbDrawContext *c, int x, int y, char R, char G, char B);
void fbBegin(fbDrawContext *c, int type);
void fbEnd(fbDrawContext *c);
void fbVertex(fbDrawContext *c, double x, double y, double z);
void fbColor(fbDrawContext *c, char R, char G, char B);
void fbDrawLine(fbDrawContext *c, int *u, int *v, char *col);
void fbMatrixMode(fbDrawContext *c, int matrix_mode);
void fbLoadIdentity(fbDrawContext *c);
void fbFrustum(fbDrawContext *c, double left, double right, double bottom, double top, double near, double far);
void fbOrtho(fbDrawContext *c, double left, double right, double bottom, double top, double near, double far);
void fbTranslate(fbDrawContext *c, double x, double y, double z);
void fbRotate(fbDrawContext *c, double angle, double x, double y, double z);
void fbPrintMatrix(fbDrawContext *c);
void fbClearColor(fbDrawContext *c, char R, char G, char B);
void normalize_coords(double *vector);
void screen_coords(fbDrawContext *c, const double *normalized_vector, int *screen_coords);
int fbLineWidth(fbDrawContext *c, int width);
int clip(const double *vector);
int colcmp(char *c1, char *c2);
