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
#include <lists/string_list.h>

#include <GL/glew.h>

#include "invader.h"
#include "config.h"
#include "util.h"
#include "libretro/piccolo.h"

core_info_t core_info_list[100];
core_option_t core_options[100];

core_info_t current_core_info;
unsigned core_count;
unsigned current_core;

core_frame_buffer_t frame_buffer;


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
      strlcpy(core_info_list[i].file_name, list->file_names[i], sizeof(core_info_list[i].file_name));
      snprintf(buf, sizeof(buf), "%s/%s", in, list->file_names[i]);
      core_load(buf, &core_info_list[i], core_options, true);
      core_info_list[i].core_id = i;

#ifdef DEBUG
      logger(LOG_DEBUG, tag, "core name: %s\n", core_info_list[i].core_name);
      logger(LOG_DEBUG, tag, "core version: %s\n", core_info_list[i].core_version);
      logger(LOG_DEBUG, tag, "valid extensions: %s\n", core_info_list[i].extensions);
#endif
      core_count++;
   }

   return true;
}

/* To-Do: Move this code elsewhere, GL specific code has no place here */
GLuint tex;
static struct nk_image compose_framebuffer(const void *data, unsigned width, unsigned height, unsigned pitch)
{
   if (!tex)
      glGenTextures(1, &tex);

   glBindTexture(GL_TEXTURE_2D, tex);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   switch (current_core_info.pixel_format)
   {
      case RETRO_PIXEL_FORMAT_XRGB8888:
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, data);
         glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch / sizeof(uint32_t));
         break;
      case RETRO_PIXEL_FORMAT_RGB565:
         glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
         glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch / sizeof(uint16_t));
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
         break;
      default:
         logger(LOG_DEBUG, tag, "pixel format: %s (%d) unhandled\n", PRINT_PIXFMT(current_core_info.pixel_format), current_core_info.pixel_format);

   }

   return nk_image_id((int)tex);
}

/* Render the main interface */
void gui_render(struct nk_context *ctx)
{
   static bool initialized;
   static unsigned previous_core;
   static bool running;

   if (!initialized)
      core_list_init(setting_get_string("directory_cores"));

   /* GUI */
   if (nk_begin(ctx, "Just a placeholder GUI", nk_rect(10, 10, 500, 700),
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

      const char* core_entries[100];
      for (unsigned i = 0; i < core_count; i++)
         core_entries[i] = core_info_list[i].core_name;

      current_core = nk_combo(ctx, core_entries, core_count, current_core, 30, nk_vec2(200,200));

      nk_layout_row_dynamic(ctx, 30, 2);
      if(current_core_info.supports_no_game)
      {
         if(nk_button_label(ctx, "Start core"))
         {
            core_load(core_info_list[current_core].file_name, &current_core_info, core_options, false);
            if(core_load_game(NULL))
            {
               running = true;
            }
         }
      }
      if (nk_button_label(ctx, "Load content"))
      {
         core_load(core_info_list[current_core].file_name, &current_core_info, core_options, false);
         if(core_load_game("rom.sfc"))
         {
            running = true;
         }
      }

      if (core_count !=0 && (!initialized || previous_core != current_core))
      {
         core_load(core_info_list[current_core].file_name, &current_core_info, core_options, true);
         previous_core = current_core;
      }

      nk_layout_row_dynamic(ctx, 30, 2);
      nk_label(ctx, "Core version:", NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      nk_label(ctx, current_core_info.core_version, NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      nk_label(ctx, "Valid extensions:", NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      nk_label_wrap(ctx, current_core_info.extensions);

      nk_layout_row_dynamic(ctx, 160, 1);
      nk_group_begin(ctx, "Core flags", NK_WINDOW_TITLE | NK_WINDOW_ROM);
      nk_layout_row_dynamic(ctx, 30, 1);
      nk_checkbox_bool(ctx, "Supports running without game", &current_core_info.supports_no_game);
      nk_checkbox_bool(ctx, "Requires game full path", &current_core_info.full_path);
      nk_checkbox_bool(ctx, "Block extraction of archives", &current_core_info.block_extract);
      nk_group_end(ctx);

      nk_layout_row_dynamic(ctx, 240, 1);
      int flags = running ? NK_WINDOW_TITLE : NK_WINDOW_TITLE | NK_WINDOW_ROM;
      nk_group_begin(ctx, "Core options", flags);
      for (unsigned i = 0; i < core_option_count(); i++)
      {
         struct string_list *list = string_split(core_options[i].values, "|");
         nk_layout_row_dynamic(ctx, 30, 1);
         nk_label(ctx, core_options[i].description, NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
         nk_layout_row_dynamic(ctx, 30, 1);
         /* To-Do: set return value */
         nk_combo_string_list(ctx, list, 0, 30, nk_vec2(200,200));
      }
      nk_group_end(ctx);
   }
   nk_end(ctx);

   initialized = true;
   if (running)
   {
      core_run(&frame_buffer);
         if (nk_begin(ctx, "Video output", nk_rect(520, 10, 664, 700),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
         {
            nk_layout_space_begin(ctx, NK_STATIC,400, INT_MAX);
            nk_layout_space_push(ctx, nk_rect(0, 0, 640, 480));
            nk_image(ctx, compose_framebuffer(frame_buffer.data, frame_buffer.width, frame_buffer.height, frame_buffer.pitch));
            nk_layout_space_end(ctx);
         }
         nk_end(ctx);
   }
}