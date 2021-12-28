#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define RES_X 11520
#define RES_Y 5120

#define MAX_ITERATIONS 100

typedef struct {
	double r,i;
} complex;

int mandelbrot(complex num) {
	complex c = num;
	int i;
	double temp;

	for (i = 0; fabs(num.r*num.r+num.i*num.i) < 4 && i != MAX_ITERATIONS; i++) {
		temp = num.r*num.r-num.i*num.i+c.r;
		num.i = 2*num.r*num.i+c.i;
		num.r = temp;
	}
	if (i != 200) return i+i/5;   // pozor, hard-coded: (i+i/5 = 1.2 ~= 1.22 = 255 / 200)
	return 255;
}
int main(void)
{
	complex num;
	double stepX = 3.0/RES_X;
	double stepY = -2.0/RES_Y;
	int i,j;
	FILE* fd;
	//char bitmap[RES_X*RES_Y+17] = "P5 11520 5120 255 ";
	char *bitmap = malloc(RES_X*RES_Y+18);
	bitmap[0] = 'P';
	bitmap[1] = '5';
	bitmap[2] = ' ';
	bitmap[3] = '1';
	bitmap[4] = '1';
	bitmap[5] = '5';
	bitmap[6] = '2';
	bitmap[7] = '0';
	bitmap[8] = ' ';
	bitmap[9] = '5';
	bitmap[10] = '1';
	bitmap[11] = '2';
	bitmap[12] = '0';
	bitmap[13] = ' ';
	bitmap[14] = '2';
	bitmap[15] = '5';
	bitmap[16] = '5';
	bitmap[17] = ' ';

	num.i = 1;

	for (i = 0; i != RES_Y; i++)
	{
		for (j = 0; j != RES_X; j++) {
		bitmap[i*RES_X+j+18] = mandelbrot(num);
		num.r += stepX;
		}
		num.r = -2;
		num.i += stepY;
	}
	if (!(fd = fopen("out.pgm","w+"))) {
		fprintf(stderr,"Nepodarilo se otevrit soubor.\n");
		exit(1);
	}
	fwrite(bitmap,RES_X*RES_Y+18,1,fd);
	fclose(fd);
	return 0;
}
