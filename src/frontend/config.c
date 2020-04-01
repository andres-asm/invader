#include "config.h"
#include "ini.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <compat/strl.h>
#include <string/stdstring.h>

#include <libintl.h>
#include <locale.h>

#include "util.h"

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

setting *setting_array;
size_t setting_array_size;

config *cfg;

static const char* tag = "[config]";

enum setting_type
{
   SETTING_UINT,
   SETTING_INT,
   SETTING_FLOAT,
   SETTING_STRING,
   SETTING_BOOL
};

const char* setting_category_labels[] =
{
   "setting_categories_none",
   "setting_categories_general",
   "setting_categories_video",
   "setting_categories_audio",
   "setting_categories_input",
   "setting_categories_paths",
};

enum setting_flags
{
   FLAG_NONE           = 0,
   FLAG_ALLOW_EMPTY    = (1 << 0),
   FLAG_HAS_RANGE      = (1 << 1),
   FLAG_ALLOW_INPUT    = (1 << 2),
   FLAG_IMMEDIATE      = (1 << 3),
   FLAG_ADVANCED       = (1 << 4)
};

enum setting_categories
{
   CAT_NONE     = 0,
   CAT_GENERAL  = 1,
   CAT_VIDEO    = 2,
   CAT_AUDIO    = 3,
   CAT_INPUT    = 4,
   CAT_PATHS    = 5
};

void setting_init_string(setting *s, char* name)
{
   s->type = SETTING_STRING;
   s->size = 256;
   s->data = (char *)calloc(s->size, sizeof(char));
   strlcpy(s->name, name, sizeof(s->name));
}

void setting_init_bool(setting *s, char* name)
{
   s->type = SETTING_BOOL;
   s->size = sizeof(bool);
   s->data = (char *)calloc(s->size, sizeof(bool));
   strlcpy(s->name, name, sizeof(s->name));
}

void config_init()
{
   cfg = (config*)calloc(1, sizeof(config));

   setting_array_size = (
      sizeof(struct config_main) +
      0) / sizeof(setting);
   setting_array = (setting *)calloc(setting_array_size, sizeof(setting));

   logger(LOG_INFO, tag, "settings found: %d\n", setting_array_size);

   setting *s;
   int index = 0;

   /* config_main */
   s = &cfg->config_main.directory_cores;
   setting_init_string(s, "directory_cores");
   setting_array[index++] = *s;

   s = &cfg->config_main.directory_cores;
   setting_init_string(s, "directory_games");
   setting_array[index++] = *s;

   s = &cfg->config_main.video_vsync;
   setting_init_bool(s, "video_vsync");
   setting_array[index++] = *s;

   s = &cfg->config_main.video_fullscreen;
   setting_init_bool(s, "video_fullscreen");
   setting_array[index++] = *s;
}

static int config_load_handler(void* c, const char* section,
   const char* name, const char* value)
{
   if (string_is_empty(section))
   {
      setting *s = setting_get((char *)name);
      logger(LOG_DEBUG, tag, "settings %s label: %s\n", s->name, setting_get_label(s));
      logger(LOG_DEBUG, tag, "settings %s description: %s\n", s->name, setting_get_desc(s));

      switch(s->type)
      {
         case SETTING_INT:
         case SETTING_UINT:
            *((unsigned*)s->data) = atoi(value);
            logger(LOG_DEBUG, tag, "settings %s value: %d\n", s->name, *((unsigned*)s->data));
            break;
         case SETTING_FLOAT:
            *((float*)s->data) = atof(value);
            logger(LOG_DEBUG, tag, "settings %s value: %d\n", s->name, *((float*)s->data));
            break;
         case SETTING_BOOL:
            if (!strcmp(value, "true"))
               *((bool*)s->data) = true;
            else
               *((bool*)s->data) = false;
            logger(LOG_DEBUG, tag, "settings %s value: %s\n", s->name, *((bool*)s->data) ? "true" : "false");
            break;
         case SETTING_STRING:
            strlcpy(s->data, value, s->size);
            logger(LOG_INFO, tag,  "settings %s value: %s size: %d\n", s->name, s->data, s->size);
            break;
         default:
            logger(LOG_DEBUG, tag, "settings %s unknown\n", s->name);
      }
   }
   else if(string_is_equal(section, "categories"))
   {
      setting *s = setting_get((char *)name);
      s->categories = atoi(value);
      logger(LOG_DEBUG, tag, "setting %s category: %s\n", s->name, category_label(s->categories));

   }
   else
      logger(LOG_DEBUG, tag, "category %s unknown\n", name);
   return true;
}

const char* setting_get_label(setting* s)
{
   char label[60];

   strlcpy(label, s->name, sizeof(label));
   strlcat(label, "_label", sizeof(label));

   return __(label);
}

const char* setting_get_desc(setting* s)
{
   char desc[60];

   strlcpy(desc, s->name, sizeof(desc));
   strlcat(desc, "_desc", sizeof(desc));

   return __(desc);
}

const char* category_label(unsigned category)
{
   return __(setting_category_labels[category]);
}

setting* setting_get(char* s)
{
   for (int i = 0; i < setting_array_size; i++)
   {
      if (!strcmp(setting_array[i].name, s))
         return &setting_array[i];
   }
}

unsigned* setting_uint_val(char* s)
{
   return ((unsigned*)setting_get(s)->data);
}

float* setting_float_val(char* s)
{
   return ((float*)setting_get(s)->data);
}

bool* setting_bool_val(char* s)
{
   return ((bool*)setting_get(s)->data);
}

char* setting_string_val(char* s)
{
   return ((char*)setting_get(s)->data);
}

bool config_load(char* file)
{
   setting *s;

   logger(LOG_INFO, tag, "loading config file: %s\n", file);
   if (ini_parse(file, config_load_handler, cfg) < 0)
   {
      logger(LOG_ERROR, tag, "error loading config file: %s\n", file);
      return false;
   }

   fflush(stdout);
   return true;
}