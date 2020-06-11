#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <iostream>
#include <string>
#include <type_traits>

#include "toml++/toml.h"

extern toml::table settings;

void settings_init(std::string path);

enum scale_modes_enum
{
   SCALE_MODE_OFF = 0,
   SCALE_MODE_FULL,
   SCALE_MODE_INTEGER,
   SCALE_MODE_INTEGER_OVERSCALE,
   SCALE_MODE_LAST,
};

typedef struct scale_mode
{
   std::string m_name;
   int m_mode;
} scale_mode_t;

template <typename T>
class SettingBase
{
protected:
   T m_value{};
   T m_default{};
   std::string m_name{};

public:
   SettingBase(std::string name, T value, T def)
      : m_name(std::move(name))
      , m_value(std::move(value))
      , m_default(std::move(def))
   { }

   T GetValue() { return m_value; };
   T SetValue(T value) { m_value = value; };
   T SetToDefault() { m_value = m_default; };

   static T Render();
};

// generic settings
template <typename T, typename Enable = void>
class Setting: public SettingBase<T>
{
public:
   Setting(std::string name, T value, T def)
      : SettingBase<T>(std::move(name), std::move(value), std::move(def))
   { }
};

template <typename T>
class Setting<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>: public SettingBase<T>
{
private:
   T m_max{};
   T m_min{};
   T m_step{};

public:
   Setting(std::string name, T value, T def, T min, T max, T step)
      : SettingBase<T>(std::move(name), std::move(value), std::move(def))
      , m_min(std::move(min))
      , m_max(std::move(max))
      , m_step(std::move(step))
   { }
};

template <>
class Setting<bool>: public SettingBase<bool>
{
public:
   Setting(std::string name, bool value, bool def)
      : SettingBase<bool>(std::move(name), std::move(value), std::move(def))
   { }

   bool Render();
};

template <>
class Setting<scale_mode_t>: public SettingBase<scale_mode_t>
{
public:
   Setting(std::string name, scale_mode_t value, scale_mode_t def)
      : SettingBase<scale_mode_t>(std::move(name), std::move(value), std::move(def))
   { }

   bool Render();
};

extern scale_mode_t scale_modes[];

extern Setting<bool>* video_fullscreen;
extern Setting<bool>* video_fullscreen_windowed;
extern Setting<bool>* video_vsync;
extern Setting<scale_mode_t>* video_scale_mode;

#endif