#include "lua/io/lua_io.h"

#include "lua/io/LuaFileAdapter.h"
#include "lua/io/LuaDirectoryAdapter.h"

#include "lua/io/LuaIOManager.h"

namespace lua
{
	namespace io
	{
		int open_library( lua_State* l )
		{
			// Create io table
			lua_newtable( l );

			static const luaL_Reg __io_functions[] = {
				{ "close", LuaIOManager::close },
				{ "flush", LuaIOManager::flush },
				{ "input", LuaIOManager::input },
				{ "lines", LuaIOManager::lines },
				{ "open", LuaIOManager::open },
				{ "output", LuaIOManager::output },
				{ "popen", LuaIOManager::read },
				{ "read", LuaIOManager::read },
				{ "tmpfile", LuaIOManager::tmpfile },
				{ "type", LuaIOManager::type },
				{ "write", LuaIOManager::write },
				{NULL, NULL}
			};

			const luaL_Reg *closure = __io_functions;
			for (; closure->func; closure++)
			{
				lua_pushstring( l, closure->name);
				lua_pushcclosure(l, closure->func, 0);
				lua_settable( l, -3 );
			}

			lua_setglobal( l, "io" );
		
			RegisterCustomClass< LuaFileAdapter >::Register( l );

			LuaIOManager::GetInstance().setupStdFiles( l );

			return 0;
		}
	}
}