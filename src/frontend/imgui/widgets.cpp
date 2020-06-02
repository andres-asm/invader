// dependency includes
#include "imgui.h"

#include "widgets.h"

static const char* tag = "[widgets]";
static bool file_manager_dialog_is_open = false;

// generic widgets
namespace Widgets
{
// controller entries widget
bool ControllerTypesCombo(
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

// TODO: replace libretro common string list with std::vector
// string list combo widget
bool StringListCombo(const char* label, int* current_item, struct string_list* list, int popup_max_height_in_items)
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

// TODO: replace paths with std::filesystem
// file list widget
bool FileList(const char* label, int* current_item, file_list_t* list, int popup_max_height_in_items)
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

// tooltip widget
void Tooltip(const char* desc)
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

}  // namespace Widgets
