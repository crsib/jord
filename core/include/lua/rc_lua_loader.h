#include "lua.hpp"

namespace lua
{
	int rc_load( lua_State* l, const char* name );
	int rc_run( lua_State* l, const char* name );

	int rc_lua_loader( lua_State* l );
}