#pragma once;

#include <string>
#include <vector>
#include <lua.hpp>

namespace lua
{
	namespace io
	{
		class LuaFilesystemProvider;
		class LuaFileAdapter;

		class LuaIOManager
		{
			LuaIOManager( const LuaIOManager& );
			LuaIOManager& operator = ( const LuaIOManager& );

			LuaIOManager();
			~LuaIOManager();
		public:
			static LuaIOManager&    GetInstance();
			void   setupStdFiles( lua_State* l );

			enum
			{
				LocalFilesystem,
				RCFilesystem,
				ProcessFilesystem,
				UserFilesystem
			};

			// Wrapped lua functionality
			// From the default IO module
			static int  close( lua_State* l );
			static int  flush( lua_State* l );
			static int  input( lua_State* l );
			static int  output( lua_State* l );
			static int  lines( lua_State* l );
			static int  open( lua_State* l );
			static int  popen( lua_State* l );
			static int  read( lua_State* l );
			static int  tmpfile( lua_State* l );
			static int  type( lua_State* l );
			static int  write( lua_State* l );

		private:
			std::vector< LuaFilesystemProvider* > m_Providers;
			LuaFileAdapter*                      m_StdInput;
			LuaFileAdapter*                      m_StdOutput;
			LuaFileAdapter*                      m_StdError;

			static LuaFileAdapter* GetFilePointer( lua_State* l, bool input );
		};
	}
}