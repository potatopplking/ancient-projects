#include "fb_draw.h"

int main(void)
{
	fbDrawContext *c;
	double angle;
	int x,y,i;

	c = fbDrawInit((const char*)"/dev/fb0");
	if (c == NULL) {
		return 1;
	}
	
	fbColor(c,0,0,250);
	/* matrix init */
	fbMatrixMode(c,FB_PROJECTION_MATRIX);
	fbLoadIdentity(c);
	printf("xres = %d\tyres = %d\n",c->var_info.xres,c->var_info.yres);
	/* set perspective matrix */
	fbFrustum(c,-50.0,50.0,-35.0,35.0,50.0,1000.0);
	printf("Projection matrix:\n");
	fbPrintMatrix(c);
	printf("\n\n");
	fbMatrixMode(c,FB_MODELVIEW_MATRIX);
	fbLoadIdentity(c);
	printf("Modelview matrix:\n");
	fbPrintMatrix(c);
	printf("\n\n");
	fbBegin(c,FB_LINES);
		fbVertex(c,20.0,10.0,-70.0);
		fbVertex(c,20.0,20.0,-70.0);
		fbVertex(c,20.0,15.0,-70.0);
		fbVertex(c,30.0,15.0,-70.0);
		fbVertex(c,30.0,10.0,-70.0);
		fbVertex(c,30.0,20.0,-70.0);
		fbVertex(c,40.0,10.0,-70.0);
		fbVertex(c,40.0,20.0,-70.0);
	fbEnd(c);
	fbFlush(c);
	sleep(1);
	fbColor(c,123,250,50);
	printf("\n\n");
	//fbLineWidth(c,5);
	for (i = 0; i < 3600; i++) {
		angle = (double)i;
		fbMatrixMode(c,FB_MODELVIEW_MATRIX);
		fbLoadIdentity(c);
		//fbTranslate(c,0.0,0.0,-200);
		fbRotate(c,angle,0.0,0,1.0);
		fbBegin(c,FB_TRIANGLES);
			fbVertex(c,5.0,0.0,-70.0);
			fbVertex(c,15.0,10.0,-70.0);
			fbVertex(c,25.0,15.0,-70.0);
			printf("\n");
			fbVertex(c,25.0,15.0,-70.0);
			fbVertex(c,15.0,10.0,-70.0);
			fbVertex(c,0.0,40.0,-70.0);
		fbEnd(c);
		fbFlush(c);
		usleep(100/520);
	}
	sleep(1);
	fbDrawClose(c);
	return 0;
}
