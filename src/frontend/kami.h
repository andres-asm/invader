/* User interface functions */
#ifndef INVADER_H_
#define INVADER_H_

#include "common.h"
#include "libretro/piccolo.h"

#include <compat/strl.h>
#include <string/stdstring.h>
#include <lists/string_list.h>


bool core_list_init(const char* in);

int render_framebuffer(const core_frame_buffer_t *data, unsigned pixel_format);
size_t render_audio(const int16_t *data, size_t frames);

void core_options_update(core_option_t* option, const char* value);

unsigned core_option_get_index(core_option_t* option, struct string_list* values);

struct string_list* core_option_get_values(core_option_t* option);

core_frame_buffer_t frame_buffer;

core_info_t core_info_list[100];
core_option_t core_options[1000];

core_info_t current_core_info;
unsigned core_count;
unsigned current_core;

#endif