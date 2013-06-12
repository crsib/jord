#include "lua/io/LuaDirectoryAdapter.h"

namespace lua
{
	namespace io
	{
		RegisterCustomClass<LuaDirectoryAdapter>::RegType LuaDirectoryAdapter::meta_methods[] = {
			{ "__tostring", &LuaDirectoryAdapter::__tostring },
			{ NULL, NULL }
		};

		RegisterCustomClass<LuaDirectoryAdapter>::RegType LuaDirectoryAdapter::methods[] = {
			{ "read", &LuaDirectoryAdapter::entries },
			{ NULL, NULL }
		};

		char* LuaDirectoryAdapter::class_name = "LuaDirectoryAdapter";

		LuaDirectoryAdapter::LuaDirectoryAdapter( LuaAbstractDirectory* handle )
			: m_Handle( handle )
		{
		}

		LuaDirectoryAdapter::~LuaDirectoryAdapter()
		{
			delete m_Handle;
		}

		int LuaDirectoryAdapter::entries( lua_State* l )
		{
			if( !m_Handle )
				return luaL_error( l, "Invalid directory" );

			if( !lua_isuserdata( l, -1 ) )
				lua_pushlightuserdata( l, this );

			lua_pushboolean( l, true );

			lua_pushcclosure( l, directory_iterator, 2 );

			return 1;
		}

		int LuaDirectoryAdapter::directory_iterator( lua_State* l )
		{

			LuaDirectoryAdapter* adapter = reinterpret_cast<LuaDirectoryAdapter*>(lua_touserdata( l, lua_upvalueindex( 1 ) ) );

			if( !adapter->m_Handle )
				return luaL_error( l, "Invalid directory" );

			bool first = lua_toboolean( l, lua_upvalueindex( 2 ) );
			
			std::string path = first ? adapter->m_Handle->getFirstEntry() : adapter->m_Handle->getNextEntry();

			if( first )
			{
				lua_pushboolean( l, false );
				lua_replace( l, lua_upvalueindex( 2 ) );
			}

			if( path.length() > 0 )
				lua_pushstring( l, path.c_str() );
			else
				lua_pushnil( l );

			return 1;
		}

		int LuaDirectoryAdapter::__tostring( lua_State* l )
		{
			if( !m_Handle )
				return luaL_error( l, "Invalid directory" );

			lua_pushstring( l, m_Handle->path().c_str() );
			return 1;
		}
	}
}