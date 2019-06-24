#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <libgen.h>
#include <sys/stat.h>
#include <dirent.h>
#include <compat/strl.h>

#include "util.h"

void logger(int level, const char *tag, const char *fmt, ...)
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

void get_file_list(const char *in, file_list_t *out, const char *filter)
{
   DIR *dir;
   struct dirent *entry;
   static const char* tag = "[dir]";

   unsigned i = 0;

   out->file_count = 0;

   logger(LOG_DEBUG, tag, "reading directory %s\n", in);
   dir = opendir(in);
   if (dir != NULL)
   {
      while ((entry = readdir(dir)) != NULL)
      {
         if (strstr(entry->d_name, filter))
         {
            logger(LOG_INFO, tag, "%s\n", entry->d_name);
            strlcpy(out->file_names[i], entry->d_name, sizeof(char) * PATH_MAX_LENGTH);
            i++;
            out->file_count++;
         }
      }
      closedir(dir);
   }
}