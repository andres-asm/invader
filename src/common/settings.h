#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "toml++/toml.h"

extern toml::table settings;

void settings_init(std::string path);

template <class T>
class Setting
{
private:
   std::string name;
   T data;
   T min;
   T max;
   T step;
   T def;

   bool changed;

public:
   Setting<T>(std::string name, T data)
   {
      this->name = name;
      this->data = data;
   };

   Setting<T>(std::string name, T data, T def)
   {
      this->name = name;
      this->data = data;
      this->def = def;
   };

   Setting<T>(std::string name, T data, T min, T max, T step, T def)
   {
      this->name = name;
      this->data = data;
      this->min = min;
      this->min = max;
      this->min = step;
      this->def = def;
   };

   T GetValue() { return data; }
   T* GetPtr() { return &data; }
   void SetChanged() { changed = true; }

   std::string GetName() { return name; }
};

extern Setting<bool>* video_fullscreen;

#endif