#ifndef UTIL_H_
#define UTIL_H_

#define PATH_MAX_LENGTH 4096

#define PRINT_BOOLEAN(X) X == true ? "true" : "false"

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
   char     file_names[100][PATH_MAX_LENGTH];
} typedef file_list_t;

void logger(int level, const char *tag, const char *fmt, ...);

void get_file_list(const char *in, file_list_t *out, const char *filter);

#endif