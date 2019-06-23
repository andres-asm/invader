/* Configuration functions and variables */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>

struct setting
{
   void*    data;
   char     name[50];
   unsigned type;
   size_t   size;
   char     desc[100];

} typedef setting;

struct config_main
{
   setting directory_cores;
   setting directory_games;
};

struct config
{
   struct config_main config_main;
}
typedef config;

config *cfg;

/* initialize configuration */
void config_init();

/* load configuration file */
bool config_load(char* file);

/* get a setting */
setting* setting_get(char* s);

/* get an unsigner setting value */
unsigned* setting_get_uint(char* s);

/* get an boolean setting value */
bool* setting_get_bool(char* s);

/* get an float setting value */
float* setting_get_float(char* s);

/* get an string setting value */
char* setting_get_string(char* s);

#endif