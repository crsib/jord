#pragma once

#include <lua.hpp>

#define CreateUDataClass( l, type, ... ) new( lua::RegisterCustomClass<type>::NewUData( l ) ) type( __VA_ARGS__ );

namespace lua
{
	template<class T>
	class RegisterCustomClass
	{
	public:
		struct RegType 
		{
			const char *Name;
			int(T::*Func)(lua_State*);
		};

		static void  Register( lua_State* l )
		{
			luaL_newmetatable( l, T::class_name );

			lua_pushstring( l, "__gc" );
			lua_pushcfunction( l, &RegisterCustomClass<T>::GC );
			lua_settable( l, -3 );

			lua_pushstring( l, "__index" );
			lua_newtable( l ); // __index

			for (int i = 0; T::methods[i].Name; i++) 
			{
				lua_pushstring(l, T::methods[i].Name);
				lua_pushnumber(l, i);
				lua_pushboolean( l, false );
				lua_pushcclosure(l, &RegisterCustomClass<T>::Thunk, 2);
				lua_settable(l, -3);
			}

			lua_settable( l,-3 );

			for (int i = 0; T::meta_methods[i].Name; i++) 
			{
				lua_pushstring(l, T::meta_methods[i].Name);
				lua_pushnumber(l, i);
				lua_pushboolean( l, true );
				lua_pushcclosure(l, &RegisterCustomClass<T>::Thunk, 2);
				lua_settable(l, -3);
			}

			lua_pop(l, 1);
		}

		static void* NewUData( lua_State* l )
		{
			void * ud = lua_newuserdata(l, sizeof(T));
			luaL_getmetatable(l, T::class_name);
			lua_setmetatable(l, -2);
			return ud;
		}
	private:
		static int   GC( lua_State* l )
		{
			T* obj = static_cast<T*>(luaL_checkudata(l, -1, T::class_name));
			if(obj)
				obj->~T(); // Lua is in charge of memory management
			return 0;
		}
	
		static int   Thunk(lua_State *L)
		{
			int i = int( lua_tonumber( L, lua_upvalueindex(1) ) );
			bool is_meta = bool( lua_toboolean( L, lua_upvalueindex( 2 ) ) != 0 );

			T* obj = static_cast<T*>(luaL_checkudata(L, 1, T::class_name));
			lua_remove(L, 1);
			return is_meta ? (obj->*(T::meta_methods[i].Func))(L) : (obj->*(T::methods[i].Func))(L);
		}
	};
}