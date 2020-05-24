#include <fstream>
#include <iostream>

#include "settings.h"
#include "util.h"

static const char* tag = "[settings]";

toml::table settings;

Setting<bool>* video_fullscreen;

void settings_init(std::string path)
{
   settings = toml::parse_file(path);

   video_fullscreen = new Setting<bool>("video_fullscreen", false, false);

   logger(
      LOG_INFO, tag, "setting name: %s value: %d\n", video_fullscreen->GetName().c_str(), video_fullscreen->GetValue());
}