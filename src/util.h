#ifndef UTIL_H_
#define UTIL_H_

enum logger_level
{
   LOG_DEBUG = 0,
   LOG_INFO,
   LOG_WARN,
   LOG_ERROR
};

void logger(int level, const char *tag, const char *fmt, ...);

#endif