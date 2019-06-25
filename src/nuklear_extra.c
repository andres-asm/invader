#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <compat/strl.h>

#include "invader.h"
#include "config.h"
#include "nuklear_extra.h"

bool nk_checkbox_bool(struct nk_context* ctx, const char* label, bool *active)
{
   int    x = *active;
   bool ret = nk_checkbox_label(ctx, label, &x);
   *active  = x;

   return ret;
}

/* This is probable very bad for performance */
int nk_combo_string_list(struct nk_context *ctx, struct string_list *list, int selected, int item_height, struct nk_vec2 size)
{
   int ret = 0;
   const char **entries = calloc(list->size, sizeof (char *));
   for (unsigned i = 0; i < list->size; i++)
   {
      entries[i] = list->elems[i].data;
   }

   ret = nk_combo(ctx, entries, list->size, selected, item_height, size);
   free(entries);
}
