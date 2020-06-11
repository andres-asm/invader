#ifndef COMMON_H_
#define COMMON_H_

// system
#include <libintl.h>
#include <locale.h>

// TODO: ifdef this for other possible backends, OpenGL only for now
// GLEW
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "settings.h"
#include "util.h"

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

extern SDL_Window* invader_window;
extern SDL_GLContext invader_context;

extern const char* glsl_version;

bool create_window(const char* app_name, unsigned width, unsigned height);
void destroy_window();

void set_fullscreen_mode(bool fullscreen, bool desktop);
void set_vsync_mode(bool vsync);

SDL_GLContext get_context();
SDL_Window* get_window();

const char* get_glsl_version();

void common_config_load();

#endif