#ifndef COMMON_H_
#define COMMON_H_

// system
#include <libintl.h>
#include <locale.h>

#include "libretro/piccolo.h"
#include "settings.h"
#include "util.h"

#define _(str) gettext(str)

#define UNUSED(a) (void)a
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a) / sizeof(a)[0])

#endif
