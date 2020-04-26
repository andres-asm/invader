#include <compat/strl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "piccolo.h"
#include "util.h"

static const char* tag = "[core]";

PiccoloController::~PiccoloController()
{ }

PiccoloController::PiccoloController(core_info_t* info)
{
   piccolo = new Piccolo(info);
}

void PiccoloController::core_deinit()
{
   delete piccolo;
}

bool PiccoloController::load_core(const char* in)
{
   piccolo->set_instance_ptr(piccolo);
   return piccolo->load_core(in, false);
}

bool PiccoloController::peek_core(const char* in)
{
   piccolo->set_instance_ptr(piccolo);
   return piccolo->load_core(in, true);
}

core_option_t* PiccoloController::get_options()
{
   return piccolo->get_options();
}

unsigned PiccoloController::get_option_count()
{
   return piccolo->get_option_count();
}

bool PiccoloController::load_game(const char* in)
{
   return piccolo->load_game(in);
}

core_info_t* PiccoloController::get_info()
{
   return piccolo->get_info();
}