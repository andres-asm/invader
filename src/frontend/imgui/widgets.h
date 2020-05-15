#ifndef WIDGET_H_
#define WIDGET_H_

#include "common.h"
#include "kami.h"

void tooltip(const char* desc);
bool file_list(const char* label, int* current_item, file_list_t* list, int popup_max_height_in_items);
bool string_list_combo(const char* label, int* current_item, struct string_list* list, int popup_max_height_in_items);
bool controller_combo(
   const char* label, int* current_item, const controller_description_t* list, size_t size,
   int popup_max_height_in_items);

#endif