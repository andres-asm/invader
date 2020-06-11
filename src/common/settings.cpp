#include <fstream>
#include <iostream>

#include "settings.h"
#include "util.h"

static const char* tag = "[settings]";

toml::table settings;

scale_mode_t scale_modes[] = {
   {"scale_mode_off", SCALE_MODE_OFF},
   {"scale_mode_full", SCALE_MODE_FULL},
   {"scale_mode_integer", SCALE_MODE_INTEGER},
   {"scale_mode_integer_overscale", SCALE_MODE_INTEGER_OVERSCALE}};

Setting<bool>* video_fullscreen;
Setting<bool>* video_fullscreen_windowed;
Setting<bool>* video_vsync;
Setting<scale_mode_t>* video_scale_mode;

void settings_init(std::string path)
{
   // settings = toml::parse_file(path);
   video_fullscreen = new Setting<bool>("video_fullscreen", false, false);
   video_fullscreen_windowed = new Setting<bool>("video_fullscreen_windowed", true, true);
   video_vsync = new Setting<bool>("video_vsync", true, true);
   video_scale_mode =
      new Setting<scale_mode_t>("video_scale_mode", scale_modes[SCALE_MODE_INTEGER], scale_modes[SCALE_MODE_INTEGER]);
}