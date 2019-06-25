#include <string.h>
#include <stdio.h>
#include <compat/strl.h>
#include <dynamic/dylib.h>

#include "piccolo.h"
#include "util.h"

static const char *tag = "[core]";
static bool supports_no_game;

#define load_sym(V, S) do {\
   if (!(V = dylib_proc(piccolo.handle, #S))) \
      logger(LOG_ERROR, tag, "failed to load symbol '" #S "'': %s"); \
   } while (0)

#define load_retro_sym(S) load_sym(piccolo.S, S)

static struct retro_system_info piccolo_system_info = {0};
static struct retro_game_info   piccolo_game_info   = {0};


static bool piccolo_set_environment(unsigned cmd, void *data)
{
   switch(cmd)
   {
      case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
      {
         supports_no_game = (bool*)data;
         logger(LOG_DEBUG, tag, "RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME: %s\n", PRINT_BOOLEAN(supports_no_game));
         break;
      }
      default:
         logger(LOG_DEBUG, tag, "unknown command: %d\n", cmd);
   }
   return true;
}

/*
static void core_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
{
   return;
}

static void core_input_poll()
{
   return;
}

static int16_t core_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
   return 0;
}

static void core_audio_sample(int16_t left, int16_t right)
{
   return;
}

static size_t core_audio_sample_batch(const int16_t *data, size_t frames)
{
   return 0;
}
*/

void core_peek(const char *in, core_info_t *out)
{
   supports_no_game = false;

   void (*set_environment)(retro_environment_t) = NULL;
   void (*set_video_refresh)(retro_video_refresh_t) = NULL;
   void (*set_input_poll)(retro_input_poll_t) = NULL;
   void (*set_input_state)(retro_input_state_t) = NULL;
   void (*set_audio_sample)(retro_audio_sample_t) = NULL;
   void (*set_audio_sample_batch)(retro_audio_sample_batch_t) = NULL;

   memset(&piccolo, 0, sizeof(piccolo));
   piccolo.handle = dylib_load(in);


   void (*proc)(struct retro_system_info*);

   proc = (void (*)(struct retro_system_info*))
      dylib_proc(piccolo.handle, "retro_get_system_info");

   if (!piccolo.handle) {
      logger(LOG_ERROR, tag, "failed to load library: %s\n");
   }

   load_retro_sym(retro_api_version);
   load_retro_sym(retro_get_system_info);
   load_sym(set_environment, retro_set_environment);

   piccolo.retro_get_system_info(&piccolo_system_info);

   strlcpy(out->file_name, in, sizeof(out->file_name));
   strlcpy(out->core_name, piccolo_system_info.library_name, sizeof(out->core_name));
   strlcpy(out->core_version, piccolo_system_info.library_version, sizeof(out->core_version));
   strlcpy(out->extensions, piccolo_system_info.valid_extensions, sizeof(out->extensions));

#ifndef DEBUG
   logger(LOG_DEBUG, tag, "retro api version: %d\n", piccolo.retro_api_version());
   logger(LOG_DEBUG, tag, "core name: %s\n", piccolo_system_info.library_name);
   logger(LOG_DEBUG, tag, "core version: %s\n", piccolo_system_info.library_version);
   logger(LOG_DEBUG, tag, "valid extensions: %s\n", piccolo_system_info.valid_extensions);
#endif

   set_environment(piccolo_set_environment);
   out->supports_no_game = supports_no_game;

   dylib_close(piccolo.handle);
}

/*
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

   piccolo.retro_get_system_info(&piccolo_system_info);

   load_sym(set_environment, retro_set_environment);
   load_sym(set_video_refresh, retro_set_video_refresh);
   load_sym(set_input_poll, retro_set_input_poll);
   load_sym(set_input_state, retro_set_input_state);
   load_sym(set_audio_sample, retro_set_audio_sample);
   load_sym(set_audio_sample_batch, retro_set_audio_sample_batch);

   set_environment(core_environment);
   set_video_refresh(core_video_refresh);
   set_input_poll(core_input_poll);
   set_input_state(core_input_state);
   set_audio_sample(core_audio_sample);
   set_audio_sample_batch(core_audio_sample_batch);
   piccolo.initialized = true;
*/


