#pragma once;

#include <string>
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/object.hpp>

#include "lua/io/LuaAbstractFile.h"
#include "lua/RegisterCustomClass.h"

namespace lua
{
	namespace io
	{
		class LuaFileAdapter
		{
		public:
			explicit LuaFileAdapter( LuaAbstractFile* handle );
			~LuaFileAdapter();

			int         read( lua_State* l );

			int         write( lua_State* l );

			int         flush( lua_State* l );
			int         close( lua_State* l );

			int         lines( lua_State* l );
			static int  lines_iterator( lua_State* l );

			int         seek( lua_State* l );
			int         setvbuf( lua_State* l );

			int         __tostring(  lua_State* l );
			int         isOpen(  lua_State* l  );

			int         reportError( lua_State* l, const std::string& message );
			int         reportResult( lua_State* l, bool successful );

			LuaAbstractFile* getFileHandle() const;

			static char* class_name;
			static RegisterCustomClass<LuaFileAdapter>::RegType methods[];
			static RegisterCustomClass<LuaFileAdapter>::RegType meta_methods[];

			static int opened_files;
		private:
			LuaAbstractFile*       m_FileHandle;
		};
	}
}