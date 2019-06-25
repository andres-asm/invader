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

#include "invader.h"
#include "config.h"
#include "nuklear_extra.h"

bool nk_checkbox_bool(struct nk_context* ctx, const char* label, bool *active)
{
   int    x = *active;
   bool ret = nk_checkbox_label(ctx, label, &x);
   *active  = x;

   return ret;
}
