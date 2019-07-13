/* User interface functions */
#ifndef INVADER_H_
#define INVADER_H_

#include "common.h"

void cfg_load();
void gui_render(struct nk_context *ctx);

struct nk_image render_framebuffer(const void *data, unsigned width, unsigned height, unsigned pitch, unsigned pixel_format);

#endif