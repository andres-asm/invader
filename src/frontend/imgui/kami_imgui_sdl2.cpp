#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include <file/file_path.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "kami.h"

static const char* tag = "[kami]";
static const char* app_name = "invader";

static const char* asset_dir = "./assets/gamepad/generic";
static const char* gamepad_texture = "base.png";

static bool quit = false;

ImVec4 clearColor;
ImGuiIO io;

Kami* kami1;
Kami* kami2;

GLuint kami1_output;
GLuint kami2_output;

static bool second_instance = true;

const char* gamepad_asset_names[] = {
   "base.png",
   "b.png",
   "a.png",
};

bool load_texture(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
   int image_width = 0;
   int image_height = 0;
   unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
   if (image_data == NULL)
      return false;

   GLuint image_texture;
   glGenTextures(1, &image_texture);
   glBindTexture(GL_TEXTURE_2D, image_texture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
   stbi_image_free(image_data);

   *out_texture = image_texture;
   *out_width = image_width;
   *out_height = image_height;

   return true;
}

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

void KamiAsset::Load(const char* filename)
{
   width = 0;
   height = 0;
   aspect = 0;

   logger(LOG_DEBUG, tag, "filename %s\n", filename);
   unsigned char* image_data = stbi_load(filename, &width, &height, NULL, 4);
   if (image_data == NULL)
      return;

   aspect = (float)width / height;

   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
   stbi_image_free(image_data);

   data = texture;
}

void KamiAsset::Render()
{
   int widget_width = ImGui::GetWindowWidth() * 0.25f;
   int widget_height = widget_width / aspect;
   ImGui::Image((void*)(intptr_t)data, ImVec2(widget_width, widget_height));
}

void Kami::blend_test()
{
   KamiAsset asset;
   GLuint data1, data2;
   asset = gamepad_assets.at(0);
   data1 = asset.get_texture();

   int widget_width = asset.get_width();
   int widget_height = asset.get_height();

   ImVec2 p = ImGui::GetCursorScreenPos();
   ImGui::Image((void*)(intptr_t)data1, ImVec2(widget_width, widget_height));

   for (unsigned i = 0; i < GAMEPAD_LAST; i++)
   {
      if (true)
      {
         asset = gamepad_assets.at(i);
         data2 = asset.get_texture();
         ImGui::GetWindowDrawList()->AddImage(
            (void*)(intptr_t)data2, p, ImVec2(p.x + widget_width, p.y + widget_height), ImVec2(0, 0), ImVec2(1, 1));
      }
   }
}

void Kami::TextureListInit(const char* path)
{
   char filename[PATH_MAX_LENGTH];

   for (unsigned i = 0; i < GAMEPAD_LAST; i++)
   {
      fill_pathname_join(filename, asset_dir, gamepad_asset_names[i], sizeof(filename));
      KamiAsset* asset = new KamiAsset();
      asset->Load(filename);
      gamepad_assets.push_back(*asset);
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

bool file_list(const char* label, int* current_item, file_list_t* list, int popup_max_height_in_items)
{
   bool ret = false;
   char** entries = (char**)calloc(list->file_count, sizeof(char*));
   for (unsigned i = 0; i < list->file_count; i++)
   {
      entries[i] = list->file_names[i];
   }

   ImGui::PushItemWidth(-FLT_MIN);
   if (ImGui::ListBox(label, current_item, entries, list->file_count, popup_max_height_in_items))
      ret = true;
   else
      ret = false;
   ImGui::PopItemWidth();

   free(entries);
   return ret;
}

bool string_list_combo(const char* label, int* current_item, struct string_list* list, int popup_max_height_in_items)
{
   bool ret = false;
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
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   switch (pixel_format)
   {
      case RETRO_PIXEL_FORMAT_XRGB8888:
         glPixelStorei(GL_UNPACK_ROW_LENGTH, video_data->pitch / sizeof(uint32_t));
         glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB8, video_data->width, video_data->height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
            video_data->data);
         break;
      case RETRO_PIXEL_FORMAT_RGB565:
         glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
         glPixelStorei(GL_UNPACK_ROW_LENGTH, video_data->pitch / sizeof(uint16_t));
         glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB565, video_data->width, video_data->height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
            video_data->data);
         break;
      case RETRO_PIXEL_FORMAT_0RGB1555:
         glPixelStorei(GL_UNPACK_ROW_LENGTH, video_data->pitch / sizeof(uint16_t));
         glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB8, video_data->width, video_data->height, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV,
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
               if (ImGui::Button(_("core_current_start_core_label"), ImVec2(120, 0)))
               {
                  piccolo->unload_core();
                  piccolo->load_core(core_info->file_name);
                  piccolo->load_game(NULL);
                  core_info = piccolo->get_info();
               }
               tooltip(_("core_current_start_core_desc"));
            }
            if (
               !(string_is_equal(supported_extensions, "N/A"))
               && ImGui::Button(_("core_current_load_content_label"), ImVec2(120, 0)))
            {
               if (!file_open_dialog_is_open)
               {
                  ImGui::OpenPopup(_("window_title_file_selector"));
                  file_open_dialog_is_open = true;
               }
            }
            tooltip(_("core_current_load_content_desc"));
            if (!file_open_dialog_is_open && file_open_dialog_result_ok)
            {
               piccolo->unload_core();
               core_info = &core_info_list[current_core];
               piccolo->load_core(core_info->file_name);
               piccolo->load_game(content_file_name);
               core_info = piccolo->get_info();
            }
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
            if (ImGui::CollapsingHeader(_("core_current_input_label"), ImGuiTreeNodeFlags_None))
            {
               /*ImGui::Columns(2, "", false);
               gamepad_assets[0].Render();
               ImGui::NextColumn();
               gamepad_assets[1].Render();*/
               blend_test();
               ImGui::Columns(1);
            }
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

   if (file_open_dialog_is_open)
   {
      if (ImGui::BeginPopupModal(_("window_title_file_selector"), NULL, ImGuiWindowFlags_AlwaysAutoResize))
      {
         static file_list_t* list = NULL;
         static int index = 0;

         static char cur[PATH_MAX_LENGTH];
         static char old[PATH_MAX_LENGTH];

         const char* dir = "./";

         ImGui::Text(_("file_selector_label"));

         if (!list)
         {
            strlcpy(cur, dir, sizeof(cur));
            logger(LOG_DEBUG, tag, "path: %s\n", cur);
            list = (file_list_t*)calloc(1, sizeof(file_list_t));
            get_file_list(cur, list, "", true);
         }
         else
         {
            if (file_list("", &index, list, 10))
            {
               fill_pathname_join(cur, old, list->file_names[index], sizeof(cur));
               if (path_is_directory(cur))
               {
                  get_file_list(cur, list, "", true);
                  strlcpy(old, cur, sizeof(old));
               }
            }
         }
         ImGui::Separator();
         /*
                  static bool dont_ask_me_next_time = false;
                  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                  ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
                  ImGui::PopStyleVar();
         */
         if (ImGui::Button(_("button_select_label"), ImVec2(240, 0)))
         {
            file_open_dialog_result_ok = true;
            file_open_dialog_is_open = false;
            strlcpy(content_file_name, cur, sizeof(content_file_name));
            ImGui::CloseCurrentPopup();
         }
         ImGui::SetItemDefaultFocus();
         ImGui::SameLine();
         if (ImGui::Button("Cancel", ImVec2(240, 0)))
         {
            file_open_dialog_result_ok = false;
            file_open_dialog_is_open = false;
            ImGui::CloseCurrentPopup();
         }
         ImGui::EndPopup();
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

   kami1->Main("Core 1");
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

   kami1 = new Kami();
   kami1->CoreListInit("./cores");
   kami1->TextureListInit(asset_dir);

   if (second_instance)
   {
      kami2 = new Kami();
      kami2->CoreListInit("./cores");
      kami2->TextureListInit(asset_dir);
   }

   while (!quit)
   {
      imgui_draw_frame();
   }

   logger(LOG_DEBUG, tag, "shutting down\n");
   delete kami1;
   if (second_instance)
      delete kami2;

   imgui_shutdown();
   destroy_window();

   return 0;
}