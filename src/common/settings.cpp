#include <fstream>
#include <iostream>

#include "settings.h"
#include "util.h"

static const char* tag = "[settings]";

toml::table settings;

void settings_init(const char* path)
{
   settings = toml::parse_file(path);
}
auto settings_get_value(const char* name)
{
   auto setting = settings[name]["value"];
   std::cout << "setting is: " << setting << "\n";
}

void test()
{
   auto fullscreen = settings["fullscreen"]["value"];
   std::cout << "fullscreen is a: " << fullscreen << "\n";
}