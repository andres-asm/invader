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
#include "util.h"
#include "libretro/piccolo.h"

core_info_t core_info[100];

static const char* tag = "[invader]";

/* Initialize configuration */
void cfg_load()
{
   config_init();
   config_load("invader.cfg");
}

bool core_list_init(const char* in)
{
   char buf[PATH_MAX_LENGTH];
   file_list_t *list;
   list = (file_list_t *)calloc(1, sizeof(file_list_t));
   get_file_list(in, list, ".dll");

   for (unsigned i = 0; i < list->file_count; i++)
   {
      strlcpy(core_info[i].file_name, list->file_names[i], sizeof(core_info[i].file_name));
      snprintf(buf, sizeof(buf), "%s/%s", in, list->file_names[i]);
      core_peek(buf, &core_info[i]);

      logger(LOG_INFO, tag, "core name: %s\n", core_info[i].core_name);
      logger(LOG_INFO, tag, "core version: %s\n", core_info[i].core_version);
      logger(LOG_INFO, tag, "valid extensions: %s\n", core_info[i].extensions);
   }

   return true;
}

/* Render the main interface */
void gui_render(struct nk_context *ctx)
{
   static bool test;

   if (!test)
      test = core_list_init(setting_get_string("directory_cores"));

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