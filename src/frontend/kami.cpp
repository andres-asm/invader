#include <assert.h>
#include <compat/strl.h>
#include <limits.h>
#include <lists/string_list.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kami.h"

core_frame_buffer_t frame_buffer;

core_info_t core_info_list[100];

unsigned core_count;
unsigned current_core;

static const char* tag = "[invader]";

SDL_AudioSpec want, have;
SDL_AudioDeviceID device;

bool kami_core_list_init(const char* in)
{
   char buf[PATH_MAX_LENGTH];
   file_list_t* list;
   list = (file_list_t*)calloc(1, sizeof(file_list_t));
#ifdef _WIN32
   get_file_list(in, list, ".dll", false);
#else
   get_file_list(in, list, ".so", false);
#endif

   logger(LOG_DEBUG, tag, "core count: %d\n", list->file_count);
   for (unsigned i = 0; i < list->file_count; i++)
   {
      PiccoloWrapper* controller = new PiccoloWrapper(&core_info_list[i]);
      snprintf(buf, sizeof(buf), "%s/%s", in, list->file_names[i]);
      controller->peek_core(buf);

      logger(LOG_DEBUG, tag, "file name: %s\n", core_info_list[i].file_name);
      logger(LOG_DEBUG, tag, "core name: %s\n", core_info_list[i].core_name);
      logger(LOG_DEBUG, tag, "core version: %s\n", core_info_list[i].core_version);
      logger(LOG_DEBUG, tag, "valid extensions: %s\n", core_info_list[i].extensions);

      core_count++;
   }

   return true;
}

void kami_core_option_update(core_option_t* option, const char* value)
{
   logger(LOG_INFO, tag, "changing option %s to %s\n", option->description, value);
   strlcpy(option->value, value, sizeof(option->value));
   // core_options_updated();
}

struct string_list* kami_core_option_get_values(core_option_t* option)
{
   return string_split(option->values, "|");
}

unsigned kami_core_option_get_index(core_option_t* option, struct string_list* values)
{
   unsigned index = 0;
   for (unsigned i = 0; i < values->size; i++)
   {
      if ((string_is_equal(values->elems[i].data, option->value)))
         index = i;
   }
   return index;
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
   } else
      logger(LOG_ERROR, tag, "opened audio device: %s\n", SDL_GetAudioDeviceName(0, 0));

   logger(
      LOG_INFO, tag, "have - frequency: %d format: f %d s %d be %d sz %d channels: %d samples: %d\n", have.freq,
      SDL_AUDIO_ISFLOAT(have.format), SDL_AUDIO_ISSIGNED(have.format), SDL_AUDIO_ISBIGENDIAN(have.format),
      SDL_AUDIO_BITSIZE(have.format), have.channels, have.samples);

   SDL_PauseAudioDevice(device, 0);
   return true;
}