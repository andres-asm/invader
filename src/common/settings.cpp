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