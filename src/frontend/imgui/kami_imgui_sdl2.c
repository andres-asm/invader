#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "kami.h"
#include "common.h"
#include "config.h"
#include "util.h"

#include <compat/strl.h>
#include <string/stdstring.h>

static const char* tag = "[kami]";
static const char* app_name = "invader";

static bool quit = false;
static bool showDemoWindow = true;
static bool showAnotherWindow = false;

const char* glsl_version;

ImVec4 clearColor;

SDL_Window *window;
SDL_GLContext context;

ImGuiIO io;

static void init_localization()
{
   setlocale (LC_ALL, "");
   bindtextdomain ("invader", "./intl/");
   textdomain ("invader");
}

static void imgui_shutdown()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   igDestroyContext(NULL);
}

static void imgui_setup()
{
   igCreateContext(NULL);
   io = *igGetIO();
   ImGui_ImplSDL2_InitForOpenGL(window, context);
   ImGui_ImplOpenGL3_Init(glsl_version);

   igStyleColorsDark(NULL);
   ImFontAtlas_AddFontDefault(io.Fonts, NULL);
}

static void imgui_set_default_style()
{
    ImGuiStyle *style = igGetStyle();

   style->WindowPadding = *ImVec2_ImVec2Float(15, 15);
   style->WindowRounding = 0.0f;
   style->FramePadding = *ImVec2_ImVec2Float(5, 5);
   style->FrameRounding = 0.0f;
   style->ItemSpacing = *ImVec2_ImVec2Float(12, 8);
   style->ItemInnerSpacing = *ImVec2_ImVec2Float(8, 6);
   style->IndentSpacing = 25.0f;
   style->ScrollbarSize = 10.0f;
   style->ScrollbarRounding = 0.0f;
   style->GrabMinSize = 10.0f;
   style->GrabRounding = 0.0f;
   style->TabRounding = 0.0f;

   style->Colors[ImGuiCol_Text] = *ImVec4_ImVec4Float(0.80f, 0.80f, 0.83f, 1.00f);
   style->Colors[ImGuiCol_TextDisabled] = *ImVec4_ImVec4Float(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_Border] = *ImVec4_ImVec4Float(0.16f, 0.15f, 0.17f, 1.00f);
   style->Colors[ImGuiCol_WindowBg] = *ImVec4_ImVec4Float(0.16f, 0.15f, 0.17f, 1.00f);
   style->Colors[ImGuiCol_ChildBg] = *ImVec4_ImVec4Float(0.07f, 0.07f, 0.09f, 1.00f);
   style->Colors[ImGuiCol_PopupBg] = *ImVec4_ImVec4Float(0.07f, 0.07f, 0.09f, 1.00f);
   style->Colors[ImGuiCol_Border] = *ImVec4_ImVec4Float(0.80f, 0.80f, 0.83f, 0.88f);
   style->Colors[ImGuiCol_BorderShadow] = *ImVec4_ImVec4Float(0.92f, 0.91f, 0.88f, 0.00f);
   style->Colors[ImGuiCol_FrameBg] = *ImVec4_ImVec4Float(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_FrameBgHovered] = *ImVec4_ImVec4Float(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_FrameBgActive] = *ImVec4_ImVec4Float(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_TitleBg] = *ImVec4_ImVec4Float(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_TitleBgCollapsed] = *ImVec4_ImVec4Float(1.00f, 0.98f, 0.95f, 0.75f);
   style->Colors[ImGuiCol_TitleBgActive] = *ImVec4_ImVec4Float(0.07f, 0.07f, 0.09f, 1.00f);
   style->Colors[ImGuiCol_MenuBarBg] = *ImVec4_ImVec4Float(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_ScrollbarBg] = *ImVec4_ImVec4Float(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_ScrollbarGrab] = *ImVec4_ImVec4Float(0.80f, 0.80f, 0.83f, 0.31f);
   style->Colors[ImGuiCol_ScrollbarGrabHovered] = *ImVec4_ImVec4Float(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_ScrollbarGrabActive] = *ImVec4_ImVec4Float(0.06f, 0.05f, 0.07f, 1.00f);
   style->Colors[ImGuiCol_PopupBg] = *ImVec4_ImVec4Float(0.19f, 0.18f, 0.21f, 1.00f);
   style->Colors[ImGuiCol_CheckMark] = *ImVec4_ImVec4Float(0.80f, 0.80f, 0.83f, 0.31f);
   style->Colors[ImGuiCol_SliderGrab] = *ImVec4_ImVec4Float(0.80f, 0.80f, 0.83f, 0.31f);
   style->Colors[ImGuiCol_SliderGrabActive] = *ImVec4_ImVec4Float(0.06f, 0.05f, 0.07f, 1.00f);
   style->Colors[ImGuiCol_Button] = *ImVec4_ImVec4Float(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_ButtonHovered] = *ImVec4_ImVec4Float(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_ButtonActive] = *ImVec4_ImVec4Float(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_Header] = *ImVec4_ImVec4Float(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_HeaderHovered] = *ImVec4_ImVec4Float(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_HeaderActive] = *ImVec4_ImVec4Float(0.06f, 0.05f, 0.07f, 1.00f);
   style->Colors[ImGuiCol_Separator] = *ImVec4_ImVec4Float(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_SeparatorHovered] = *ImVec4_ImVec4Float(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_SeparatorActive] = *ImVec4_ImVec4Float(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_ResizeGrip] = *ImVec4_ImVec4Float(0.00f, 0.00f, 0.00f, 0.00f);
   style->Colors[ImGuiCol_ResizeGripHovered] = *ImVec4_ImVec4Float(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_ResizeGripActive] = *ImVec4_ImVec4Float(0.06f, 0.05f, 0.07f, 1.00f);
   style->Colors[ImGuiCol_PlotLines] = *ImVec4_ImVec4Float(0.40f, 0.39f, 0.38f, 0.63f);
   style->Colors[ImGuiCol_PlotLinesHovered] = *ImVec4_ImVec4Float(0.25f, 1.00f, 0.00f, 1.00f);
   style->Colors[ImGuiCol_PlotHistogram] = *ImVec4_ImVec4Float(0.40f, 0.39f, 0.38f, 0.63f);
   style->Colors[ImGuiCol_PlotHistogramHovered] = *ImVec4_ImVec4Float(0.25f, 1.00f, 0.00f, 1.00f);
   style->Colors[ImGuiCol_TextSelectedBg] = *ImVec4_ImVec4Float(0.25f, 1.00f, 0.00f, 0.43f);
   style->Colors[ImGuiCol_ModalWindowDimBg] = *ImVec4_ImVec4Float(1.00f, 0.98f, 0.95f, 0.73f);
   style->Colors[ImGuiCol_Tab] = *ImVec4_ImVec4Float(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_TabHovered] = *ImVec4_ImVec4Float(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_TabActive] = *ImVec4_ImVec4Float(0.56f, 0.56f, 0.58f, 1.00f);

   clearColor = *ImVec4_ImVec4Float(0.45f, 0.55f, 0.60f, 1.00f);
}

static void tooltip(const char* desc)
{
   igTextDisabled("(?)");
   if (igIsItemHovered(0))
   {
      igBeginTooltip();
      igPushTextWrapPos(igGetFontSize() * 35.0f);
      igTextUnformatted(desc, (const char*)NULL);
      igPopTextWrapPos();
      igEndTooltip();
   }
}

static void imgui_wnd_status()
{
   static float f = 0.0f;
   static int counter = 0;

   igBegin(__("status_window_title"), NULL, 0);

   igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
   igEnd();
}

static void imgui_wnd_settings()
{
   igBegin(__("settings_window_title"), NULL, 0);

   for (unsigned i = 0; i < CAT_LAST; i++)
   {
      settings_t* current = settings_get();
      if(igCollapsingHeaderTreeNodeFlags(category_label(i), ImGuiTreeNodeFlags_None))
      {
         while (current)
         {
            setting_t* s = current->data;
            if (s->categories == i)
            {
               const char* label = __(setting_get_label(s));
               const char* desc = __(setting_get_desc(s));
               switch(s->type)
               {
                  case SETTING_BOOL:
                     igCheckbox(label, s->data);
                     break;
                  case SETTING_STRING:
                     igInputText(label, s->data, sizeof(s->data), ImGuiInputTextFlags_ReadOnly, NULL, NULL);
                     break;
                  case SETTING_INT:
                  case SETTING_UINT:
                  {
                     if (string_is_equal(s->name, "log_level"))
                     {
                        int* curr = s->data;
                        const char* value_label = __(logger_get_level_name(*curr));

                        igSliderInt(label, s->data, s->min, s->max, value_label);
                     }
                     else
                        igSliderInt(label, s->data, s->min, s->max, 0);
                  }
                     break;
                  default:
                     break;
               }
               if (!string_is_empty(desc))
               {
                  igSameLine(0, 0);
                  tooltip(desc);
               }
            }
            current = current->next;
         }
      }
   }

   igEnd();
}

static void imgui_draw_frame()
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

   imgui_wnd_settings();
   imgui_wnd_status();

   /* render */
   igRender();
   SDL_GL_MakeCurrent(window, context);
   glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
   glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
   glClear(GL_COLOR_BUFFER_BIT);
   ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
   SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[])
{
   logger_set_level(LOG_DEBUG);
   init_localization();
   common_config_load();
   if (!create_window(app_name, WINDOW_WIDTH, WINDOW_HEIGHT))
      return -1;
   window = get_window();
   context = get_context();

   glsl_version = get_glsl_version();

   imgui_setup();
   imgui_set_default_style();

   while (!quit)
   {
      imgui_draw_frame();
   }

   imgui_shutdown();
   destroy_window();

   return 0;
}