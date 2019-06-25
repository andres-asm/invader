#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <compat/strl.h>
#include <dynamic/dylib.h>

#include "piccolo.h"
#include "util.h"

static const char *tag = "[core]";

piccolo_t piccolo = {0};

struct retro_game_info   piccolo_game_info   = {0};

static void piccolo_logger(enum retro_log_level level, const char *fmt, ...)
{
   va_list va;
   char buffer[4096] = {0};
   static const char *level_char = "diwe";

   va_start(va, fmt);
   vsnprintf(buffer, sizeof(buffer), fmt, va);
   va_end(va);

   fprintf(stderr, "[%c] --- %s %s", level_char[level], "[libretro]", buffer);
   fflush(stderr);
}

static void piccolo_set_variables(void *data)
{
   char buf[PATH_MAX_LENGTH];
   const char *values;
   const char *value;

   piccolo.core_option_count = 0;

   struct retro_variable *vars = (struct retro_variable*)data;

   /* pointer to count and iterate over options */
   struct retro_variable *count = vars;

   /* count core options */
   while (count->key)
   {
      count++;
      piccolo.core_option_count++;
   }
   core_option_t * core_options = piccolo.core_options;

   logger(LOG_DEBUG, tag, "variables: %u\n", piccolo.core_option_count);
   for (unsigned i = 0; i < piccolo.core_option_count; i++)
   {
      unsigned j = 0;
      strlcpy(core_options[i].key, vars[i].key, sizeof(core_options[i].key));

      values = strstr(vars[i].value, "; ");
      value = values;

      while(values[j] != '|' && values[j])
      {
         value++;
         j++;
      }

      strlcpy(core_options[i].description, vars[i].value, values + 1 - vars[i].value);
      strlcpy(core_options[i].values, values + 2, sizeof(core_options[i].values));
      strlcpy(core_options[i].value, values + 2, value - values - 1);
      logger(LOG_DEBUG, tag, "key: %s description: %s values: %s default: %s\n",
         core_options[i].key, core_options[i].description, core_options[i].values, core_options[i].value);
   }
}

static bool piccolo_set_environment(unsigned cmd, void *data)
{
   switch(cmd)
   {
      case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
      {
         logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME: %s\n", PRINT_BOOLEAN(*(bool*)data));
         piccolo.core_info->supports_no_game = *(bool*)data;
         break;
      }
      case RETRO_ENVIRONMENT_SET_VARIABLES:
      {
         logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_SET_VARIABLES:\n");
         piccolo.set_variables(data);
         break;
      }
      case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
         logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: %s\n", PRINT_PIXFMT(*(int*)data));
         piccolo.core_info->pixel_format = *(int*)data;
         break;
      case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
      {
         logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_GET_LOG_INTERFACE\n");
         struct retro_log_callback *callback = (struct retro_log_callback*)data;
         callback->log = piccolo_logger;
         break;
      }
      default:
         logger(LOG_DEBUG, tag, "unknown command: %d\n", cmd);
   }
   return true;
}

static void piccolo_set_av_info(struct retro_system_av_info *info)
{

}

static void piccolo_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
{
   return;
}

static void piccolo_input_poll()
{
   return;
}

static int16_t piccolo_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
   return 0;
}

static void piccolo_audio_sample(int16_t left, int16_t right)
{
   return;
}

static size_t piccolo_audio_sample_batch(const int16_t *data, size_t frames)
{
   return 0;
}

void core_load(const char *in, core_info_t *info, core_option_t *options, bool peek)
{
   piccolo.core_options = options;
   piccolo.core_info = info;
   piccolo.core_info->supports_no_game = false;

   void (*set_environment)(retro_environment_t) = NULL;
   void (*set_video_refresh)(retro_video_refresh_t) = NULL;
   void (*set_input_poll)(retro_input_poll_t) = NULL;
   void (*set_input_state)(retro_input_state_t) = NULL;
   void (*set_audio_sample)(retro_audio_sample_t) = NULL;
   void (*set_audio_sample_batch)(retro_audio_sample_batch_t) = NULL;

   piccolo.handle = dylib_load(in);
   piccolo.set_variables = piccolo_set_variables;

   void (*proc)(struct retro_system_info*);

   proc = (void (*)(struct retro_system_info*))
      dylib_proc(piccolo.handle, "retro_get_system_info");

   if (!piccolo.handle) {
      logger(LOG_ERROR, tag, "failed to load library: %s\n");
   }

   load_retro_sym(retro_api_version);
   load_retro_sym(retro_get_system_info);
   load_sym(set_environment, retro_set_environment);

   piccolo.retro_api_version();
   piccolo.retro_get_system_info(&piccolo.system_info);

   strlcpy(piccolo.core_info->file_name, in, sizeof(piccolo.core_info->file_name));
   strlcpy(piccolo.core_info->core_name, piccolo.system_info.library_name, sizeof(piccolo.core_info->core_name));
   strlcpy(piccolo.core_info->core_version, piccolo.system_info.library_version, sizeof(piccolo.core_info->core_version));
   strlcpy(piccolo.core_info->extensions, piccolo.system_info.valid_extensions, sizeof(piccolo.core_info->extensions));
   piccolo.core_info->full_path = piccolo.system_info.need_fullpath;
   piccolo.core_info->block_extract = piccolo.system_info.block_extract;

#ifndef DEBUG
   logger(LOG_DEBUG, tag, "retro api version: %d\n", piccolo.retro_api_version());
   logger(LOG_DEBUG, tag, "core name: %s\n", piccolo.system_info.library_name);
   logger(LOG_DEBUG, tag, "core version: %s\n", piccolo.system_info.library_version);
   logger(LOG_DEBUG, tag, "valid extensions: %s\n", piccolo.system_info.valid_extensions);
#endif

   set_environment(piccolo_set_environment);

   if (peek)
   {
      dylib_close(piccolo.handle);
      return;
   }

   load_retro_sym(retro_init);
   load_retro_sym(retro_deinit);
   load_retro_sym(retro_get_system_av_info);
   load_retro_sym(retro_set_controller_port_device);
   load_retro_sym(retro_reset);
   load_retro_sym(retro_run);
   load_retro_sym(retro_load_game);
   load_retro_sym(retro_unload_game);
   load_retro_sym(retro_get_memory_data);
   load_retro_sym(retro_get_memory_size);

   load_retro_sym(retro_serialize);
   load_retro_sym(retro_serialize_size);
   load_retro_sym(retro_unserialize);

   load_sym(set_video_refresh, retro_set_video_refresh);
   load_sym(set_input_poll, retro_set_input_poll);
   load_sym(set_input_state, retro_set_input_state);
   load_sym(set_audio_sample, retro_set_audio_sample);
   load_sym(set_audio_sample_batch, retro_set_audio_sample_batch);

   set_video_refresh(piccolo_video_refresh);
   set_input_poll(piccolo_input_poll);
   set_input_state(piccolo_input_state);
   set_audio_sample(piccolo_audio_sample);
   set_audio_sample_batch(piccolo_audio_sample_batch);

   piccolo.retro_get_system_av_info(&piccolo.av_info);

   logger(LOG_DEBUG, tag, "geometry: %ux%d/%ux%d %f\n",
      piccolo.av_info.geometry.base_width, piccolo.av_info.geometry.base_height,
      piccolo.av_info.geometry.max_width, piccolo.av_info.geometry.max_height,
      piccolo.av_info.geometry.aspect_ratio);
   logger(LOG_DEBUG, tag, "timing: %ffps %fHz\n",
      piccolo.av_info.timing.fps, piccolo.av_info.timing.sample_rate);
   piccolo.retro_init();
   piccolo.initialized = true;
}

unsigned core_option_count()
{
   return piccolo.core_option_count;
}

/*

   piccolo.initialized = true;
*/


