#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <libgen.h>
#include <sys/stat.h>
#include <dirent.h>
#include <compat/strl.h>
#include <file/file_path.h>
#include <retro_stat.h>

#include <compat/strl.h>
#include <string/stdstring.h>
#include <lists/string_list.h>

static const char* tag = "[util]";

static int log_level = 0;

void logger_set_level(unsigned level)
{
   log_level = level;
}

const char* logger_get_level_name(unsigned level)
{
   const char* levels[] = { "log_level_desc_debug", "log_level_desc_info", "log_level_desc_warning", "log_level_desc_error"};
   return levels[level];
}

void logger(int level, const char *tag, const char *fmt, ...)
{
   if (level >= log_level)
   {
      va_list va;
      char buffer[4096] = {0};
      static const char *level_char = "diwe";

      va_start(va, fmt);
      vsnprintf(buffer, sizeof(buffer), fmt, va);
      va_end(va);

      fprintf(stderr, "[%c] --- %s %s", level_char[level], tag, buffer);
      fflush(stderr);
   }
}

void get_file_list(const char *in, file_list_t *out, const char *filter, bool include_dirs)
{
   DIR *dir;
   struct dirent *entry;
   unsigned i = 0;

   out->file_count = 0;

   logger(LOG_DEBUG, tag, "reading directory %s\n", in);
   dir = opendir(in);
   if (dir != NULL)
   {
      while ((entry = readdir(dir)) != NULL)
      {
         if (!string_is_empty(filter))
         {
            if (strstr(entry->d_name, filter))
            {
               strlcpy(out->file_names[i], entry->d_name, sizeof(char) * PATH_MAX_LENGTH);
               i++;
               out->file_count++;
            }
         }
         else
         {
            if (!path_is_directory(entry->d_name))
            {
               strlcpy(out->file_names[i], entry->d_name, sizeof(char) * PATH_MAX_LENGTH);
               i++;
               out->file_count++;
            }
         }


         if (include_dirs)
         {
            if (path_is_directory(entry->d_name))
            {
               strlcpy(out->file_names[i], entry->d_name, sizeof(char) * PATH_MAX_LENGTH);
               i++;
               out->file_count++;
            }
         }

      }

      closedir(dir);
   }
}