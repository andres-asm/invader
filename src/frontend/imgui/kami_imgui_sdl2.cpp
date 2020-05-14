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

static bool quit = false;

static bool file_manager_dialog_is_open = false;

ImVec4 clearColor;
ImGuiIO io;

Kami* kami1;
Kami* kami2;

GLuint kami1_output;
GLuint kami2_output;

static bool second_instance = false;

std::vector<Asset> device_gamepad_inputs;
Asset asset;

const char* device_gamepad_asset_names[] = {"base.png", "b.png",    "y.png",     "select.png", "start.png", "up.png",
                                            "down.png", "left.png", "right.png", "a.png",      "x.png",     "l.png",
                                            "r.png",    "l2.png",   "r2.png",    "l3.png",     "r3.png"};

/*helpers*/
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

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

/*TODO: move outside of this file*/
/*tooltip widget*/
void tooltip(const char* desc)
{
   ImGui::SameLine(0, 0);
   ImGui::TextDisabled(" (?)");
   if (ImGui::IsItemHovered(0))
   {
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(desc, (const char*)NULL);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
   }
}

/*TODO: move outside of this file*/
/*file list widget*/
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

/*TODO: move outside of this file*/
/*string list combo widget*/
bool string_list_combo(const char* label, int* current_item, struct string_list* list, int popup_max_height_in_items)
{
   bool ret = false;
   char** entries = (char**)calloc(list->size, sizeof(char*));
   for (unsigned i = 0; i < list->size; i++)
   {
      entries[i] = list->elems[i].data;
   }
   if (ImGui::Combo(label, current_item, entries, list->size, popup_max_height_in_items))
      ret = true;
   else
      ret = false;
   free(entries);
   return ret;
}

/*TODO: move outside of this file*/
/*controller entries widget*/
bool controller_combo(
   const char* label, int* current_item, const controller_description_t* list, size_t size,
   int popup_max_height_in_items)
{
   bool ret = false;
   char** entries = (char**)calloc(size, sizeof(char*));
   for (unsigned i = 0; i < size; i++)
      entries[i] = (char*)list[i].desc;

   if (ImGui::Combo(label, current_item, entries, size, popup_max_height_in_items))
      ret = true;
   else
      ret = false;

   return ret;
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
   style->ScrollbarSize = 1.0f;
   style->ScrollbarRounding = 0.0f;
   style->GrabMinSize = 10.0f;
   style->GrabRounding = 0.0f;
   style->TabRounding = 0.0f;

   ImVec4 widgets_idle = ImVec4(0.10f, 0.10f, 0.18f, 1.00f);
   ImVec4 widgets_hovered = ImVec4(0.20f, 0.20f, 0.28f, 1.00f);
   ImVec4 widgets_active = ImVec4(0.50f, 0.50f, 0.58f, 1.00f);

   ImVec4 highlights = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);

   ImVec4 grabs_idle = ImVec4(0.80f, 0.80f, 0.80f, 0.50f);
   ImVec4 grabs_hovered = ImVec4(0.80f, 0.80f, 0.80f, 0.50f);
   ImVec4 grabs_active = ImVec4(0.80f, 0.80f, 0.80f, 0.50f);

   style->Colors[ImGuiCol_Text] = highlights;
   style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
   style->Colors[ImGuiCol_Border] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

   style->Colors[ImGuiCol_WindowBg] = ImVec4(0.30f, 0.30f, 0.38f, 1.00f);
   style->Colors[ImGuiCol_ChildBg] = ImVec4(0.30f, 0.30f, 0.38f, 1.00f);
   style->Colors[ImGuiCol_PopupBg] = ImVec4(0.30f, 0.30f, 0.38f, 1.00f);
   style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);

   style->Colors[ImGuiCol_Border] = highlights;
   style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);

   style->Colors[ImGuiCol_FrameBg] = widgets_hovered;
   style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);

   style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
   style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);

   style->Colors[ImGuiCol_ScrollbarBg] = widgets_idle;
   style->Colors[ImGuiCol_ScrollbarGrab] = grabs_idle;
   style->Colors[ImGuiCol_ScrollbarGrabHovered] = grabs_hovered;
   style->Colors[ImGuiCol_ScrollbarGrabActive] = grabs_active;
   style->Colors[ImGuiCol_CheckMark] = highlights;
   style->Colors[ImGuiCol_SliderGrab] = grabs_idle;
   style->Colors[ImGuiCol_SliderGrabActive] = grabs_active;
   style->Colors[ImGuiCol_Button] = widgets_idle;
   style->Colors[ImGuiCol_ButtonHovered] = widgets_hovered;
   style->Colors[ImGuiCol_ButtonActive] = widgets_active;
   style->Colors[ImGuiCol_Header] = widgets_idle;
   style->Colors[ImGuiCol_HeaderHovered] = widgets_hovered;
   style->Colors[ImGuiCol_HeaderActive] = widgets_active;
   style->Colors[ImGuiCol_Separator] = widgets_idle;
   style->Colors[ImGuiCol_SeparatorHovered] = widgets_hovered;
   style->Colors[ImGuiCol_SeparatorActive] = widgets_active;
   style->Colors[ImGuiCol_ResizeGrip] = widgets_idle;
   style->Colors[ImGuiCol_ResizeGripHovered] = widgets_hovered;
   style->Colors[ImGuiCol_ResizeGripActive] = widgets_active;
   style->Colors[ImGuiCol_Tab] = widgets_idle;
   style->Colors[ImGuiCol_TabHovered] = widgets_hovered;
   style->Colors[ImGuiCol_TabActive] = widgets_active;

   style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
   style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
   style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
   style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
   style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
   style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.70f, 0.70f, 0.78f, 0.70f);

   clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

void Asset::Load(const char* filename)
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

void Asset::Render(unsigned width, unsigned height)
{
   ImGui::Image((void*)(intptr_t)data, ImVec2(width, height));
}

void RenderInputDeviceStatus(Kami* kami, unsigned port, unsigned width, unsigned height)
{
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   Asset asset;
   GLuint base, result;
   asset = device_gamepad_inputs.at(0);
   base = asset.get_texture();

   ImVec2 p = ImGui::GetCursorScreenPos();
   ImGui::Image((void*)(intptr_t)base, ImVec2(width, height));

   input_state_t state;
   state = kami->GetInputState(port);
   for (unsigned i = 0; i < GAMEPAD_LAST; i++)
   {
      if (true)
      {
         asset = device_gamepad_inputs.at(i + 1);
         result = asset.get_texture();
         if ((state.buttons & (1 << i)) >> i)
            ImGui::GetWindowDrawList()->AddImage(
               (void*)(intptr_t)result, p, ImVec2(p.x + width, p.y + height), ImVec2(0, 0), ImVec2(1, 1));
      }
   }
}

void Kami::TextureListInit(const char* path)
{
   char filename[PATH_MAX_LENGTH];

   for (unsigned i = 0; i <= GAMEPAD_LAST; i++)
   {
      fill_pathname_join(filename, asset_dir, device_gamepad_asset_names[i], sizeof(filename));
      Asset* asset = new Asset();
      asset->Load(filename);
      device_gamepad_inputs.push_back(*asset);
   }
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

void Kami::InputPoll()
{ }

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

   static int padding = ImGui::GetStyle().WindowPadding.x;

   ImGui::SetNextWindowSizeConstraints(ImVec2(640 + padding * 2, 100), ImVec2(640 + padding * 2, 800));

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

      size_t option_count = piccolo->get_option_count();
      core_option_t* options = piccolo->get_options();
      status = piccolo->get_status();

      size_t controller_port_count = piccolo->get_controller_port_count();
      controller_info_t* controllers = piccolo->get_controller_info();

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
                  piccolo->set_callbacks(InputPoll);
                  piccolo->load_game(core_info->file_name, NULL, frontend_supports_bitmasks);
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
               piccolo->set_callbacks(InputPoll);
               piccolo->load_game(core_info->file_name, content_file_name, frontend_supports_bitmasks);
               core_info = piccolo->get_info();
            }
#ifdef DEBUG
            /* frontend flags */
            if (ImGui::CollapsingHeader(_("frontend_flags_label"), ImGuiTreeNodeFlags_None))
            {
               ImGui::Checkbox(_("frontend_supports_bitmasks_label"), &frontend_supports_bitmasks);
               tooltip(_("frontend_supports_bitmasks_desc"));
            }
#endif
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

            /*TODO: remove this
             *auto draw_list = ImGui::GetBackgroundDrawList();
             *ImVec2 p = ImVec2(0.0f, 0.0f);
             *draw_list->AddImage(
             *   (void*)(intptr_t)image_texture, p, ImVec2(p.x + height * aspect, p.y + height), ImVec2(0,
             *   0), ImVec2(1, 1));
             */
            if (ImGui::CollapsingHeader(_("core_current_video_output_label"), ImGuiTreeNodeFlags_None))
            {
               ImGui::Image(
                  image_texture, ImVec2((float)640, (float)640 / aspect), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),
                  ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
            if (ImGui::CollapsingHeader(_("core_current_actions_label"), ImGuiTreeNodeFlags_None))
            {
               if (ImGui::Button(_("core_current_reset_core_label"), ImVec2(240, 0)))
                  piccolo->core_reset();
               tooltip(_("core_current_reset_core_desc"));
            }
            if (ImGui::CollapsingHeader(_("core_current_input_label"), ImGuiTreeNodeFlags_None))
            {
               /*TODO: remove this, asset rendering example
               device_gamepad_inputs[0].Render(width, height);
               device_gamepad_inputs[1].Render(width, height);
               */
               ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

               for (unsigned i = 0; i < controller_port_count; i++)
               {
                  char buf[100];
                  snprintf(buf, sizeof(buf), "%s %d\n", _("core_current_port_label"), i + 1);
                  if (ImGui::CollapsingHeader(buf, ImGuiTreeNodeFlags_None))
                  {
                     ImGui::Columns(2, "", false);
                     unsigned width = device_gamepad_inputs[0].get_width() / 2;
                     unsigned height = width / device_gamepad_inputs[0].get_aspect();
                     ImGui::SetColumnWidth(-1, width + ImGui::GetTreeNodeToLabelSpacing());
                     RenderInputDeviceStatus(this, 0, width, height);
                     ImGui::NextColumn();

                     /*TODO: get the actual current device from the core at init and make sure to get the one from
                      * setting once settings are implemented, also change 16 to a define somewhere*/
                     static int current_device[MAX_PORTS];

                     if (controller_combo(
                            _("core_current_port_current_device_label"), &current_device[i], controllers[i].types,
                            controllers[i].num_types, controller_port_count))
                     {
                        const unsigned index = current_device[i];
                        const unsigned idx = controllers[i].types[index].id;
                        const char* desc = controllers[i].types[index].desc;

                        logger(LOG_DEBUG, tag, "changing port to: %d (%s)\n", idx, desc);
                        ParseInputDescriptors();
                        ControllerPortUpdate(i, idx);
                     }
                     tooltip(_("core_current_port_current_device_desc"));

                     for (unsigned j = 0; j < MAX_IDS; j++)
                     {
                        if (!string_is_empty(input_descriptors[i][j].description))
                        {
                           ImGui::PushButtonRepeat(true);
                           input_state[i].buttons &= ~(1 << j);
                           if (ImGui::Button(input_descriptors[i][j].description, ImVec2(240, 0)))
                              input_state[i].buttons |= 1 << 1 * j;
                           ImGui::PopButtonRepeat();
                        }
                     }
                     ImGui::Columns(1);
                     piccolo->set_input_state(i, input_state[i]);

                     ImGui::Columns(1);
                  }
               }
               ImGui::Unindent();
            }
            if (ImGui::CollapsingHeader(_("core_current_info_label"), ImGuiTreeNodeFlags_None))
            {
               ImGui::LabelText(_("core_current_label"), core_name);
               tooltip(_("core_current_desc"));
               ImGui::LabelText(_("core_current_version_label"), core_version);
               tooltip(_("core_current_version_desc"));
               ImGui::LabelText(_("core_current_extensions_label"), supported_extensions);
               tooltip(_("core_current_extensions_desc"));

               ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
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
               ImGui::Unindent();
            }
            if (option_count > 0 && ImGui::CollapsingHeader(_("core_current_options_label"), ImGuiTreeNodeFlags_None))
            {
               ImGuiWindowFlags window_flags = 0;
               ImGui::BeginChild(
                  "ChildL", ImVec2(ImGui::GetWindowContentRegionWidth() * 1.0f, 500), false, window_flags);
               for (unsigned i = 0; i < option_count; i++)
               {
                  core_option_t* option = &options[i];
                  char* description = option->description;
                  struct string_list* values = OptionGetValues(option);

                  int index = OptionGetIndex(option, values);
                  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.30f);
                  if (string_list_combo(description, &index, values, 0))
                  {
                     char* value = values->elems[index].data;
                     OptionUpdate(option, value);
                  }
                  ImGui::PopItemWidth();
               }
               ImGui::EndChild();
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

void file_manager(const char* dir_left, const char* dir_right)
{
   static int padding = ImGui::GetStyle().WindowPadding.x;
   ImGui::SetNextWindowSizeConstraints(ImVec2(800 + padding * 2, 100), ImVec2(800 + padding * 2, FLT_MAX));

   if (file_manager_dialog_is_open)
   {
      if (ImGui::BeginPopupModal(_("window_title_file_manager"), NULL, ImGuiWindowFlags_AlwaysAutoResize))
      {
         static file_list_t* list_left = NULL;
         static file_list_t* list_right = NULL;
         static int index_left = 0;
         static int index_right = 0;

         static char cur_left[PATH_MAX_LENGTH];
         static char old_left[PATH_MAX_LENGTH];
         static char cur_right[PATH_MAX_LENGTH];
         static char old_right[PATH_MAX_LENGTH];

         ImGui::Columns(2, "", false);
         if (!list_left)
         {
            strlcpy(cur_left, dir_left, sizeof(cur_left));
            logger(LOG_DEBUG, tag, "path: %s\n", cur_left);
            list_left = (file_list_t*)calloc(1, sizeof(file_list_t));
            get_file_list(cur_left, list_left, "", true);
         }
         else
         {
            ImGui::PushID("left");
            if (file_list("", &index_left, list_left, 10))
            {
               fill_pathname_join(cur_left, old_left, list_left->file_names[index_left], sizeof(cur_left));
               if (path_is_directory(cur_left))
               {
                  index_left = 0;
                  get_file_list(cur_left, list_left, "", true);
                  strlcpy(old_left, cur_left, sizeof(old_left));
               }
            }
            ImGui::PopID();
         }
         ImGui::NextColumn();
         if (!list_right)
         {
            strlcpy(cur_right, dir_right, sizeof(cur_right));
            logger(LOG_DEBUG, tag, "path: %s\n", cur_right);
            list_right = (file_list_t*)calloc(1, sizeof(file_list_t));
            get_file_list(cur_right, list_right, "", true);
         }
         else
         {
            ImGui::PushID("right");
            if (file_list("", &index_right, list_right, 10))
            {
               fill_pathname_join(cur_right, old_right, list_right->file_names[index_right], sizeof(cur_right));
               if (path_is_directory(cur_right))
               {
                  index_right = 0;
                  get_file_list(cur_right, list_right, "", true);
                  strlcpy(old_right, cur_right, sizeof(old_right));
               }
            }
            ImGui::PopID();
         }
         ImGui::Columns(1);
         ImGui::Separator();
         /*
         static bool dont_ask_me_next_time = false;
         ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
         ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
         ImGui::PopStyleVar();
         */
         /*if (ImGui::Button(_("button_select_label"), ImVec2(240, 0)))
         {
            file_open_dialog_result_ok = true;
            file_open_dialog_is_open = false;
            strlcpy(content_file_name, cur, sizeof(content_file_name));
            ImGui::CloseCurrentPopup();
         }*/
         ImGui::SetItemDefaultFocus();
         ImGui::SameLine();
         /*if (ImGui::Button("Cancel", ImVec2(240, 0)))
         {
            file_open_dialog_result_ok = false;
            file_open_dialog_is_open = false;
            ImGui::CloseCurrentPopup();
         }*/
         ImGui::EndPopup();
      }
   }
}

void invader()
{
   ImGui::Begin(_("window_title_invader"), NULL, ImGuiWindowFlags_AlwaysAutoResize);
   if (ImGui::Button(_("invader_file_manager"), ImVec2(120, 0)))
   {
      ImGui::OpenPopup(_("window_title_file_manager"));
      file_manager_dialog_is_open = true;
   }
   file_manager(".", ".");
   ImGui::End();
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
   ImGui_ImplSDL2_InitForOpenGL(invader_window, invader_context);
   ImGui_ImplOpenGL3_Init(glsl_version);

   ImGui::StyleColorsDark(NULL);
   io.Fonts->AddFontDefault();
}

const char* vertex_shader_source =
   "#version 330 core\n"
   "layout (location = 0) in vec3 aPos;\n"
   "layout (location = 1) in vec3 aColor;\n"
   "layout (location = 2) in vec2 aTexCoord;\n"
   "out vec3 ourColor;\n"
   "out vec2 TexCoord;\n"
   "void main()\n"
   "{\n"
   "    gl_Position = vec4(aPos, 1.0);\n"
   "    ourColor = aColor;\n"
   "    TexCoord = aTexCoord;\n"
   "}\n";

const char* fragment_shader_source =
   "#version 330 core\n"
   "out vec4 FragColor;\n"
   "in vec3 ourColor;\n"
   "in vec2 TexCoord;\n"
   "uniform sampler2D ourTexture;\n"
   "void main()\n"
   "{\n"
   "    FragColor = texture(ourTexture, TexCoord);\n"
   "}\n";

void framebuffer_setup()
{
   int success;
   char infoLog[512];

   /*vertex shader*/
   int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
   glCompileShader(vertex_shader);

   glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
   if (!success)
   {
      glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
      logger(LOG_DEBUG, tag, "vertex shader compilation error: %s\n", infoLog);
   }

   /*fragment shader*/
   int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
   glCompileShader(fragment_shader);

   glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
   if (!success)
   {
      glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
      logger(LOG_DEBUG, tag, "fragment shader compilation error: %s\n", infoLog);
   }

   /*link shaders*/
   int shader_program = glCreateProgram();
   glAttachShader(shader_program, vertex_shader);
   glAttachShader(shader_program, fragment_shader);
   glLinkProgram(shader_program);

   glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
   if (!success)
   {
      glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
      logger(LOG_DEBUG, tag, "shader program linking error: %s\n", infoLog);
   }
   glDeleteShader(vertex_shader);
   glDeleteShader(fragment_shader);

   float vertices[] = {1.0f, -1.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                       0.0f, 1.0f,  0.0f,  1.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                       0.0f, 0.0f,  -1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f};
   unsigned int indices[] = {0, 1, 3, 1, 2, 3};

   unsigned int vbo, vao, ebo;
   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &ebo);

   glBindVertexArray(vao);

   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   /*position attributes*/
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   /*color attributes*/
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   /*texture coordinate attributes*/
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, kami1->get_texture());

   glUseProgram(shader_program);
   glBindVertexArray(vao);
}

void framebuffer_render()
{
   /*TODO: setup viewport*/
   if (kami1->get_texture())
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
         && e.window.windowID == SDL_GetWindowID(invader_window))
         quit = true;
   }

   /* start imgui frame */
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplSDL2_NewFrame(invader_window);

   ImGui::NewFrame();

   // invader();

   kami1->Main("Core 1");
   if (second_instance)
      kami2->Main("Core 2");

   /*TODO: remove this*/
   bool demo = true;
   ImGui::ShowDemoWindow(&demo);

   ImGui::Render();
   SDL_GL_MakeCurrent(invader_window, invader_context);
   glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
   glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
   glClear(GL_COLOR_BUFFER_BIT);
   framebuffer_render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
   SDL_GL_SwapWindow(invader_window);
}

int main(int argc, char* argv[])
{
   logger_set_level(LOG_DEBUG);
   init_localization();

   if (!create_window(app_name, WINDOW_WIDTH, WINDOW_HEIGHT))
      return -1;
   invader_window = get_window();
   invader_context = get_context();

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

   framebuffer_setup();

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