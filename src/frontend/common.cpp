#include "common.h"
#include "util.h"

static const char* tag = "[common]";
const char* glsl_version;

SDL_Window* invader_window = NULL;
SDL_GLContext invader_context = NULL;

bool create_window(const char* app_name, unsigned width, unsigned height)
{
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) == -1)
      logger(LOG_ERROR, tag, SDL_GetError());
      /* Decide GL+GLSL versions */
#if __APPLE__
   /* GL 3.2 Core + GLSL 150 */
   glsl_version = "#version 150";
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); /* Always required on Mac */
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
   /* GL 3.0 + GLSL 130 */
   glsl_version = "#version 130";
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

   /* and prepare OpenGL stuff */
   SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
   SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_DisplayMode current;
   SDL_GetCurrentDisplayMode(0, &current);

   uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;

   invader_window = SDL_CreateWindow(app_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
   if (invader_window == NULL)
   {
      logger(LOG_DEBUG, tag, "failed to create window: %s", SDL_GetError());
      return -1;
   }

   invader_context = SDL_GL_CreateContext(invader_window);
   /* enable vsync */
   SDL_GL_SetSwapInterval(1);

   /* check opengl version sdl uses */
   logger(LOG_DEBUG, tag, "opengl version: %s", (char*)glGetString(GL_VERSION));

   glViewport(0, 0, width, height);
   glewExperimental = 1;

   bool err = glewInit() != GLEW_OK;

   if (err)
   {
      logger(LOG_DEBUG, tag, "Failed to initialize OpenGL loader!");
      return 1;
   }

   return true;
}

void destroy_window()
{
   SDL_GL_DeleteContext(invader_context);
   if (invader_window != NULL)
   {
      SDL_DestroyWindow(invader_window);
      invader_window = NULL;
   }
   SDL_Quit();
}

SDL_GLContext get_context()
{
   return invader_context;
}

SDL_Window* get_window()
{
   return invader_window;
}

const char* get_glsl_version()
{
   return glsl_version;
}

/* Initialize configuration */
void common_config_load()
{ }