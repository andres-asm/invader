#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "toml++/toml.h"

extern toml::table settings;

void settings_init(const char* path);

template <class T>
class Setting
{
   const char* name;
   T data;
   T min;
   T max;
   T step;
   T def;

public:
   Setting<T>(const char* name, T data)
   {
      this->name = name;
      this->data = data;
   };
   T GetValue() { return data; }
};

#endif