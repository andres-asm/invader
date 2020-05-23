#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "toml++/toml.h"

extern toml::table settings;

void settings_init(const char* path);
auto settings_get_value(const char* name);

#endif