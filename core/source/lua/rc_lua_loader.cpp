#include "lua/rc_lua_loader.h"
#include "rc/rc.h"

#include <string>
#include <algorithm>

static int readable (const char *filename) 
{
	rc::NodePtr node = rc::find_node( filename );
	return node && dynamic_cast<rc::File*>( node.get() ) ? 1 : 0;
}


static const char *pushnexttemplate (lua_State *L, const char *path) 
{
	const char *l;
	while (*path == *LUA_PATHSEP) path++;  /* skip separators */
	if (*path == '\0') return NULL;  /* no more templates */
	l = strchr(path, *LUA_PATHSEP);  /* find next separator */
	if (l == NULL) 
		l = path + strlen(path);
	lua_pushlstring(L, path, l - path);  /* template */
	return l;
}

static const char *findfile (lua_State *L, const char *name,  const char *pname) 
{
	const char *path;
	name = luaL_gsub(L, name, ".", "/");
	lua_getfield(L, LUA_ENVIRONINDEX, pname);
	path = lua_tostring(L, -1);
	if (path == NULL)
		luaL_error(L, LUA_QL("package.%s") " must be a string", pname);
	lua_pushliteral(L, "");  /* error accumulator */
	while ((path = pushnexttemplate(L, path)) != NULL) 
	{
		const char *filename;
		filename = luaL_gsub(L, lua_tostring(L, -1), LUA_PATH_MARK, name);
		lua_remove(L, -2);  /* remove path template */
		if (readable(filename))  /* does file exist and is readable? */
			return filename;  /* return that file name */
		lua_pushfstring(L, "\n\tno file " LUA_QS, filename);
		lua_remove(L, -2);  /* remove file name */
		lua_concat(L, 2);  /* add entry to possible error message */
	}
	return NULL;  /* not found */
}

int lua::rc_lua_loader( lua_State* l )
{
	std::string name = luaL_checkstring( l, 1 );
	const char* path = findfile( l, name.c_str(), "rcpath" );

	if( !path )
	{
		lua_pushnil( l );
		return 1;
	}

	if( rc_load( l, path ) )
		luaL_error(l, "error loading module " LUA_QS " from file " LUA_QS ":\n\t%s", lua_tostring(l, 1), path, lua_tostring( l, -1 ) );
	return 1;
}

int lua::rc_load( lua_State* l, const char* name )
{
	rc::NodePtr node = rc::find_node( name );

	void const* addr = rc::get_file_pointer( node );

	if( !addr )
	{
		lua_pushfstring( l, "Failed to find " LUA_QS, name );
		return LUA_ERRFILE;
	}	
	size_t size      = rc::get_file_size( node );

	return luaL_loadbuffer( l, reinterpret_cast<const char*>(addr), size, name );
}

int lua::rc_run( lua_State* l, const char* name )
{
	return rc_load( l, name ) || lua_pcall( l, 0, LUA_MULTRET, 0 ); 
}

