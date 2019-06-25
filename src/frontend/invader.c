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
core_info_t current_core_info;
unsigned core_count;
unsigned current_core;

core_option_t *current_core_options;

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
#ifdef _WIN32
   get_file_list(in, list, ".dll");
#else
   get_file_list(in, list, ".so");
#endif
   logger(LOG_DEBUG, tag, "core count: %d\n", list->file_count);
   for (unsigned i = 0; i < list->file_count; i++)
   {
      strlcpy(core_info[i].file_name, list->file_names[i], sizeof(core_info[i].file_name));
      snprintf(buf, sizeof(buf), "%s/%s", in, list->file_names[i]);
      core_peek(buf, &core_info[i], current_core_options);
      core_info[i].core_id = i;

#ifdef DEBUG
      logger(LOG_DEBUG, tag, "core name: %s\n", core_info[i].core_name);
      logger(LOG_DEBUG, tag, "core version: %s\n", core_info[i].core_version);
      logger(LOG_DEBUG, tag, "valid extensions: %s\n", core_info[i].extensions);
#endif
      core_count++;
   }

   return true;
}

/* Render the main interface */
void gui_render(struct nk_context *ctx)
{
   static bool initialized;
   static unsigned previous_core;

   if (!initialized)
      core_list_init(setting_get_string("directory_cores"));

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

      nk_layout_row_dynamic(ctx, 30, 1);
      nk_label(ctx, "Core:", NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);

      char* core_entries[100];
      for (unsigned i = 0; i < core_count; i++)
         core_entries[i] = core_info[i].core_name;

      current_core = nk_combo(ctx, core_entries, core_count, current_core, 30, nk_vec2(200,200));

      if (core_count !=0 && (!initialized || previous_core != current_core))
      {
         core_peek(core_info[current_core].file_name, &current_core_info, current_core_options);
         previous_core = current_core;
      }

      nk_layout_row_dynamic(ctx, 30, 2);
      nk_label(ctx, "Core version:", NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      nk_label(ctx, current_core_info.core_version, NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      nk_label(ctx, "Valid extensions:", NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      nk_label(ctx, current_core_info.extensions, NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      nk_layout_row_dynamic(ctx, 30, 1);
      nk_checkbox_bool(ctx, "Supports running without game", &current_core_info.supports_no_game);
   }
   nk_end(ctx);

   initialized = true;
}