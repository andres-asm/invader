#ifndef WIDGET_H_
#define WIDGET_H_

#include "common.h"
#include "kami.h"

#include "settings.h"

namespace Widgets
{
// generic widgets
void Tooltip(const char* desc);
bool FileList(const char* label, int* current_item, file_list_t* list, int popup_max_height_in_items);
bool StringListCombo(const char* label, int* current_item, struct string_list* list, int popup_max_height_in_items);
bool ControllerTypesCombo(
   const char* label, int* current_item, const controller_description_t* list, size_t size,
   int popup_max_height_in_items);

}  // namespace Widgets

#endif