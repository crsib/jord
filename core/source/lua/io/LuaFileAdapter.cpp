#include "lua/io/LuaFileAdapter.h"

namespace lua
{
	namespace io
	{
		char* LuaFileAdapter::class_name = "LuaFileAdapter";

		RegisterCustomClass<LuaFileAdapter>::RegType LuaFileAdapter::methods[] = {
			{ "read", &LuaFileAdapter::read },
			{ "write", &LuaFileAdapter::write },
			{ "flush", &LuaFileAdapter::flush },
			{ "close", &LuaFileAdapter::close },
			{ "lines", &LuaFileAdapter::lines },
			{ "seek", &LuaFileAdapter::seek },
			{ "setvbuf", &LuaFileAdapter::setvbuf },
			{ "is_open", &LuaFileAdapter::isOpen },
			{ NULL, NULL }
		};


		RegisterCustomClass<LuaFileAdapter>::RegType LuaFileAdapter::meta_methods[] = {
			{ "__tostring", &LuaFileAdapter::__tostring },
			{ NULL, NULL }
		};


		LuaFileAdapter::LuaFileAdapter( LuaAbstractFile* handle )
			: m_FileHandle( handle )
		{
			if( handle && handle->isOpen() && !handle->isStdFile() )
				opened_files++;
		}

		LuaFileAdapter::~LuaFileAdapter()
		{

			if( m_FileHandle )
			{
				if( m_FileHandle->close() )
					opened_files--;

				if( !m_FileHandle->isStdFile() )
					delete m_FileHandle;
			}
		}

		int LuaFileAdapter::read( lua_State* l )
		{
			if( !m_FileHandle || !m_FileHandle->isOpen() )
				return luaL_error( l, "attempt to read a closed file" );

			int  num_args = lua_gettop( l );
			bool success = true;
			int  num_ret = 0;

			std::vector<char> data;

			if( 0 == num_args )
			{
				std::string line;
				success = m_FileHandle->readLine( line );
				lua_pushstring( l, line.c_str() );
				++num_ret;
			}
			else
			{
				luaL_checkstack(l, num_args + LUA_MINSTACK, "too many arguments");
				for( num_ret = 1; num_args-- && success;  )
				{
					if( lua_isnumber( l, num_ret ) ) // Integral is passed
					{
						size_t num_bytes = size_t( lua_tonumber( l, num_ret ) );

						if( !num_bytes ) // Test for eof
						{
							if( m_FileHandle->isEOF() )
							{
								lua_pushnil( l );
								success = false;
							}
							else
								lua_pushstring( l, "" );
						}
						else
						{
							data.clear();
							success = m_FileHandle->read( data, num_bytes );

							if( success )
								lua_pushlstring( l, &data.front(), data.size() );
							else
								lua_pushnil( l );
						}
					} // if( lua_isnumber( l, num_ret ) ) 
					else
					{
						const char *p = lua_tostring(l, num_ret);
						luaL_argcheck(l, p && p[0] == '*', num_ret, "invalid option");
						switch (p[1]) 
						{
						case 'n':  
							{
								lua_Number n; 
								success = m_FileHandle->readNumber( n );
								lua_pushnumber( l, n );
							}
							break;
						case 'l':  
							{
								std::string line;
								success = m_FileHandle->readLine( line );
								lua_pushstring( l, line.c_str() );
							}
							break;
						case 'a':  
							{
								data.clear();
								m_FileHandle->read( data, ~((size_t)0));
								if( !data.empty() )
									lua_pushlstring( l, &data.front(), data.size() );
								else
									lua_pushlstring( l, NULL, 0 );
							}
							break;
						default:
							return luaL_argerror(l, num_ret, "invalid format");
						} // switch (p[1]) 
					}// !if( lua_isnumber( l, num_ret ) ) 
					++num_ret;
				}
				--num_ret;
			}

			if( m_FileHandle->hasFailed() )
				return reportError( l, "read failure" );

			if( !success )
			{
				lua_pop( l, 1 );
				lua_pushnil( l );
			}

			return num_ret;
		}

		int LuaFileAdapter::write( lua_State* l )
		{
			if( !m_FileHandle )
				return luaL_error( l, "attempt to write a closed file" );

			int  nargs = lua_gettop(l);
			int  arg = 1;
			bool status = true;

			for (; nargs--; arg++) 
			{
				if (lua_type(l, arg) == LUA_TNUMBER)
					status = status && m_FileHandle->writeNumber( lua_tonumber( l, arg ) );
				else 
				{
					size_t length;
					const char *s = luaL_checklstring(l, arg, &length);
					status = status && m_FileHandle->writeBuffer( s, length );
				}
			}
			return reportResult( l, status );
		}

		int LuaFileAdapter::flush( lua_State* l )
		{
			if( !m_FileHandle )
				return luaL_error( l, "attempt to flush a closed file" );
			return reportResult( l, m_FileHandle->flush() );
		}

		int LuaFileAdapter::close( lua_State* l )
		{
			if( !m_FileHandle )
				luaL_error( l, "file is already closed" );

			bool result;
			if( (result = m_FileHandle->close()) )
				opened_files--;
			else
				luaL_error( l, "file is already closed" );

			if( !m_FileHandle->isStdFile() )
				delete m_FileHandle;

			m_FileHandle = NULL;
			return reportResult( l, result );
		}

		int LuaFileAdapter::lines( lua_State* l )
		{
			if( !m_FileHandle || !m_FileHandle->isOpen() )
				return luaL_error( l, "attempt to read a closed file" );

			bool close_file = true;

			if( !lua_isuserdata( l, -1 ) )
			{
				lua_pushlightuserdata( l, this );
				close_file = false;
			}

			lua_pushboolean( l, close_file );
			lua_pushcclosure( l, lines_iterator, 2 );

			return 1;
		}

		int LuaFileAdapter::lines_iterator( lua_State* l )
		{
			LuaFileAdapter* adapter = reinterpret_cast<LuaFileAdapter*>(lua_touserdata( l, lua_upvalueindex( 1 ) ) );

			std::string line;
			if( !adapter->m_FileHandle->isOpen() )
				luaL_error( l, "file is closed" );

			if( adapter->m_FileHandle->readLine( line ) )
 				lua_pushstring( l, line.c_str() );
			else
			{
				if( lua_toboolean( l, lua_upvalueindex( 2 ) ) )
					adapter->m_FileHandle->close();

				lua_pushnil( l );
			}

			return 1;
		}

		int LuaFileAdapter::seek( lua_State* l )
		{
			if( !m_FileHandle )
				return luaL_error( l, "attempt to seek a closed file" );

			static const LuaAbstractFile::SeekMode __modes[] = { LuaAbstractFile::Set, LuaAbstractFile::Cur, LuaAbstractFile::End };
			static const char *const __modenames[] = {"set", "cur", "end", NULL};
			int op = luaL_checkoption(l, 1, "cur", __modenames);
			long offset = luaL_optlong(l, 2, 0);
			long position;
			bool result = m_FileHandle->seek( __modes[ op ], offset, position );

			if( !result )
				return reportError( l, "seek failed" );

			lua_pushinteger( l, position );
			return 1;
		}

		int LuaFileAdapter::setvbuf( lua_State* l )
		{
			if( !m_FileHandle )
				return luaL_error( l, "attempt to setvbuf to a closed file" );

			static const LuaAbstractFile::VBufMode __mode[] = { LuaAbstractFile::No, LuaAbstractFile::Full, LuaAbstractFile::Line };
			static const char *const __modenames[] = {"no", "full", "line", NULL};

			int op = luaL_checkoption(l, 1, NULL, __modenames);
			lua_Integer sz = luaL_optinteger(l, 2, LUAL_BUFFERSIZE);
			return reportResult( l, m_FileHandle->setvbuf(__mode[op], sz ) );
		}

		int LuaFileAdapter::__tostring(  lua_State* l  ) 
		{
			lua_pushstring( l, m_FileHandle ? m_FileHandle->getFileName().c_str() : "file (closed)" );
			return 1;
		}

		int LuaFileAdapter::reportError( lua_State* l, const std::string& message )
		{
			std::string msg = m_FileHandle ? (m_FileHandle->getFileName() + " : " + message + " : " + m_FileHandle->getLastError() ) : message;
			int err = m_FileHandle ? m_FileHandle->getLastErrorCode() : -1;

			lua_pushnil( l );
			lua_pushstring( l, msg.c_str() );
			lua_pushnumber( l, err );
			return 3;
		}

		int LuaFileAdapter::reportResult( lua_State* l, bool successful )
		{
			if( successful )
			{
				lua_pushboolean( l, successful );
				return 1;
			}

			else
				return reportError( l, m_FileHandle ? m_FileHandle->getLastError() : "io error");
		}

		int LuaFileAdapter::isOpen(  lua_State* l  ) 
		{
			lua_pushboolean( l, !!m_FileHandle );
			return 1;
		}

		LuaAbstractFile* LuaFileAdapter::getFileHandle() const
		{
			return m_FileHandle;
		}

		int LuaFileAdapter::opened_files = 0;

	}
}
