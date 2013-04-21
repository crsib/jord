#include "rc/rc.h"
#include "core.h"

#include <iostream>

int main( int argc, char** argv )
{
	INIT_RESOURCE( admin );

	lua_State* l = core_startvm();

	if( lua::rc_load( l, ":/lua/admin/entry.lua" ) )
		std::cout << lua_tostring( l, -1 );

	for( int i = 0; i <= argc; ++i )
		lua_pushstring( l, argv[i] );

	if( lua_pcall( l, argc + 1, LUA_MULTRET, 0 ) )
		std::cout << lua_tostring( l, -1 );

	lua_close( l );

	system( "pause" );
	return 0;
}