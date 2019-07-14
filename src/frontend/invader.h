/* User interface functions */
#ifndef INVADER_H_
#define INVADER_H_

#include "common.h"
#include "libretro/piccolo.h"

void cfg_load();
void gui_render(struct nk_context *ctx);

struct nk_image render_framebuffer(const core_frame_buffer_t *data, unsigned pixel_format);
size_t render_audio(const int16_t *data, size_t frames);

#endif