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

static bool core_running = false;

const char* glsl_version;

const char* core_entries[100];

ImVec4 clearColor;

SDL_Window *window;
SDL_GLContext context;

ImGuiIO io;

GLuint texture;

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
   style->Colors[ImGuiCol_TitleBgCollapsed] = *ImVec4_ImVec4Float(0.70f, 0.70f, 0.70f, 0.70f);
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

/* test code */
size_t render_audio(const int16_t *data, size_t frames)
{
   //SDL_QueueAudio(device, data, 4 * frames);
   return frames;
}

int render_framebuffer(const core_frame_buffer_t *frame_buffer, unsigned pixel_format)
{
   if (!texture)
      glGenTextures(1, &texture);

   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   switch (pixel_format)
   {
      case RETRO_PIXEL_FORMAT_XRGB8888:
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, frame_buffer->width, frame_buffer->height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, frame_buffer->data);
         glPixelStorei(GL_UNPACK_ROW_LENGTH, frame_buffer->pitch / sizeof(uint32_t));
         break;
      case RETRO_PIXEL_FORMAT_RGB565:
         glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
         glPixelStorei(GL_UNPACK_ROW_LENGTH, frame_buffer->pitch / sizeof(uint16_t));
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, frame_buffer->width, frame_buffer->height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, frame_buffer->data);
         break;
      default:
         logger(LOG_DEBUG, tag, "pixel format: %s (%d) unhandled\n", PRINT_PIXFMT(pixel_format), pixel_format);

   }

   return ((int)texture);
}

static void tooltip(const char* desc)
{
   igSameLine(0, 0);
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

static void window_status()
{
   static float f = 0.0f;
   static int counter = 0;

   igBegin(__("window_title_status"), NULL, 0);

   igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
   igEnd();
}

static void window_core()
{
   igBegin(__("window_title_core"), NULL, 0);
   core_run(&frame_buffer, &render_audio);
   render_framebuffer(&frame_buffer, current_core_info.pixel_format);

   ImTextureID image_texture = (void*)(intptr_t)texture;
   igImage(image_texture, *ImVec2_ImVec2Float(640, 480),
                          *ImVec2_ImVec2Float(0, 0),
                          *ImVec2_ImVec2Float(1, 1),
                          *ImVec4_ImVec4Float(1.0f, 1.0f, 1.0f, 1.0f),
                          *ImVec4_ImVec4Float(1.0f, 1.0f, 1.0f, 1.0f));


   igEnd();
}

static void window_core_control()
{
   static int previous_core = -1;
   static const char* current_core_label = current_core_info.core_name;
   static const char* current_core_version = current_core_info.core_version;
   static const char* current_core_extensions = current_core_info.extensions;

   static bool current_core_supports_no_game;
   static bool current_core_block_extract;
   static bool current_core_full_path;

   igBegin(__("window_title_core_control"), NULL, 0);


   igComboStr_arr(__("core_selector_label"), (int*)(&current_core), core_entries,  core_count, 0);
   tooltip(__("core_selector_desc"));

   igLabelText(__("core_current_label"), !string_is_empty(current_core_label) ? current_core_label : __("core_empty_label"));
   tooltip(__("core_current_desc"));

   if (core_count !=0 && (/*!initialized ||*/ previous_core != current_core) || previous_core == -1)
   {
      core_load(core_info_list[current_core].file_name, &current_core_info, core_options, true);

      static bool core_running = false;
      current_core_supports_no_game = current_core_info.supports_no_game;
      previous_core = current_core;
   }

   if (!string_is_empty(current_core_label))
   {
      igLabelText(__("core_current_version_label"), current_core_version);
      tooltip(__("core_current_version_desc"));
      igLabelText(__("core_current_extensions_label"), current_core_extensions);
      tooltip(__("core_current_extensions_desc"));

      if (current_core_supports_no_game && !core_running)
      {
         if(igButton(__("core_current_start_core_label"), *ImVec2_ImVec2Float(0, 0)))
         {
            core_load(core_info_list[current_core].file_name, &current_core_info, core_options, false);
            if (core_load_game(NULL))
               core_running = true;
         }
         tooltip(__("core_current_start_core_desc"));
      }

      if(igCollapsingHeaderTreeNodeFlags(__("core_current_flags_label"), ImGuiTreeNodeFlags_None))
      {
         igCheckbox(__("core_current_supports_no_game_label"), &current_core_supports_no_game);
         tooltip(__("core_current_supports_no_game_desc"));
         igCheckbox(__("core_current_block_extract_label"), &current_core_block_extract);
         tooltip(__("core_current_block_extract_desc"));
         igCheckbox(__("core_current_full_path_label"), &current_core_full_path);
         tooltip(__("core_current_full_path_desc"));
      }
   }

   igEnd();
}

static void window_settings()
{
   igBegin(__("window_title_settings"), NULL, 0);

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
                  tooltip(desc);
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

   window_settings();
   window_status();
   window_core_control();

   if (core_running)
      window_core();

   if (showDemoWindow)
      igShowDemoWindow(&showDemoWindow);

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

   core_list_init(setting_string_val("directory_cores"));

   for (unsigned i = 0; i < core_count; i++)
   {
      core_entries[i] = core_info_list[i].core_name;
      logger(LOG_DEBUG, tag, "loading file %s\n", core_entries[i]);
   }

   while (!quit)
   {
      imgui_draw_frame();
   }

   imgui_shutdown();
   destroy_window();

   return 0;
}