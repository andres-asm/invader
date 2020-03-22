#ifndef NUKLEAR_EXTRA_H_
#define NUKLEAR_EXTRA_H_

#include <stdbool.h>
#include <lists/string_list.h>

bool nk_checkbox_bool(struct nk_context* ctx, const char* label, bool *active);

int nk_combo_string_list(struct nk_context *ctx, struct string_list *list, int selected, int item_height, struct nk_vec2 size);

#endif