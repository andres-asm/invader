// dependency includes
#include "imgui.h"

#include "widgets.h"

static const char* tag = "[widgets]";

static bool file_manager_dialog_is_open = false;

// tooltip widget
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

// file list widget
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

// string list combo widget
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

// controller entries widget
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

// setting checkbox
bool setting_checkbox(Setting<bool>* setting)
{
   std::string label(setting->GetName());
   label = label + "_label";

   std::string desc(setting->GetName());
   desc = desc + "_desc";

   bool ret = ImGui::Checkbox(_(label.c_str()), setting->GetPtr());
   tooltip(_(desc.c_str()));

   if (ret)
      setting->SetChanged();

   return ret;
}

// TODO: finish this up
// rudimentary file manager
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
         //
         static bool dont_ask_me_next_time = false;
         ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
         ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
         ImGui::PopStyleVar();

         // if (ImGui::Button(_("button_select_label"), ImVec2(240, 0)))
         //{
         //   file_open_dialog_result_ok = true;
         //   file_open_dialog_is_open = false;
         //   strlcpy(content_file_name, cur, sizeof(content_file_name));
         //   ImGui::CloseCurrentPopup();
         //}
         ImGui::SetItemDefaultFocus();
         ImGui::SameLine();
         // if (ImGui::Button("Cancel", ImVec2(240, 0)))
         //{
         //   file_open_dialog_result_ok = false;
         //      file_open_dialog_is_open = false;
         // ImGui::CloseCurrentPopup();
         //}
         ImGui::EndPopup();
      }
   }
}
