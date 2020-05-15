#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kami.h"

static const char* tag = "[invader]";

SDL_AudioSpec want, have;
SDL_AudioDeviceID device;

bool Kami::CoreListInit(const char* path)
{
   char buf[PATH_MAX_LENGTH];
   bool peeked = false;
   file_list_t* core_list = (file_list_t*)calloc(1, sizeof(file_list_t));
#ifdef _WIN32
   get_file_list(path, core_list, ".dll", false);
#else
   get_file_list(path, core_list, ".so", false);
#endif

   piccolo = new PiccoloWrapper();
   logger(LOG_DEBUG, tag, "core count: %d\n", core_list->file_count);
   for (unsigned i = 0; i < core_list->file_count; i++)
   {
      snprintf(buf, sizeof(buf), "%s/%s", path, core_list->file_names[i]);
      peeked = piccolo->peek_core(buf);
      if (peeked)
      {
         core_info_t* info = piccolo->get_info();

         strlcpy(core_info_list[i].file_name, info->file_name, sizeof(core_info_list[i].file_name));
         strlcpy(core_info_list[i].core_name, info->core_name, sizeof(core_info_list[i].core_name));
         strlcpy(core_info_list[i].core_version, info->core_version, sizeof(core_info_list[i].core_version));
         strlcpy(core_info_list[i].extensions, info->extensions, sizeof(core_info_list[i].extensions));
         core_info_list[i].supports_no_game = info->supports_no_game;
         core_info_list[i].block_extract = info->block_extract;
         core_info_list[i].full_path = info->full_path;

         logger(LOG_DEBUG, tag, "file name: %s\n", core_info_list[i].file_name);
         logger(LOG_DEBUG, tag, "core name: %s\n", core_info_list[i].core_name);
         logger(LOG_DEBUG, tag, "core version: %s\n", core_info_list[i].core_version);
         logger(LOG_DEBUG, tag, "valid extensions: %s\n", core_info_list[i].extensions);
         core_count++;
      }
      piccolo->unload_core();
   }

   logger(LOG_DEBUG, tag, "cores found: %d\n", core_count);
   for (unsigned i = 0; i < core_count; i++)
   {
      core_entries[i] = core_info_list[i].core_name;
      logger(LOG_DEBUG, tag, "loading file %s\n", core_entries[i]);
   }
   free(core_list);
   return true;
}

void Kami::OptionUpdate(core_option_t* option, const char* value)
{
   logger(LOG_INFO, tag, "changing option %s to %s\n", option->description, value);
   strlcpy(option->value, value, sizeof(option->value));
   piccolo->set_options_updated();
}

struct string_list* Kami::OptionGetValues(core_option_t* option)
{
   return string_split(option->values, "|");
}

unsigned Kami::OptionGetIndex(core_option_t* option, struct string_list* values)
{
   unsigned index = 0;
   for (unsigned i = 0; i < values->size; i++)
   {
      if ((string_is_equal(values->elems[i].data, option->value)))
         index = i;
   }
   return index;
}

void Kami::ParseInputDescriptors()
{
   input_descriptor_t* new_descriptors = piccolo->get_input_descriptors();
   unsigned port = 0;
   unsigned id = 0;
   unsigned idx = 0;
   const char* desc;

   for (unsigned i = 0; i < piccolo->get_input_descriptor_count(); i++)
   {
      port = new_descriptors[i].port;
      id = new_descriptors[i].id;
      idx = new_descriptors[i].index;
      desc = new_descriptors[i].description;

      input_descriptors[port][id].port = port;
      input_descriptors[port][id].id = id;
      input_descriptors[port][id].index = idx;
      input_descriptors[port][id].description = desc;
   }
}

size_t kami_render_audio(const int16_t* data, size_t frames)
{
   SDL_QueueAudio(device, data, 4 * frames);
   return frames;
}

bool kami_init_audio()
{
   int devices = SDL_GetNumAudioDevices(0);

   logger(LOG_INFO, tag, "audio devices: %d\n", devices);

   for (unsigned i = 0; i < devices; i++)
      logger(LOG_INFO, tag, "device %d: %s\n", i, SDL_GetAudioDeviceName(i, 0));

   SDL_zero(want);

   want.freq = 48000;
   want.format = AUDIO_S16;
   want.channels = 2;
   want.samples = 4096;
   want.callback = NULL;

   logger(
      LOG_INFO, tag, "want - frequency: %d format: f %d s %d be %d sz %d channels: %d samples: %d\n", want.freq,
      SDL_AUDIO_ISFLOAT(want.format), SDL_AUDIO_ISSIGNED(want.format), SDL_AUDIO_ISBIGENDIAN(want.format),
      SDL_AUDIO_BITSIZE(want.format), want.channels, want.samples);
   device = SDL_OpenAudioDevice(0, 0, &want, &have, 0);
   if (!device)
   {
      logger(LOG_ERROR, tag, "failed to open audio device: %s\n", SDL_GetError());
      SDL_Quit();
      return false;
   }
   else
      logger(LOG_ERROR, tag, "opened audio device: %s\n", SDL_GetAudioDeviceName(0, 0));

   logger(
      LOG_INFO, tag, "have - frequency: %d format: f %d s %d be %d sz %d channels: %d samples: %d\n", have.freq,
      SDL_AUDIO_ISFLOAT(have.format), SDL_AUDIO_ISSIGNED(have.format), SDL_AUDIO_ISBIGENDIAN(have.format),
      SDL_AUDIO_BITSIZE(have.format), have.channels, have.samples);

   SDL_PauseAudioDevice(device, 0);
   return true;
}