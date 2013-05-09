#pragma once;

#include "lua.hpp"

namespace lua
{
	namespace io
	{
		int open_library( lua_State* l );
	}
}