# Framebuffer OpenGL draw

Simple implementation of OpenGL functionality for drawing into linux framebuffer (implementing basic OpenGL API).

Library aims to be as much similar to OpenGL as possible. As of now, only wireframe
drawing is supported (no shading).

Demo: https://youtu.be/IYz0oabQolc

TODO list:
- fix transformations order (translate+rotate differs from openGL?)
- fix colors (hard-coded to BGR)

Note: if no /dev/fbX device is present: http://unix.stackexchange.com/questions/33596/no-framebuffer-device-how-to-enable-it
