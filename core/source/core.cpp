#include "core.h"
#include "rc/rc.h"
#include "lua/io/lua_io.h"

static const luaL_Reg __lualibs[] = {
	{ "",             luaopen_base   },
	{ LUA_TABLIBNAME, luaopen_table  },
	//{ LUA_IOLIBNAME,  luaopen_io     },
	{ LUA_OSLIBNAME,  luaopen_os     },
	{ LUA_STRLIBNAME, luaopen_string },
	{ LUA_MATHLIBNAME,luaopen_math   },
	{ LUA_DBLIBNAME,  luaopen_debug  },
	{ LUA_LOADLIBNAME,luaopen_package},
	{ "core",         core_open      },
	{ LUA_IOLIBNAME,  lua::io::open_library }
};

static const size_t   __libs_count = sizeof( __lualibs ) / sizeof( luaL_Reg );


int core_open( lua_State* l )
{
	INIT_RESOURCE( core );

	// Setup rc envinromet
	lua_pushstring( l, ":/lua/?.lua;:/?.lua");
	lua_setfield( l, LUA_ENVIRONINDEX, "rcpath" );
	// Add loader
	lua_getfield( l, LUA_GLOBALSINDEX, "package" );
	lua_getfield( l, -1, "loaders" );

	int count = luaL_getn( l, -1 );
	lua_pushcfunction( l, lua::rc_lua_loader );
	lua_rawseti( l, -2, count + 1 );

	lua_pop( l, 2 );

	// Load up base scripts
	rc::NodePtr node = rc::find_node(":/lua/base");
	rc::Directory* base_directory = dynamic_cast<rc::Directory*>( node.get() );
	if( base_directory )
	{
		rc::Directory::children_list_t files;
		base_directory->getChildren( files, []( const rc::NodePtr& node ) { return dynamic_cast<rc::File*>( node.get() ) != nullptr; }, true );

		for( size_t i = 0; i < files.size(); ++i )
		{
			std::string script_path = files[i]->getFullName();
			
			if( lua::rc_load( l, script_path.c_str() ) )
				;
		}
	}

	return 0;
}

lua_State* core_startvm()
{
	lua_State* l = lua_open();

	for( size_t i = 0; i < __libs_count; ++i )
	{
		const luaL_Reg* lib = __lualibs + i;
		lua_pushcfunction(l, lib->func);
		lua_pushstring(l, lib->name);
		lua_call(l, 1, 0);
	}

	return l;
}
