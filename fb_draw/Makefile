LIBS=-lm -lcblas
CC=clang
all: fbtest obj
fbtest:
	$(CC) -o fb_draw_test -L $(LIBINCLUDE) $(LIBS) fb_draw_test.c fb_draw.c
obj:
	$(CC) -o obj_viewer_fb -L $(LIBINCLUDE) $(LIBS) obj_viewer_fb.c fb_draw.c
