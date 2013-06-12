#pragma once;

#include <string>
#include <lua.hpp>

#include "lua/io/LuaAbstractDirectory.h"
#include "lua/RegisterCustomClass.h"

namespace lua
{
	namespace io
	{
		class LuaDirectoryAdapter
		{
		public:
			explicit LuaDirectoryAdapter( LuaAbstractDirectory* handle );
			~LuaDirectoryAdapter();

			int        entries( lua_State* l );
			static int directory_iterator( lua_State* l );

			int         __tostring(  lua_State* l );

			static char* class_name;
			static RegisterCustomClass<LuaDirectoryAdapter>::RegType methods[];
			static RegisterCustomClass<LuaDirectoryAdapter>::RegType meta_methods[];

		private:
			LuaAbstractDirectory*    m_Handle;
		};
	}
}