#ifndef UTIL_H_
#define UTIL_H_

#include <dirent.h>
#include <stdarg.h>

#include <file/file_path.h>
#include <lists/string_list.h>
#include <string/stdstring.h>

#define PATH_MAX_LENGTH 4096

#define PRINT_BOOLEAN(X) X == true ? "true" : "false"
#define PRINT_PIXFMT(X) \
   X == RETRO_PIXEL_FORMAT_0RGB1555 \
      ? "0RGB1555" \
      : X == RETRO_PIXEL_FORMAT_XRGB8888 ? "XRGB8888" : X == RETRO_PIXEL_FORMAT_RGB565 ? "RGB565" : "UNKNOWN"

#define ARRAY_SIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*_ARR)))

enum logger_level
{
   LOG_DEBUG = 0,
   LOG_INFO,
   LOG_WARN,
   LOG_ERROR
};

struct file_list
{
   unsigned file_count;
   char file_names[4096][PATH_MAX_LENGTH];
} typedef file_list_t;

void logger_set_level(unsigned level);

const char* logger_get_level_name(unsigned level);

void logger(int level, const char* tag, const char* fmt, ...);

void get_file_list(const char* in, file_list_t* out, const char* filter, bool include_dirs);

bool filename_supported(const char* filename, const char* extensions);

#endif