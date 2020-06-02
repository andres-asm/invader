#include <fstream>
#include <iostream>

#include "settings.h"
#include "util.h"

static const char* tag = "[settings]";

toml::table settings;

Setting<bool>* video_fullscreen;

void settings_init(std::string path)
{
   // settings = toml::parse_file(path);
   video_fullscreen = new Setting<bool>("video_fullscreen", false, false);
}