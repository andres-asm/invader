#include <compat/strl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
// Get declaration for f(int i, char c, float x)
#include <dynamic/dylib.h>
}

extern "C" dylib_t dylib_load(const char* path);
extern "C" void dylib_close(dylib_t lib);
extern "C" char* dylib_error(void);
extern "C" function_t dylib_proc(dylib_t lib, const char* proc);

#include "piccolo.h"
#include "util.h"

static const char* tag = "[core]";

static Piccolo* piccolo_ptr;

Piccolo::Piccolo(core_info_t* info, core_option_t* options)
{
   core_info = info;
   core_options = options;
}

static void piccolo_logger(enum retro_log_level level, const char* fmt, ...)
{
   va_list va;
   char buffer[4096] = {0};
   static const char* level_char = "diwe";

   va_start(va, fmt);
   vsnprintf(buffer, sizeof(buffer), fmt, va);
   va_end(va);

   fprintf(stderr, "[%c] --- %s %s", level_char[level], "[libretro]", buffer);
   fflush(stderr);
}

void Piccolo::core_get_variables(void* data)
{
   if (piccolo_ptr->core_option_count == 0)
      return;

   struct retro_variable* var = (struct retro_variable*)data;
   var->value = NULL;

   for (int i = 0; i < piccolo_ptr->core_option_count; i++)
   {
      if (!strcmp(var->key, piccolo_ptr->core_options[i].key))
         var->value = piccolo_ptr->core_options[i].value;
   }
}

void Piccolo::core_set_variables(void* data)
{
   char buf[PATH_MAX_LENGTH];
   const char* values;
   const char* value;

   piccolo_ptr->core_option_count = 0;

   struct retro_variable* vars = (struct retro_variable*)data;

   /* pointer to count and iterate over options */
   struct retro_variable* count = vars;

   /* count core options */
   while (count->key)
   {
      count++;
      piccolo_ptr->core_option_count++;
   }
   core_option_t* core_options = piccolo_ptr->core_options;

   logger(LOG_DEBUG, tag, "variables: %u\n", piccolo_ptr->core_option_count);
   for (unsigned i = 0; i < piccolo_ptr->core_option_count; i++)
   {
      unsigned j = 0;
      strlcpy(core_options[i].key, vars[i].key, sizeof(core_options[i].key));

      values = strstr(vars[i].value, "; ");
      value = values;

      while (values[j] != '|' && values[j])
      {
         value++;
         j++;
      }

      strlcpy(core_options[i].description, vars[i].value, values + 1 - vars[i].value);
      strlcpy(core_options[i].values, values + 2, sizeof(core_options[i].values));
      strlcpy(core_options[i].value, values + 2, value - values - 1);
#ifdef DEBUG
      logger(
         LOG_DEBUG, tag, "key: %s description: %s values: %s default: %s\n", core_options[i].key,
         core_options[i].description, core_options[i].values, core_options[i].value);
#endif
   }
}

bool Piccolo::core_set_environment(unsigned cmd, void* data)
{
   switch (cmd)
   {
   case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME: {
      logger(
         LOG_INFO, tag, "RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME: %s\n", PRINT_BOOLEAN(*(bool*)data));
      piccolo_ptr->core_info->supports_no_game = *(bool*)data;
      break;
   }
   case RETRO_ENVIRONMENT_SET_VARIABLES: {
      logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_SET_VARIABLES:\n");
      piccolo_ptr->core_set_variables(data);
      break;
   }
   case RETRO_ENVIRONMENT_GET_VARIABLE: {
      struct retro_variable* var = (struct retro_variable*)data;
      piccolo_ptr->core_get_variables(data);
      logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_GET_VARIABLE: %s=%s\n", var->key, var->value);
      break;
   }
   case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE: {
      if (piccolo_ptr->core_options)
         *(bool*)data = piccolo_ptr->core_options_updated;
      else
         *(bool*)data = false;
      piccolo_ptr->core_options_updated = false;
      break;
   }
   case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
      logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: %s\n", PRINT_PIXFMT(*(int*)data));
      piccolo_ptr->core_info->pixel_format = *(int*)data;
      break;
   case RETRO_ENVIRONMENT_GET_LOG_INTERFACE: {
      logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_GET_LOG_INTERFACE\n");
      struct retro_log_callback* callback = (struct retro_log_callback*)data;
      callback->log = piccolo_logger;
      break;
   }
   case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY: {
      logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY\n", "./");
      *(const char**)data = "C:\\";
      break;
   }
   case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY: {
      logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY %s\n", "./");
      *(const char**)data = "C:\\";
      break;
   }
   case RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY: {
      logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY\n", "./");
      *(const char**)data = "C:\\";
      break;
   }
   case RETRO_ENVIRONMENT_GET_CAN_DUPE:
      *(bool*)data = true;
      break;
   case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
      logger(LOG_DEBUG, tag, "RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS unhandled\n");
      break;
   case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
      logger(LOG_DEBUG, tag, "RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL unhandled\n");
      break;
   case RETRO_ENVIRONMENT_GET_FASTFORWARDING:
      logger(LOG_DEBUG, tag, "RETRO_ENVIRONMENT_GET_FASTFORWARDING unhandled\n");
      break;
   case RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION:
      *(unsigned*)data = 0;
      logger(LOG_INFO, tag, "RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION: %d\n", *(unsigned*)data);
      break;
   default:
      logger(LOG_DEBUG, tag, "unknown command: %d\n", cmd);
   }
   return true;
}

void Piccolo::core_input_poll()
{
   return;
}

int16_t Piccolo::core_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
   return 0;
}

void Piccolo::core_audio_sample(int16_t left, int16_t right)
{
   return;
}

size_t Piccolo::core_audio_sample_batch(const int16_t* data, size_t frames)
{
   return piccolo_ptr->audio_callback(data, frames);
}

void Piccolo::core_video_refresh(const void* data, unsigned width, unsigned height, size_t pitch)
{
   piccolo_ptr->video_data->data = data;
   piccolo_ptr->video_data->width = width;
   piccolo_ptr->video_data->height = height;
   piccolo_ptr->video_data->pitch = pitch;
   return;
}

bool Piccolo::core_load(const char* in, bool peek)
{
   initialized = false;
   piccolo_ptr = this;

   core_option_count = 0;
   core_info->supports_no_game = false;

   void (*set_environment)(retro_environment_t) = NULL;
   void (*set_video_refresh)(retro_video_refresh_t) = NULL;
   void (*set_input_poll)(retro_input_poll_t) = NULL;
   void (*set_input_state)(retro_input_state_t) = NULL;
   void (*set_audio_sample)(retro_audio_sample_t) = NULL;
   void (*set_audio_sample_batch)(retro_audio_sample_batch_t) = NULL;

   handle = dylib_load(in);

   void (*proc)(struct retro_system_info*);
   proc = (void (*)(struct retro_system_info*))dylib_proc(handle, "retro_get_system_info");

   if (!handle)
   {
      logger(LOG_ERROR, tag, "failed to load library: %s\n");
      return false;
   }

   load_retro_sym(retro_api_version);
   load_retro_sym(retro_get_system_info);
   load_sym(set_environment, retro_set_environment);

   retro_api_version();
   retro_get_system_info(&system_info);

   strlcpy(core_info->file_name, in, sizeof(core_info->file_name));
   strlcpy(core_info->core_name, system_info.library_name, sizeof(core_info->core_name));
   strlcpy(core_info->core_version, system_info.library_version, sizeof(core_info->core_version));
   if (system_info.valid_extensions)
      strlcpy(core_info->extensions, system_info.valid_extensions, sizeof(core_info->extensions));
   else
      strlcpy(core_info->extensions, "N/A", sizeof(core_info->extensions));
   core_info->full_path = system_info.need_fullpath;
   core_info->block_extract = system_info.block_extract;

#ifdef DEBUG
   logger(LOG_DEBUG, tag, "retro api version: %d\n", retro_api_version());
   logger(LOG_DEBUG, tag, "core name: %s\n", core_info->core_name);
   logger(LOG_DEBUG, tag, "core version: %s\n", core_info->core_version);
   logger(LOG_DEBUG, tag, "valid extensions: %s\n", core_info->extensions);
#endif

   set_environment(core_set_environment);

   if (peek)
   {
      dylib_close(handle);
      return true;
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

   set_video_refresh(core_video_refresh);
   set_input_poll(core_input_poll);
   set_input_state(core_input_state);
   set_audio_sample(core_audio_sample);
   set_audio_sample_batch(core_audio_sample_batch);

   retro_get_system_av_info(&av_info);

   core_info->av_info.geometry.base_width = av_info.geometry.base_width;
   core_info->av_info.geometry.base_height = av_info.geometry.base_height;
   core_info->av_info.geometry.max_width = av_info.geometry.max_width;
   core_info->av_info.geometry.max_height = av_info.geometry.max_height;
   core_info->av_info.geometry.aspect_ratio = av_info.geometry.aspect_ratio;

   logger(
      LOG_DEBUG, tag, "geometry: %ux%d/%ux%d %f\n", av_info.geometry.base_width,
      av_info.geometry.base_height, av_info.geometry.max_width, av_info.geometry.max_height,
      av_info.geometry.aspect_ratio);
   logger(LOG_DEBUG, tag, "timing: %ffps %fHz\n", av_info.timing.fps, av_info.timing.sample_rate);
   retro_init();
   initialized = true;

   return true;
}

/*
void core_load(const char* in, core_info_t* info, core_option_t* options, bool peek)
{


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

   piccolo.core_info->av_info.geometry.base_width = piccolo.av_info.geometry.base_width;
   piccolo.core_info->av_info.geometry.base_height = piccolo.av_info.geometry.base_height;
   piccolo.core_info->av_info.geometry.max_width = piccolo.av_info.geometry.max_width;
   piccolo.core_info->av_info.geometry.max_height = piccolo.av_info.geometry.max_height;
   piccolo.core_info->av_info.geometry.aspect_ratio = piccolo.av_info.geometry.aspect_ratio;

   logger(
      LOG_DEBUG, tag, "geometry: %ux%d/%ux%d %f\n", piccolo.av_info.geometry.base_width,
      piccolo.av_info.geometry.base_height, piccolo.av_info.geometry.max_width,
      piccolo.av_info.geometry.max_height, piccolo.av_info.geometry.aspect_ratio);
   logger(
      LOG_DEBUG, tag, "timing: %ffps %fHz\n", piccolo.av_info.timing.fps,
      piccolo.av_info.timing.sample_rate);
   piccolo.retro_init();
   piccolo.initialized = true;
}
 */
