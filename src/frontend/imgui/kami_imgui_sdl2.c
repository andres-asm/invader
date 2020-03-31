#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "kami.h"
#include "common.h"
#include "config.h"
#include "util.h"

static const char* tag = "[kami]";
static const char* app_name = "invader";

int main(int argc, char* argv[])
{

   create_window(app_name, WINDOW_WIDTH, WINDOW_HEIGHT);
   SDL_Window *window = get_window();
   SDL_GLContext context = get_context();

   const char* glsl_version = get_glsl_version();

   /* setup imgui */
   igCreateContext(NULL);
   ImGuiIO io = *igGetIO();
   ImGui_ImplSDL2_InitForOpenGL(window, context);
   ImGui_ImplOpenGL3_Init(glsl_version);

   igStyleColorsDark(NULL);
   ImFontAtlas_AddFontDefault(io.Fonts, NULL);

   bool quit = false;
   bool showDemoWindow = true;
   bool showAnotherWindow = false;
   ImVec4 clearColor;
   clearColor.x = 0.45f;
   clearColor.y = 0.55f;
   clearColor.z = 0.60f;
   clearColor.w = 1.00f;

   while (!quit)
   {
      SDL_Event e;

      while (SDL_PollEvent(&e) != 0)
      {
         ImGui_ImplSDL2_ProcessEvent(&e);
         if (e.type == SDL_QUIT)
            quit = true;
         if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(window))
            quit = true;
      }

      /* start imgui frame */
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL2_NewFrame(window);
      igNewFrame();

      if (showDemoWindow)
         igShowDemoWindow(&showDemoWindow);

      {
         static float f = 0.0f;
         static int counter = 0;

         igBegin("Hello, world!", NULL, 0);
         igText("This is some useful text");
         igCheckbox("Demo window", &showDemoWindow);
         igCheckbox("Another window", &showAnotherWindow);

         igSliderFloat("Float", &f, 0.0f, 1.0f, "%.3f", 1.0f);
         igColorEdit3("clear color", (float*)&clearColor, 0);

         ImVec2 buttonSize;
         buttonSize.x = 0;
         buttonSize.y = 0;
         if (igButton("Button", buttonSize))
            counter++;
         igSameLine(0.0f, -1.0f);
         igText("counter = %d", counter);

         igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
         igEnd();
      }

      if (showAnotherWindow)
      {
         igBegin("imgui Another Window", &showAnotherWindow, 0);
         igText("Hello from imgui");
         ImVec2 buttonSize;
         buttonSize.x = 0; buttonSize.y = 0;
         if (igButton("Close me", buttonSize))
         {
            showAnotherWindow = false;
         }
         igEnd();
      }

      /* render */
      igRender();
      SDL_GL_MakeCurrent(window, context);
      glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
      glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
      glClear(GL_COLOR_BUFFER_BIT);
      ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
      SDL_GL_SwapWindow(window);
   }

   /* clean up */
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   igDestroyContext(NULL);

   SDL_GL_DeleteContext(get_context());
   if (window != NULL)
   {
      SDL_DestroyWindow(window);
      window = NULL;
   }
   SDL_Quit();

   return 0;
}