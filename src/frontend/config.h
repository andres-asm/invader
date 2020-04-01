/* Configuration functions and variables */

#ifndef CONFIG_H
#define CONFIG_H

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#include <stdbool.h>
#include <stddef.h>

#include <libintl.h>
#include <locale.h>

#define __(str) gettext(str)

struct setting
{
   void*    data;
   char     name[50];
   unsigned type;
   size_t   size;
   char     label[100];
   char     help[100];

} typedef setting;

struct config_main
{
   setting directory_cores;
   setting directory_games;
   setting video_vsync;
   setting video_fullscreen;
};

struct config
{
   struct config_main config_main;
}
typedef config;

/* initialize configuration */
void config_init();

/* load configuration file */
bool config_load(char* file);

/* get a setting */
setting* setting_get(char* s);

/* get an unsigner setting value */
unsigned* setting_uint_val(char* s);

/* get an boolean setting value */
bool* setting_bool_val(char* s);

/* get an float setting value */
float* setting_float_val(char* s);

/* get an string setting value */
char* setting_string_val(char* s);

/* get a setting label */
const char* setting_get_label(setting* setting);

/* get a setting description */
const char* setting_get_desc(setting* setting);

#endif