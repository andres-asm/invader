// imgui
#include "imgui.h"

#include "common.h"
#include "kami.h"
#include "widgets.h"

static const char* tag = "[main]";

std::vector<Asset> gamepad_assets;

// TODO: render the actual per-button gamepad overlay once Vulkan texture
// upload (Asset::Load / ImGui_ImplVulkan_AddTexture) is wired up.
void RenderBackendInputState(Kami* kami, unsigned port, unsigned width, unsigned height)
{
   UNUSED(kami);
   UNUSED(port);
   ImGui::Dummy(ImVec2(width, height));
   ImGui::TextDisabled(_("gamepad overlay not yet implemented"));
}

// TODO: load the gamepad overlay images via Asset::Load once Vulkan texture
// upload is implemented; for now just reserve placeholder entries so
// RenderGui's layout math (which indexes gamepad_assets[0]) has something
// to read.
void texture_list_init(const char* path)
{
   UNUSED(path);

   for (unsigned i = 0; i <= GAMEPAD_LAST; i++)
   {
      Asset asset;
      asset.Load(NULL);
      gamepad_assets.push_back(asset);
   }
}

unsigned Kami::RenderVideo(unsigned* output)
{
   // TODO: upload core_frame_buffer_t to a VkImage and register it with
   // ImGui_ImplVulkan_AddTexture, mirroring kami_imgui_opengl3.cpp's
   // glTexImage2D-based RenderVideo.
   return *output;
}

void Kami::InputPoll()
{ }

void Kami::Main()
{
   if (core_loaded)
   {
      status = piccolo->get_status();

      if (status == CORE_STATUS_LOADED || status == CORE_STATUS_RUNNING)
      {
         piccolo->core_run(NULL);
         RenderVideo(&texture_data);
      }
   }
}

void Kami::RenderGui(const char* title)
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

   ImGui::SetNextWindowSizeConstraints(ImVec2(640 + padding * 2, 100), ImVec2(640 + padding * 2, 900));

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

      size_t controller_port_count = piccolo->get_controller_port_count();
      controller_info_t* controllers = piccolo->get_controller_info();

      switch (status)
      {
         case CORE_STATUS_NONE:
         {
            ImGui::Combo(_("core_selector_label"), &current_core, core_entries, core_count);
            Widgets::Tooltip(_("core_selector_desc"));
            if (previous_core != current_core || previous_core == -1)
            {
               core_info = &core_info_list[current_core];
               piccolo->unload_core();
               core_loaded = piccolo->peek_core(core_info->file_name);
               previous_core = current_core;
            }
            ImGui::LabelText(_("core_current_version_label"), core_version);
            Widgets::Tooltip(_("core_current_version_desc"));
            ImGui::LabelText(_("core_current_extensions_label"), supported_extensions);
            Widgets::Tooltip(_("core_current_extensions_desc"));

            if (supports_no_game)
            {
               if (ImGui::Button(_("core_current_start_core_label"), ImVec2(120, 0)))
               {
                  piccolo->unload_core();
                  piccolo->set_callbacks(InputPoll);
                  piccolo->load_game(core_info->file_name, NULL, frontend_supports_bitmasks);
                  core_info = piccolo->get_info();
               }
               Widgets::Tooltip(_("core_current_start_core_desc"));
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
            Widgets::Tooltip(_("core_current_load_content_desc"));
            if (!file_open_dialog_is_open && file_open_dialog_result_ok)
            {
               piccolo->unload_core();
               core_info = &core_info_list[current_core];
               piccolo->set_callbacks(InputPoll);
               piccolo->load_game(core_info->file_name, content_file_name, frontend_supports_bitmasks);
               core_info = piccolo->get_info();
            }
#ifdef DEBUG
            // frontend flags
            if (ImGui::CollapsingHeader(_("frontend_flags_label"), ImGuiTreeNodeFlags_None))
            {
               ImGui::Checkbox(_("frontend_supports_bitmasks_label"), &frontend_supports_bitmasks);
               Widgets::Tooltip(_("frontend_supports_bitmasks_desc"));
            }
#endif
            break;
         }
         case CORE_STATUS_LOADED:
         case CORE_STATUS_RUNNING:
         {
            int width = core_info->av_info.geometry.base_width;
            int height = core_info->av_info.geometry.base_height;
            float aspect;

            if (core_info->av_info.geometry.aspect_ratio == 0)
               aspect = (float)core_info->av_info.geometry.base_width / core_info->av_info.geometry.base_height;
            else
               aspect = core_info->av_info.geometry.aspect_ratio;

            ImTextureID image_texture = (ImTextureID)GetTextureData();

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
               Widgets::Tooltip(_("core_current_reset_core_desc"));
            }
            if (ImGui::CollapsingHeader(_("core_current_input_label"), ImGuiTreeNodeFlags_None))
            {
               ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

               for (unsigned i = 0; i < controller_port_count; i++)
               {
                  char buf[100];
                  snprintf(buf, sizeof(buf), "%s %d\n", _("core_current_port_label"), i + 1);
                  if (ImGui::CollapsingHeader(buf, ImGuiTreeNodeFlags_None))
                  {
                     ImGui::Columns(2, "", false);
                     unsigned width = gamepad_assets[0].get_width() / 2;
                     unsigned height = width / gamepad_assets[0].get_aspect();
                     ImGui::SetColumnWidth(-1, width + ImGui::GetTreeNodeToLabelSpacing());
                     RenderBackendInputState(this, 0, width, height);
                     ImGui::NextColumn();

                     // TODO: get the actual current device from the core at init and make sure to get the one from
                     // setting once settings are implemented
                     static int current_device[MAX_PORTS];

                     if (Widgets::ControllerTypesCombo(
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
                     Widgets::Tooltip(_("core_current_port_current_device_desc"));

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
               ImGuiWindowFlags window_flags = 0;
               ImGui::BeginChild("info", ImVec2(ImGui::GetContentRegionAvail().x, 120), false, window_flags);
               ImGui::LabelText(_("core_current_label"), core_name);
               Widgets::Tooltip(_("core_current_desc"));
               ImGui::LabelText(_("core_current_version_label"), core_version);
               Widgets::Tooltip(_("core_current_version_desc"));
               ImGui::LabelText(_("core_current_extensions_label"), supported_extensions);
               Widgets::Tooltip(_("core_current_extensions_desc"));

               ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
               if (ImGui::CollapsingHeader(_("core_current_info_video_label"), ImGuiTreeNodeFlags_None))
               {
                  core_frame_buffer_t* video_data = piccolo->get_video_data();
                  int base_width = width;
                  int base_height = height;

                  ImGui::InputInt(_("framebuffer_width_label"), &base_width, 0, 0, ImGuiInputTextFlags_ReadOnly);
                  Widgets::Tooltip(_("framebuffer_width_desc"));
                  ImGui::InputInt(_("framebuffer_height_label"), &base_height, 0, 0, ImGuiInputTextFlags_ReadOnly);
                  Widgets::Tooltip(_("framebuffer_height_desc"));
                  ImGui::InputFloat(_("framebuffer_aspect_label"), &aspect, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
                  Widgets::Tooltip(_("framebuffer_aspect_desc"));
               }
               ImGui::Unindent();
               ImGui::EndChild();
            }
            if (option_count > 0 && ImGui::CollapsingHeader(_("core_current_options_label"), ImGuiTreeNodeFlags_None))
            {
               ImGuiWindowFlags window_flags = 0;
               ImGui::BeginChild(
                  "options", ImVec2(ImGui::GetContentRegionAvail().x, 120), false, window_flags);
               for (unsigned i = 0; i < option_count; i++)
               {
                  core_option_t* option = &options[i];
                  char* description = option->description;
                  struct string_list* values = OptionGetValues(option);

                  int index = OptionGetIndex(option, values);
                  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.30f);
                  if (Widgets::StringListCombo(description, &index, values, 0))
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

      // Core flags
      if (ImGui::CollapsingHeader(_("core_current_flags_label"), ImGuiTreeNodeFlags_None))
      {
         ImGui::Checkbox(_("core_current_supports_no_game_label"), &supports_no_game);
         Widgets::Tooltip(_("core_current_supports_no_game_desc"));
         ImGui::Checkbox(_("core_current_block_extract_label"), &block_extract);
         Widgets::Tooltip(_("core_current_block_extract_desc"));
         ImGui::Checkbox(_("core_current_full_path_label"), &full_path);
         Widgets::Tooltip(_("core_current_full_path_desc"));
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
         ImGui::TextDisabled(_("no cores found"));
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
            if (Widgets::FileList("", &index, list, 10))
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
