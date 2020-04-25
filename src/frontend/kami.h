/* User interface functions */
#ifndef KAMI_H_
#define KAMI_H_

#include <compat/strl.h>
#include <lists/string_list.h>
#include <string/stdstring.h>

#include "common.h"
#include "libretro/piccolo.h"
#include "util.h"

extern core_frame_buffer_t frame_buffer;

extern core_info_t core_info_list[100];
extern core_option_t core_options[1000];

extern core_info_t current_core_info;
extern unsigned core_count;
extern unsigned current_core;

extern Piccolo* piccolo;

bool kami_core_list_init(const char* in);

int kami_render_framebuffer(const core_frame_buffer_t* data, unsigned pixel_format);
size_t kami_render_audio(const int16_t* data, size_t frames);

void kami_core_option_update(core_option_t* option, const char* value);

unsigned kami_core_option_get_index(core_option_t* option, struct string_list* values);

struct string_list* kami_core_option_get_values(core_option_t* option);

bool kami_init_audio();

#endif