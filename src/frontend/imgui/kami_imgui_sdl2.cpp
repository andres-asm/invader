#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include "kami.h"

static const char* tag = "[kami]";
static const char* app_name = "invader";

static bool quit = false;
static bool showDemoWindow = true;
static bool showAnotherWindow = false;

static bool core_running = false;

const char* core_entries[100];

static char filename[2048] = "";

file_list_t* file_selector_list = NULL;

ImVec4 clearColor;
ImGuiIO io;

GLuint texture;

static void init_localization()
{
   setlocale(LC_ALL, "");
   bindtextdomain("invader", "./intl/");
   textdomain("invader");
}

static void imgui_shutdown()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImGui::DestroyContext(NULL);
}

static void imgui_setup()
{
   ImGui::CreateContext(NULL);
   io = ImGui::GetIO();
   ImGui_ImplSDL2_InitForOpenGL(mywindow, mycontext);
   ImGui_ImplOpenGL3_Init(glsl_version);

   ImGui::StyleColorsDark(NULL);
   io.Fonts->AddFontDefault();
}

/*
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

int kami_render_framebuffer(const core_frame_buffer_t *frame_buffer, unsigned pixel_format)
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
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, frame_buffer->width, frame_buffer->height, 0,
GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, frame_buffer->data); glPixelStorei(GL_UNPACK_ROW_LENGTH,
frame_buffer->pitch / sizeof(uint32_t)); break; case RETRO_PIXEL_FORMAT_RGB565:
         glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
         glPixelStorei(GL_UNPACK_ROW_LENGTH, frame_buffer->pitch / sizeof(uint16_t));
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, frame_buffer->width, frame_buffer->height, 0,
GL_RGB, GL_UNSIGNED_SHORT_5_6_5, frame_buffer->data); break; default: logger(LOG_DEBUG, tag, "pixel
format: %s (%d) unhandled\n", PRINT_PIXFMT(pixel_format), pixel_format);

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

bool igComboStringList(const char* label, int* current_item, struct string_list *list, int
popup_max_height_in_items)
{
   int ret = 0;
   const char **entries = calloc(list->size, sizeof (char *));
   for (unsigned i = 0; i < list->size; i++)
   {
      entries[i] = list->elems[i].data;
   }
   if (igComboStr_arr(label, current_item, entries, list->size, 0))
      return true;
   else
      return false;
}

bool igComboFileList(const char* label, int* current_item, file_list_t *list, int
popup_max_height_in_items)
{
   int ret = 0;
   const char **entries = calloc(list->file_count, sizeof (char *));
   for (unsigned i = 0; i < list->file_count; i++)
   {
      entries[i] = list->file_names[i];
   }
   if (igComboStr_arr(label, current_item, entries, list->file_count, 0))
      return true;
   else
      return false;
}

bool igListBoxFileList(const char* label, int* current_item, file_list_t *list, int
popup_max_height_in_items)
{
   int ret = 0;
   const char **entries = calloc(list->file_count, sizeof (char *));
   for (unsigned i = 0; i < list->file_count; i++)
   {
      entries[i] = list->file_names[i];
   }
   if (igListBoxStr_arr(label, current_item, entries, list->file_count, popup_max_height_in_items))
      return true;
   else
      return false;
}

bool igListBoxStringList(const char* label, int* current_item, struct string_list *list, int
popup_max_height_in_items)
{
   int ret = 0;
   const char **entries = calloc(list->size, sizeof (char *));
   for (unsigned i = 0; i < list->size; i++)
   {
      entries[i] = list->elems[i].data;
   }
   if (igListBoxStr_arr(label, current_item, entries, list->size, 0))
      return true;
   else
      return false;
}

static void window_status()
{
   static float f = 0.0f;
   static int counter = 0;

   igBegin(__("window_title_status"), NULL, 0);

   igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate,
igGetIO()->Framerate); igEnd();
}


static bool window_file_selector(char* output, size_t size, const char* dir, const char* extensions)
{
   static bool ret = false;
   static int index = 0;

   static char cur[2048];
   static char old[2048];

   static bool cancel_result = false;
   static bool select_result = false;

   static file_list_t* list = NULL;

   igBegin(__("window_title_file_selector"), NULL, 0);

   if (!list)
   {
      strlcpy(cur, dir, sizeof(cur));
      logger(LOG_DEBUG, tag, "path: %s\n", cur);
      list = (file_list_t *)calloc(1, sizeof(file_list_t));
      get_file_list(cur, list, "", true);
   }
   else
   {
      if (igListBoxFileList(__(""), &index, list, 10))
      {
         fill_pathname_join(cur, old, list->file_names[index], sizeof(cur));
         if (path_is_directory(cur))
         {
            get_file_list(cur, list, "", true);
            strlcpy(old, cur, sizeof(old));
         }
      }
   }

   cancel_result = igButton(__("button_cancel_label"), *ImVec2_ImVec2Float(0, 0));
   tooltip(__("button_cancel_desc"));

   select_result = igButton(__("button_select_label"), *ImVec2_ImVec2Float(0, 0));
   tooltip(__("button_select_desc"));

   if (cancel_result || select_result)
   {
      list = NULL;
      ret = false;

      if (select_result)
         strlcpy(output, cur, size);
   }
   else
      ret = true;


   igEnd();

   return ret;
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

   static bool file_selector_open;

   igBegin(__("window_title_core_control"), NULL, 0);
   igPushItemWidth(igGetWindowWidth() * 0.40f);

   igComboStr_arr(__("core_selector_label"), (int*)(&current_core), core_entries,  core_count, 0);
   tooltip(__("core_selector_desc"));

   igLabelText(__("core_current_label"), !string_is_empty(current_core_label) ? current_core_label :
__("core_empty_label")); tooltip(__("core_current_desc"));

   if (core_count !=0 && (previous_core != current_core) || previous_core == -1)
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
            core_load(core_info_list[current_core].file_name, &current_core_info, core_options,
false); if (core_load_game(NULL)) core_running = true;
         }
         tooltip(__("core_current_start_core_desc"));
      }

      if(igButton(__("core_current_select_content_label"), *ImVec2_ImVec2Float(0, 0)))
         file_selector_open = true;
      if (file_selector_open)
      {
         if (!window_file_selector(filename, sizeof(filename), "./", NULL))
            file_selector_open = false;
      }
      tooltip(__("core_current_select_content_desc"));

      igInputText(__("core_current_filename_label"), filename, sizeof(filename),
ImGuiInputTextFlags_ReadOnly, NULL, NULL); tooltip(__("core_current_filename_desc"));

      if(!string_is_empty(filename) && filename_supported(filename, current_core_extensions) &&
igButton(__("core_current_load_content_label"), *ImVec2_ImVec2Float(0, 0)))
      {
         core_load(core_info_list[current_core].file_name, &current_core_info, core_options, false);
         if (core_load_game(filename))
            core_running = true;
      }
      tooltip(__("core_current_load_content_desc"));

      /* Core flags
      if(igCollapsingHeaderTreeNodeFlags(__("core_current_flags_label"), ImGuiTreeNodeFlags_None))
      {
         igCheckbox(__("core_current_supports_no_game_label"), &current_core_supports_no_game);
         tooltip(__("core_current_supports_no_game_desc"));
         igCheckbox(__("core_current_block_extract_label"), &current_core_block_extract);
         tooltip(__("core_current_block_extract_desc"));
         igCheckbox(__("core_current_full_path_label"), &current_core_full_path);
         tooltip(__("core_current_full_path_desc"));
      }

      /* Core options V1
      if(core_option_count() > 0 &&
igCollapsingHeaderTreeNodeFlags(__("core_current_options_label"), ImGuiTreeNodeFlags_None))
      {
         for (unsigned i = 0; i < core_option_count(); i++)
         {
            core_option_t* option = &core_options[i];
            char* description = option->description;
            struct string_list* values = kami_core_option_get_values(option);

            int index = kami_core_option_get_index(option, values);

            if (igComboStringList(description, &index, values, 0))
            {
               char* value = values->elems[index].data;
               kami_core_option_update(option, value);
            }
            string_list_free(values);
         }
      }

      /* Core info
      if(core_running && igCollapsingHeaderTreeNodeFlags(__("core_current_info_label"),
ImGuiTreeNodeFlags_None))
      {
         if(igCollapsingHeaderTreeNodeFlags(__("core_current_info_video_label"),
ImGuiTreeNodeFlags_None))
         {
            int base_width = current_core_info.av_info.geometry.base_width;
            int base_height = current_core_info.av_info.geometry.base_height;
            float aspect = current_core_info.av_info.geometry.aspect_ratio;

            igInputInt(__("framebuffer_width_label"), &base_width, 0, 0,
ImGuiInputTextFlags_ReadOnly); tooltip(__("framebuffer_width_desc"));
            igInputInt(__("framebuffer_height_label"), &base_height, 0, 0,
ImGuiInputTextFlags_ReadOnly); tooltip(__("framebuffer_height_desc"));
            igInputFloat(__("framebuffer_aspect_label"), &aspect, 0, 0, "%.3f",
ImGuiInputTextFlags_ReadOnly); tooltip(__("framebuffer_aspect_desc"));
         }


      }

   }

   igEnd();
}

static void window_settings()
{
   igBegin(__("window_title_settings"), NULL, 0);
   igPushItemWidth(igGetWindowWidth() * 0.40f);
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
                     igInputText(label, s->data, sizeof(s->data), ImGuiInputTextFlags_ReadOnly,
NULL, NULL); break; case SETTING_INT: case SETTING_UINT:
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
*/

static void tooltip(const char* desc)
{
   ImGui::SameLine(0, 0);
   ImGui::TextDisabled("(?)");
   if (ImGui::IsItemHovered(0))
   {
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(desc, (const char*)NULL);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
   }
}

static void window_core_control()
{
   static int current_core = 0;
   static int previous_core = -1;

   static const char* current_core_label = current_core_info.core_name;
   static const char* current_core_version = current_core_info.core_version;
   static const char* current_core_extensions = current_core_info.extensions;

   static bool file_selector_open;

   static bool current_core_supports_no_game;
   static bool current_core_block_extract;
   static bool current_core_full_path;

   ImGui::Begin(__("window_title_core_control"), NULL, 0);

   ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.40f);
   ImGui::Combo(__("core_selector_label"), &current_core, core_entries, core_count);

   if (core_count != 0 && (previous_core != current_core) || previous_core == -1)
   {
      current_core_info = core_info_list[current_core];

      delete controller;
      controller = new PiccoloController(&current_core_info, core_options);
      controller->core_peek(current_core_info.file_name);

      static bool core_running = false;
      current_core_supports_no_game = current_core_info.supports_no_game;
      previous_core = current_core;
      controller->core_deinit();
   }

   if (!string_is_empty(current_core_label))
   {
      ImGui::LabelText(__("core_current_version_label"), current_core_version);
      tooltip(__("core_current_version_desc"));
      ImGui::LabelText(__("core_current_extensions_label"), current_core_extensions);
      tooltip(__("core_current_extensions_desc"));

      if (current_core_supports_no_game && !core_running)
      {
         if (ImGui::Button(__("core_current_start_core_label")))
         {
            controller->core_load(current_core_info.file_name);
            // if (core_load_game(NULL))
            //   core_running = true;
         }
         tooltip(__("core_current_start_core_desc"));
      }
   }
   ImGui::End();
}

static void imgui_draw_frame()
{
   SDL_Event e;

   while (SDL_PollEvent(&e) != 0)
   {
      ImGui_ImplSDL2_ProcessEvent(&e);
      if (e.type == SDL_QUIT)
         quit = true;
      if (
         e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE
         && e.window.windowID == SDL_GetWindowID(mywindow))
         quit = true;
   }

   /* start imgui frame */
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplSDL2_NewFrame(mywindow);
   ImGui::NewFrame();

   // window_settings();
   // window_status();
   window_core_control();
   /*
   if (core_running)
      window_core();

   if (showDemoWindow)
      igShowDemoWindow(&showDemoWindow);
   */
   bool demo = true;
   ImGui::ShowDemoWindow(&demo);
   ImGui::Render();
   SDL_GL_MakeCurrent(mywindow, mycontext);
   glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
   glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
   glClear(GL_COLOR_BUFFER_BIT);
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
   SDL_GL_SwapWindow(mywindow);
}

int main(int argc, char* argv[])
{
   logger_set_level(LOG_DEBUG);
   init_localization();

   // common_config_load();
   if (!create_window(app_name, WINDOW_WIDTH, WINDOW_HEIGHT))
      return -1;
   mywindow = get_window();
   mycontext = get_context();

   glsl_version = get_glsl_version();

   imgui_setup();
   // imgui_set_default_style();

   // kami_init_audio();
   // logger(LOG_INFO, tag, "audio driver: %s\n", SDL_GetCurrentAudioDriver());

   kami_core_list_init("./cores");

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