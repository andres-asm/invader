#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <compat/strl.h>
#include <lists/string_list.h>

#include "kami.h"
#include "config.h"
#include "util.h"

static const char* tag = "[invader]";

bool core_list_init(const char* in)
{
   char buf[PATH_MAX_LENGTH];
   file_list_t *list;
   list = (file_list_t *)calloc(1, sizeof(file_list_t));
#ifdef _WIN32
   get_file_list(in, list, ".dll");
#else
   get_file_list(in, list, ".so");
#endif
   logger(LOG_DEBUG, tag, "core count: %d\n", list->file_count);
   for (unsigned i = 0; i < list->file_count; i++)
   {
      strlcpy(core_info_list[i].file_name, list->file_names[i], sizeof(core_info_list[i].file_name));
      snprintf(buf, sizeof(buf), "%s/%s", in, list->file_names[i]);
      core_load(buf, &core_info_list[i], core_options, true);
      core_info_list[i].core_id = i;

#ifdef DEBUG
      logger(LOG_DEBUG, tag, "core name: %s\n", core_info_list[i].core_name);
      logger(LOG_DEBUG, tag, "core version: %s\n", core_info_list[i].core_version);
      logger(LOG_DEBUG, tag, "valid extensions: %s\n", core_info_list[i].extensions);
#endif
      core_count++;
   }

   return true;
}

void core_options_update(core_option_t* option, const char* value)
{
   strlcpy(option->value, value, sizeof(option->value));
   piccolo_core_options_update();
}

struct string_list* core_option_get_values(core_option_t* option)
{
   return string_split(option->values, "|");
}

unsigned core_option_get_index(core_option_t* option, struct string_list* values)
{
   unsigned index = 0;
   for (unsigned i = 0; i < values->size; i++)
   {
      if ((string_is_equal(values->elems[i].data, option->value)))
         index = i;
   }
   return index;
}