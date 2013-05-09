#include "lua/io/LuaIOManager.h"
#include "lua/io/LuaFileAdapter.h"

#include "lua/io/details/LuaBoostFilesystemProvider.h"
#include "lua/RegisterCustomClass.h"

namespace lua
{
	namespace io
	{


		LuaIOManager::LuaIOManager()
		{
			LuaFilesystemProvider* local_fs = new details::LuaBoostFilesystemProvider;
			m_StdInput  = NULL;
			m_StdOutput = NULL;
			m_StdError  = NULL;

			m_Providers.push_back( local_fs );
		}


		void LuaIOManager::setupStdFiles( lua_State* l )
		{
			m_StdInput = CreateUDataClass( l, LuaFileAdapter, m_Providers[ LocalFilesystem ]->getDefaultStdin() );
			lua_setfield( l, LUA_REGISTRYINDEX, "__JORD_STDIN");

			m_StdOutput = CreateUDataClass( l, LuaFileAdapter, m_Providers[ LocalFilesystem ]->getDefaultStdout() );
			lua_setfield( l, LUA_REGISTRYINDEX, "__JORD_STDOUT");

			m_StdError = CreateUDataClass( l, LuaFileAdapter, m_Providers[ LocalFilesystem ]->getDefaultStderr() );
			lua_setfield( l, LUA_REGISTRYINDEX, "__JORD_STDERR");

			lua_getglobal( l, "io" );

			lua_getfield( l, LUA_REGISTRYINDEX, "__JORD_STDIN");
			lua_setfield( l, -2, "stdin" );

			lua_getfield( l, LUA_REGISTRYINDEX, "__JORD_STDOUT");
			lua_setfield( l, -2, "stdout" );

			lua_getfield( l, LUA_REGISTRYINDEX, "__JORD_STDERR");
			lua_setfield( l, -2, "stderr" );

			lua_pop( l, 1);
		}


		LuaIOManager::~LuaIOManager()
		{
			for( size_t i = 0; i < m_Providers.size(); ++i )
				delete m_Providers[ i ];
		}

		LuaIOManager& LuaIOManager::GetInstance()
		{
			static LuaIOManager __; // This singleton will be force-instantiated when the io module is loaded
			return __;
		}

		int LuaIOManager::close( lua_State* l )
		{
			// GetFilePointer will never return NULL
			return GetFilePointer( l, false )->close( l );
		}

		int LuaIOManager::flush( lua_State* l )
		{
			return GetInstance().m_StdOutput->flush( l );
		}

		int LuaIOManager::input( lua_State* l )
		{
			if (!lua_isnoneornil(l, 1)) 
			{
				const char *filename = lua_tostring(l, 1);
				if (filename) 
				{
					LuaAbstractFile* pf = GetInstance().m_Providers[ LocalFilesystem ]->getFile( filename, "r" );
					if ( !pf )
						luaL_error( l, "could not acquire %s", filename);

					if( !pf->isOpen() )
					{
						std::string error = pf->getLastError();
						delete pf;
						luaL_error( l, error.c_str() );
					}

					GetInstance().m_StdInput = CreateUDataClass( l, LuaFileAdapter, pf );
				}
				else
				{
					LuaFileAdapter* adapter = reinterpret_cast<LuaFileAdapter*>( luaL_checkudata( l, 1, LuaFileAdapter::class_name ) ); 
					if( !adapter )
						luaL_error(l, "attempt to use a closed file"); // This will make a jump outside a function;
					lua_pushvalue(l, 1);
					GetInstance().m_StdInput = adapter;
				}

				lua_setfield( l, LUA_REGISTRYINDEX, "__JORD_STDIN" );
			}

			lua_getfield( l, LUA_REGISTRYINDEX, "__JORD_STDIN" );
			return 1;
		}

		int LuaIOManager::output( lua_State* l )
		{
			if (!lua_isnoneornil(l, 1)) 
			{
				const char *filename = lua_tostring(l, 1);
				if (filename) 
				{
					LuaAbstractFile* pf = GetInstance().m_Providers[ LocalFilesystem ]->getFile( filename, "w" );
					if ( !pf )
						luaL_error( l, "could not acquire %s", filename);

					if( !pf->isOpen() )
					{
						std::string error = pf->getLastError();
						delete pf;
						luaL_error( l, error.c_str() );
					}

					GetInstance().m_StdOutput = CreateUDataClass( l, LuaFileAdapter, pf );
				}
				else
				{
					LuaFileAdapter* adapter = reinterpret_cast<LuaFileAdapter*>( luaL_checkudata( l, 1, LuaFileAdapter::class_name ) ); 
					if( !adapter )
						luaL_error(l, "attempt to use a closed file"); // This will make a jump outside a function;
					lua_pushvalue(l, 1);
					GetInstance().m_StdOutput = adapter;
				}

				lua_setfield( l, LUA_REGISTRYINDEX, "__JORD_STDOUT" );
			}

			lua_getfield( l, LUA_REGISTRYINDEX, "__JORD_STDOUT" );
			return 1;
		}

		int LuaIOManager::lines( lua_State* l )
		{
			LuaFileAdapter* adapter;
			const char*     filename;

			if( lua_isnone( l, 1 ) )
				adapter = GetInstance().m_StdInput;
			else if( ( filename = lua_tostring( l, 1 ) ) )
			{
				LuaAbstractFile* pf = GetInstance().m_Providers[ LocalFilesystem ]->getFile( filename, "r" );
				if ( !pf )
					luaL_error( l, "could not acquire %s", filename);

				if( !pf->isOpen() )
				{
					std::string error = pf->getLastError();
					delete pf;
					luaL_error( l, error.c_str() );
				}

				adapter = CreateUDataClass( l, LuaFileAdapter, pf );
			}
			else
				luaL_error( l, "invalid parameter" );

			return adapter->lines( l );
		}

		int LuaIOManager::open( lua_State* l )
		{
			const char *filename = luaL_checkstring(l, 1);
			const char *mode = luaL_optstring(l, 2, "r");

			LuaAbstractFile* pf = NULL;
			
			if( mode[0] == 'r' )
			{
				for( size_t i = 0; i < GetInstance().m_Providers.size(); ++i )
				{
					if( GetInstance().m_Providers[ i ]->pathExists( filename ) )
					{
						pf = GetInstance().m_Providers[ i ]->getFile( filename, mode );
						break ;
					}
				}
			}

			if( !pf )
				pf = GetInstance().m_Providers[ LocalFilesystem ]->getFile( filename, mode );
			if ( !pf )
				luaL_error( l, "could not acquire %s", filename);

			LuaFileAdapter* adapter = CreateUDataClass( l, LuaFileAdapter, pf );

			if( !pf->isOpen() )
			{
				lua_pop( l, 1 );
				return adapter->reportError(l, "failed to open a file");
			}

			return 1;
		}

		int LuaIOManager::popen( lua_State* l )
		{
			return 0;
		}

		int LuaIOManager::read( lua_State* l )
		{
			return GetInstance().m_StdInput->read( l );
		}

		int LuaIOManager::tmpfile( lua_State* l )
		{
			LuaAbstractFile* pf = GetInstance().m_Providers[ LocalFilesystem ]->getTempFile( );
			if ( !pf )
				luaL_error( l, "could not acquire temp file" );

			LuaFileAdapter* adapter = CreateUDataClass( l, LuaFileAdapter, pf );
			return pf->isOpen() ? adapter->reportError(l, "failed to open a file") : 1;
		}

		int LuaIOManager::type( lua_State* l )
		{
			luaL_checkany( l, 1 );
			
			void* ud = lua_touserdata(l, 1);

			lua_getfield(l, LUA_REGISTRYINDEX, LuaFileAdapter::class_name );
			
			if (ud == NULL || !lua_getmetatable(l, 1) || !lua_rawequal(l, -2, -1))
				lua_pushnil(l);  /* not a file */
			else if( reinterpret_cast<LuaFileAdapter*>( ud )->getFileHandle() == NULL )
				lua_pushliteral(l, "closed file");
			else
				lua_pushliteral(l, "file");
			return 1;
		}

		int LuaIOManager::write( lua_State* l )
		{
			return GetInstance().m_StdOutput->write( l );
		}

		LuaFileAdapter* LuaIOManager::GetFilePointer( lua_State* l, bool input )
		{
			LuaIOManager& _this = GetInstance();
			if( lua_isnone( l, 1 ) )
				return input ? _this.m_StdInput : _this.m_StdOutput;

			LuaFileAdapter* adapter;

			if( lua_isuserdata( l, 1 ) )
			{
				adapter = reinterpret_cast<LuaFileAdapter*>( luaL_checkudata( l, 1, LuaFileAdapter::class_name ) ); 
				if( !adapter )
					luaL_error(l, "attempt to use a closed file"); // This will make a jump outside a function;
			}
			else
				luaL_error( l, "invalid parameter" );

			lua_remove( l, 1 ); // Pretend, that nothing was on the bottom of the stack
			// However, adapter will be possibly garbage collected, so open should never
			// be used when the pointer should be retained
			return adapter;
		}

	}
}