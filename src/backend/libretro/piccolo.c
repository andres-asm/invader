#include "piccolo.h"
#include "util.h"

#define load_sym(V, S) do {\
   if (!((*(void**)&V) = dlsym(g_retro.handle, #S))) \
   logger("Failed to load symbol '" #S "'': %s", dlerror()); \
   } while (0)

