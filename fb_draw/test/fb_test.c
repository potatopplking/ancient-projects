#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>

typedef struct fb_var_screeninfo FramebufferInfo;
typedef struct fb_fix_screeninfo NodeInfo;

void printFramebufferInfo(int fb_dev)
{
	FramebufferInfo fb_info;
	NodeInfo node_info;
	if (ioctl(fb_dev,FBIOGET_VSCREENINFO,&fb_info)) {
		fprintf(stderr,"Cannot get framebuffer device info: ioctl call failed\n");
		return;
	}
	if (ioctl(fb_dev,FBIOGET_FSCREENINFO,&node_info)) {
		fprintf(stderr,"Cannot get framebuffer device info: ioctl call failed\n");
		return;
	}
	printf("Framebuffer device information:\nReal resolution:\t%dx%d\n",fb_info.xres,fb_info.yres);
	printf("Virtual resolution:\t%dx%d\n",fb_info.xres_virtual,fb_info.yres_virtual);
	printf("Bits per pixel:\t%d\n",fb_info.bits_per_pixel);
	printf("Identification:\t%s\n",node_info.id);
	printf("Line length:\t%d B\n",node_info.line_length);
	printf("Framebuffer size:\t%d B\n",node_info.smem_len);
}

int getFramebufferSize(int fb_dev)
{
	NodeInfo node_info;
	if (ioctl(fb_dev,FBIOGET_FSCREENINFO,&node_info)) {
		fprintf(stderr,"Cannot get framebuffer device info: ioctl call failed\n");
		return -1;
	}
	return node_info.smem_len;
}

int main(int argc, char **argv)
{
	int fb_device;
	int buffer_size;
	char *pixels;
	int i;

	if (argc != 2) {
		fprintf(stderr,"No framebuffer file specified\n");
		exit(1);
	}
	fb_device = open(argv[1],O_RDWR);
	if (fb_device != -1) {
		printFramebufferInfo(fb_device);
	} else {
		fprintf(stderr,"Couldn\'t open framebuffer file %s\n",argv[1]);
		exit(1);
	}
	/* mmap framebuffer device */
	buffer_size = getFramebufferSize(fb_device);
	pixels = (char*) mmap(0,buffer_size,PROT_READ|PROT_WRITE,MAP_SHARED,fb_device,0);
	if (pixels == MAP_FAILED) {
		perror("Cannot access framebuffer (mmap failed)\n");
		close(fb_device);
		exit(-1);
	}
	/* set colors */
	for (i = 0; i < buffer_size; i++) {
		pixels[i] = (char) i;
	}
	/* unmap and close */
	munmap(pixels,buffer_size);
	close(fb_device);
	return 0;
}
