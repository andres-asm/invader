#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include "kami.h"

static const char* tag = "[kami]";
static const char* app_name = "invader";

static bool quit = false;

ImVec4 clearColor;
ImGuiIO io;

Kami* kami;
Kami* kami2;

static bool second_instance = true;

void init_localization()
{
   setlocale(LC_ALL, "");
   bindtextdomain("invader", "./intl/");
   textdomain("invader");
}

void imgui_shutdown()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImGui::DestroyContext(NULL);
}

void imgui_setup()
{
   ImGui::CreateContext(NULL);
   io = ImGui::GetIO();
   ImGui_ImplSDL2_InitForOpenGL(mywindow, mycontext);
   ImGui_ImplOpenGL3_Init(glsl_version);

   ImGui::StyleColorsDark(NULL);
   io.Fonts->AddFontDefault();
}

void tooltip(const char* desc)
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

void set_default_style()
{
   ImGuiStyle* style = &ImGui::GetStyle();

   style->WindowPadding = ImVec2(15, 15);
   style->WindowRounding = 0.0f;
   style->FramePadding = ImVec2(5, 5);
   style->FrameRounding = 0.0f;
   style->ItemSpacing = ImVec2(12, 8);
   style->ItemInnerSpacing = ImVec2(8, 6);
   style->IndentSpacing = 25.0f;
   style->ScrollbarSize = 10.0f;
   style->ScrollbarRounding = 0.0f;
   style->GrabMinSize = 10.0f;
   style->GrabRounding = 0.0f;
   style->TabRounding = 0.0f;

   style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
   style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_Border] = ImVec4(0.16f, 0.15f, 0.17f, 1.00f);
   style->Colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.15f, 0.17f, 1.00f);
   style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
   style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
   style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
   style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
   style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
   style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
   style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
   style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
   style->Colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
   style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
   style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
   style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
   style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
   style->Colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
   style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
   style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
   style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
   style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
   style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
   style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
   style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
   style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
   style->Colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_TabActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);

   clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

bool string_list_combo(const char* label, int* current_item, struct string_list* list, int popup_max_height_in_items)
{
   int ret = 0;
   char** entries = (char**)calloc(list->size, sizeof(char*));
   for (unsigned i = 0; i < list->size; i++)
   {
      entries[i] = list->elems[i].data;
   }
   if (ImGui::Combo(label, current_item, entries, list->size, 0))
      ret = true;
   else
      ret = false;
   free(entries);
   return ret;
}

int Kami::RenderVideo()
{
   unsigned pixel_format = core_info->pixel_format;
   core_frame_buffer_t* video_data = piccolo->get_video_data();

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
         glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB8, video_data->width, video_data->height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
            video_data->data);
         glPixelStorei(GL_UNPACK_ROW_LENGTH, video_data->pitch / sizeof(uint32_t));
         break;
      case RETRO_PIXEL_FORMAT_RGB565:
         glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
         glPixelStorei(GL_UNPACK_ROW_LENGTH, video_data->pitch / sizeof(uint16_t));
         glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB565, video_data->width, video_data->height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
            video_data->data);
         break;
      default:
         logger(LOG_DEBUG, tag, "pixel format: %s (%d) unhandled\n", PRINT_PIXFMT(pixel_format), pixel_format);
   }

   return ((int)texture);
}

void Kami::Main(const char* title)
{
   const char* core_name;
   const char* core_version;
   const char* supported_extensions;

   bool supports_no_game;
   bool block_extract;
   bool full_path;

   unsigned option_count;
   core_option_t* options;

   ImGui::Begin(_(title), NULL, ImGuiWindowFlags_AlwaysAutoResize);
   ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.40f);

   if (core_loaded)
   {
      core_name = core_info->core_name;
      const char* core_version = core_info->core_version;
      const char* supported_extensions = core_info->extensions;

      bool supports_no_game = core_info->supports_no_game;
      bool block_extract = core_info->block_extract;
      bool full_path = core_info->full_path;

      unsigned option_count = piccolo->get_option_count();
      core_option_t* options = piccolo->get_options();
      status = piccolo->get_status();

      switch (status)
      {
         case CORE_STATUS_NONE:
         {
            ImGui::Combo(_("core_selector_label"), &current_core, core_entries, core_count);
            tooltip(_("core_selector_desc"));
            if (previous_core != current_core || previous_core == -1)
            {
               core_info = &core_info_list[current_core];
               piccolo->unload_core();
               core_loaded = piccolo->peek_core(core_info->file_name);
               previous_core = current_core;
            }
            ImGui::LabelText(_("core_current_version_label"), core_version);
            tooltip(_("core_current_version_desc"));
            ImGui::LabelText(_("core_current_extensions_label"), supported_extensions);
            tooltip(_("core_current_extensions_desc"));

            if (supports_no_game)
            {
               if (ImGui::Button(_("core_current_start_core_label")))
               {
                  piccolo->unload_core();
                  piccolo->load_core(core_info->file_name);
                  piccolo->load_game(NULL);
                  core_info = piccolo->get_info();
               }
               tooltip(_("core_current_start_core_desc"));
            }
            if (ImGui::Button(_("core_current_load_content_label")))
            {
               piccolo->unload_core();
               core_info = &core_info_list[current_core];
               piccolo->load_core(core_info->file_name);
               piccolo->load_game("rom.nes");
               core_info = piccolo->get_info();
            }
            tooltip(_("core_current_load_content_desc"));

            break;
         }
         case CORE_STATUS_LOADED:
         case CORE_STATUS_RUNNING:
         {
            piccolo->core_run(NULL);

            int width = core_info->av_info.geometry.base_width;
            int height = core_info->av_info.geometry.base_height;
            float aspect;

            if (core_info->av_info.geometry.aspect_ratio == 0)
               aspect = (float)core_info->av_info.geometry.base_width / core_info->av_info.geometry.base_height;
            else
               aspect = core_info->av_info.geometry.aspect_ratio;

            ImTextureID image_texture = (void*)(intptr_t)this->RenderVideo();
            ImGui::Image(
               image_texture, ImVec2((float)height * 2 * aspect, (float)height * 2), ImVec2(0.0f, 0.0f),
               ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            if (ImGui::CollapsingHeader(_("core_current_info_label"), ImGuiTreeNodeFlags_None))
            {
               ImGui::LabelText(_("core_current_label"), core_name);
               tooltip(_("core_current_desc"));
               ImGui::LabelText(_("core_current_version_label"), core_version);
               tooltip(_("core_current_version_desc"));
               ImGui::LabelText(_("core_current_extensions_label"), supported_extensions);
               tooltip(_("core_current_extensions_desc"));
               if (ImGui::CollapsingHeader(_("core_current_info_video_label"), ImGuiTreeNodeFlags_None))
               {
                  core_frame_buffer_t* video_data = piccolo->get_video_data();
                  int base_width = width;
                  int base_height = height;

                  ImGui::InputInt(_("framebuffer_width_label"), &base_width, 0, 0, ImGuiInputTextFlags_ReadOnly);
                  tooltip(_("framebuffer_width_desc"));
                  ImGui::InputInt(_("framebuffer_height_label"), &base_height, 0, 0, ImGuiInputTextFlags_ReadOnly);
                  tooltip(_("framebuffer_height_desc"));
                  ImGui::InputFloat(_("framebuffer_aspect_label"), &aspect, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
                  tooltip(_("framebuffer_aspect_desc"));
               }
            }
            if (option_count > 0 && ImGui::CollapsingHeader(_("core_current_options_label"), ImGuiTreeNodeFlags_None))
            {
               for (unsigned i = 0; i < option_count; i++)
               {
                  core_option_t* option = &options[i];
                  char* description = option->description;
                  struct string_list* values = OptionGetValues(option);

                  int index = OptionGetIndex(option, values);
                  if (string_list_combo(description, &index, values, 0))
                  {
                     char* value = values->elems[index].data;
                     OptionUpdate(option, value);
                  }
               }
            }
         }
         break;
         default:
            break;
      }

      /* Core flags */
      if (ImGui::CollapsingHeader(_("core_current_flags_label"), ImGuiTreeNodeFlags_None))
      {
         ImGui::Checkbox(_("core_current_supports_no_game_label"), &supports_no_game);
         tooltip(_("core_current_supports_no_game_desc"));
         ImGui::Checkbox(_("core_current_block_extract_label"), &block_extract);
         tooltip(_("core_current_block_extract_desc"));
         ImGui::Checkbox(_("core_current_full_path_label"), &full_path);
         tooltip(_("core_current_full_path_desc"));
      }
   }
   else
   {
      if (core_count > 0)
      {
         ImGui::Combo(_("core_selector_label"), &current_core, core_entries, core_count);

         if (previous_core == -1)
         {
            core_info = &core_info_list[0];

            core_loaded = piccolo->peek_core(core_info->file_name);
            previous_core = current_core;
         }
      }
      else
      {
         /*TODO: add no cores label*/
      }
   }

   ImGui::End();
   return;
}

void imgui_draw_frame()
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

   kami->Main("Core 1");
   kami2->Main("Core 2");

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

   if (!create_window(app_name, WINDOW_WIDTH, WINDOW_HEIGHT))
      return -1;
   mywindow = get_window();
   mycontext = get_context();

   glsl_version = get_glsl_version();

   imgui_setup();
   set_default_style();

   kami = new Kami();
   kami->CoreListInit("./cores");

   if (second_instance)
   {
      kami2 = new Kami();
      kami2->CoreListInit("./cores");
   }

   while (!quit)
   {
      imgui_draw_frame();
   }

   logger(LOG_DEBUG, tag, "shutting down\n");
   delete kami;

   imgui_shutdown();
   destroy_window();

   return 0;
}