#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "toml++/toml.h"

extern toml::table settings;

void settings_init(const char* path);

template <class T>
class Setting
{
private:
   const char* name;
   T data;
   T min;
   T max;
   T step;
   T def;

public:
   Setting<T>(const char* name, T data)
   {
      this->name = strdup(name);
      this->data = data;
   };

   Setting<T>(const char* name, T data, T def)
   {
      this->name = strdup(name);
      this->data = data;
      this->def = def;
   };

   Setting<T>(const char* name, T data, T min, T max, T step, T def)
   {
      this->name = strdup(name);
      this->data = data;
      this->min = min;
      this->min = max;
      this->min = step;
      this->def = def;
   };

   T GetValue() { return data; }
   T* GetPtr() { return &data; }

   const char* GetName() { return name; }
};

extern Setting<bool>* video_fullscreen;

#endif