#pragma once

#include "lua.hpp"
#include "lua/rc_lua_loader.h"

extern "C" int core_open( lua_State* state );
lua_State*     core_startvm();
