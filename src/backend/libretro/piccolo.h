#ifndef PICCOLO_H_
#define PICCOLO_H_

#include "util.h"
#include "libretro.h"

#define load_sym(V, S) do {\
   function_t func = dylib_proc(piccolo.handle, #S); \
   memcpy(&V, &func, sizeof(func)); \
   if (!func) \
      logger(LOG_ERROR, tag, "failed to load symbol '" #S "'': %s"); \
   } while (0)


#define load_retro_sym(S) load_sym(piccolo.S, S)

struct core_frame_buffer
{
   const void *data;
   unsigned width;
   unsigned height;
   unsigned pitch;
} typedef core_frame_buffer_t;

struct audio_buffer
{
   const int16_t* data;
   size_t frames;

   int16_t left;
   int16_t right;
} typedef core_audio_buffer_t;

struct core_info
{
   unsigned core_id;
   char file_name[PATH_MAX_LENGTH];
   char core_name[PATH_MAX_LENGTH];
   char core_version[PATH_MAX_LENGTH];
   char extensions[PATH_MAX_LENGTH];

   bool supports_no_game;
   bool block_extract;
   bool full_path;

   unsigned pixel_format;

   struct retro_system_av_info av_info;
} typedef core_info_t;

struct core_option
{
   char key[100];
   char description[100];
   char value[100];
   char values[PATH_MAX_LENGTH];
} typedef core_option_t;

struct piccolo
{
   void   *handle;
   bool initialized;
   void (*retro_init)(void);
   void (*retro_deinit)(void);
   unsigned (*retro_api_version)(void);
   void (*retro_get_system_info)(struct retro_system_info *info);
   void (*retro_get_system_av_info)(struct retro_system_av_info *info);
   void (*retro_set_controller_port_device)(unsigned port, unsigned device);
   void (*retro_reset)(void);
   void (*retro_run)(void);
   size_t (*retro_serialize_size)(void);
   bool (*retro_serialize)(void *data, size_t size);
   bool (*retro_unserialize)(const void *data, size_t size);
   //void retro_cheat_reset(void);
   //void retro_cheat_set(unsigned index, bool enabled, const char *code);
   bool (*retro_load_game)(const struct retro_game_info *game);
   //bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info);
   void (*retro_unload_game)(void);
   //unsigned retro_get_region(void);
   void* (*retro_get_memory_data)(unsigned id);
   size_t (*retro_get_memory_size)(unsigned id);

   void (*set_variables)(void *data);

   struct retro_system_info system_info;
   struct retro_system_av_info av_info;

   core_option_t *core_options;
   core_info_t *core_info;

   core_frame_buffer_t *video_data;
   core_audio_buffer_t *audio_data;

   unsigned core_option_count;

} typedef piccolo_t;

void core_load(const char *in, core_info_t *info, core_option_t *options, bool peek);

bool core_load_game(const char* filename);

void core_run(core_frame_buffer_t *video_data, core_audio_buffer_t *audio_data);

unsigned core_option_count();
#endif