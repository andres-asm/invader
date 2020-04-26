/* Common defines, includes and variables for all implementations */
#ifndef COMMON_H_
#define COMMON_H_

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <libintl.h>
#include <locale.h>
#define _(str) gettext(str)

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define UNUSED(a) (void)a
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a) / sizeof(a)[0])

#define MAX_SIZE 256
#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

extern SDL_Window* mywindow;
extern SDL_GLContext mycontext;

extern const char* glsl_version;

bool create_window(const char* app_name, unsigned width, unsigned height);
void destroy_window();

SDL_GLContext get_context();
SDL_Window* get_window();

const char* get_glsl_version();

void common_config_load();

#endif