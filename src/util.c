#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <libgen.h>
#include <sys/stat.h>

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