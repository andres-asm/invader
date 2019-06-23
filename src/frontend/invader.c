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

#include "invader.h"
#include "config.h"

/* Initialize configuration */
void cfg_load()
{
   config_init();
   config_load("invader.cfg");
}

/* Render the main interface */
void gui_render(struct nk_context *ctx)
{
   /* GUI */
   if (nk_begin(ctx, "Obviously not the final GUI", nk_rect(50, 50, 300, 300),
         NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
         NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
   {
      nk_layout_row_dynamic(ctx, 30, 2);
      nk_label(ctx, setting_get("directory_cores")->desc, NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);

      char* string = setting_get_string("directory_cores");
      int len = strlen(setting_get_string("directory_cores"));
      int size = setting_get("directory_cores")->size;

      nk_edit_string(ctx, NK_EDIT_SIMPLE, string, &len, size, nk_filter_default);

      nk_label(ctx, setting_get("directory_games")->desc, NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);

      string = setting_get_string("directory_games");
      len = strlen(setting_get_string("directory_games"));
      size = setting_get("directory_games")->size;

      nk_edit_string(ctx, NK_EDIT_SIMPLE, string, &len, size, nk_filter_default);
   }
   nk_end(ctx);
}