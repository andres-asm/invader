#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "invader.h"
#include "nuklear_sdl_gl3.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

int main(int argc, char *argv[])
{
   /* Platform */
   SDL_Window *win;
   SDL_GLContext glContext;
   int win_width, win_height;
   int running = 1;

   /* SDL setup */
   SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
   SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS);
   SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
   SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   win = SDL_CreateWindow("Invader",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
   glContext = SDL_GL_CreateContext(win);
   SDL_GetWindowSize(win, &win_width, &win_height);

   /* OpenGL setup */
   glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
   glewExperimental = 1;
   if (glewInit() != GLEW_OK)
   {
      fprintf(stderr, "Failed to setup GLEW\n");
      exit(1);
   }

   ctx = nk_sdl_init(win);
   /* Load Fonts: if none of these are loaded a default font will be used  */
   /* Load Cursor: if you uncomment cursor loading please hide the cursor */
   {
      struct nk_font_atlas *atlas;
      nk_sdl_font_stash_begin(&atlas);
      /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
      nk_sdl_font_stash_end();
      /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
      /*nk_style_set_font(ctx, &roboto->handle);*/}

      bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
      while (running)
      {
         /* Input */
         SDL_Event evt;
         render_gui(ctx);
         nk_input_begin(ctx);
         while (SDL_PollEvent(&evt))
         {
            if (evt.type == SDL_QUIT) goto cleanup;
            nk_sdl_handle_event(&evt);
         }
         nk_input_end(ctx);

         /* Draw */
         SDL_GetWindowSize(win, &win_width, &win_height);
         glViewport(0, 0, win_width, win_height);
         glClear(GL_COLOR_BUFFER_BIT);
         glClearColor(bg.r, bg.g, bg.b, bg.a);
         /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
          * with blending, scissor, face culling, depth test and viewport and
          * defaults everything back into a default state.
          * Make sure to either a.) save and restore or b.) reset your own state after
          * rendering the UI. */
         nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
         SDL_GL_SwapWindow(win);
      }

cleanup:
      nk_sdl_shutdown();
      SDL_GL_DeleteContext(glContext);
      SDL_DestroyWindow(win);
      SDL_Quit();
      return 0;
}

