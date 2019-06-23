/* nuklear - 1.32.0 - public domain */
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

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#include "invader.h"
#include "nuklear_glfw_gl3.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024


static void error_callback(int e, const char *d)
{
   printf("Error %d: %s\n", e, d);
}

int main(void)
{
   /* Platform */
   static GLFWwindow *win;
   int width = 0, height = 0;
   struct nk_context *ctx;
   struct nk_colorf bg;

   /* GLFW */
   glfwSetErrorCallback(error_callback);
   if (!glfwInit())
   {
      fprintf(stdout, "[GFLW] failed to init!\n");
      exit(1);
   }
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
   win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "invader GLFW3", NULL, NULL);
   glfwMakeContextCurrent(win);
   glfwGetWindowSize(win, &width, &height);

   /* OpenGL */
   glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
   glewExperimental = 1;
   if (glewInit() != GLEW_OK)
   {
      fprintf(stderr, "[invader] Failed to setup GLEW\n");
      exit(1);
   }

   /* Configuration setup */
   cfg_load();

   ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
   /* Load Fonts: if none of these are loaded a default font will be used  */
   /* Load Cursor: if you uncomment cursor loading please hide the cursor */
   {
      struct nk_font_atlas *atlas;
      nk_glfw3_font_stash_begin(&atlas);
      /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
      /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
      /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
      /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
      /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
      /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
      nk_glfw3_font_stash_end();
      /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
      /*nk_style_set_font(ctx, &droid->handle);*/}

   bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
   while (!glfwWindowShouldClose(win))
   {
      /* Input */
      glfwPollEvents();
      nk_glfw3_new_frame();

      gui_render(ctx);

      /* Draw */
      glfwGetWindowSize(win, &width, &height);
      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);
      glClearColor(bg.r, bg.g, bg.b, bg.a);
      /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
       * with blending, scissor, face culling, depth test and viewport and
       * defaults everything back into a default state.
       * Make sure to either a.) save and restore or b.) reset your own state after
       * rendering the UI. */
      nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
      glfwSwapBuffers(win);
   }
   nk_glfw3_shutdown();
   glfwTerminate();
   return 0;
}

