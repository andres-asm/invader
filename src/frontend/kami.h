/* User interface functions */
#ifndef KAMI_H_
#define KAMI_H_

#include <compat/strl.h>
#include <lists/string_list.h>
#include <string/stdstring.h>

#include "common.h"
#include "libretro/piccolo.h"
#include "util.h"

class Kami
{
public:
   Kami();
   void Run(const char* title);
   bool CoreListInit(const char* path);

private:
   PiccoloWrapper* piccolo;
   int current_core;
   int previous_core;
   bool active;
   int core_count;
   core_info_t* core_info;
   /*TODO: refactor this*/
   core_info_t core_list[100];
   const char* core_entries[100];
};

extern Kami* kami;
extern Kami* kami2;

extern core_frame_buffer_t frame_buffer;

bool kami_core_list_init(const char* in);

int kami_render_framebuffer(const core_frame_buffer_t* data, unsigned pixel_format);
size_t kami_render_audio(const int16_t* data, size_t frames);

void kami_core_option_update(core_option_t* option, const char* value);

unsigned kami_core_option_get_index(core_option_t* option, struct string_list* values);

struct string_list* kami_core_option_get_values(core_option_t* option);

bool kami_init_audio();

#endif