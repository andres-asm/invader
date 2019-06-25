#ifndef PICCOLO_H_
#define PICCOLO_H_

#include "util.h"
#include "libretro.h"

struct core_info
{
   unsigned core_id;
   char file_name[PATH_MAX_LENGTH];
   char core_name[PATH_MAX_LENGTH];
   char core_version[PATH_MAX_LENGTH];
   char extensions[PATH_MAX_LENGTH];

   bool supports_no_game;
} typedef core_info_t;

struct {
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
} piccolo;

void core_peek(const char *in, core_info_t *out);
#endif