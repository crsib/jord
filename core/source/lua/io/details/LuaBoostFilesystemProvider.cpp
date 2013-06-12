#include "lua/io/details/LuaBoostFilesystemProvider.h"
#include "lua/io/details/LuaBoostFile.h"
#include "lua/io/details/LuaBoostDirectory.h"

#include <boost/filesystem.hpp>

namespace lua
{
	namespace io
	{
		namespace details
		{


			bool LuaBoostFilesystemProvider::pathExists( const std::string& path ) const
			{
				return boost::filesystem::exists( path );
			}

			bool LuaBoostFilesystemProvider::isRegularFile( const std::string& path ) const
			{
				return boost::filesystem::is_regular_file( path );
			}

			bool LuaBoostFilesystemProvider::isDirectory( const std::string& path ) const
			{
				return boost::filesystem::is_directory( path );
			}

			bool LuaBoostFilesystemProvider::isSymlink( const std::string& path ) const
			{
				return boost::filesystem::is_symlink( path );
			}

			bool LuaBoostFilesystemProvider::isEmpty( const std::string& path ) const
			{
				return boost::filesystem::is_empty( path );
			}

			LuaAbstractFile* LuaBoostFilesystemProvider::getFile( const std::string& path, const std::string& mode ) const
			{
				return new LuaBoostFile( path.c_str(), mode.c_str() );
			}

			LuaAbstractDirectory* LuaBoostFilesystemProvider::getDirectory( const std::string& path ) const
			{
				using namespace boost;
				if( filesystem::exists( path ) && filesystem::is_directory( path ) )
					return new LuaBoostDirectory( path.c_str() );
				return NULL;
			}

			LuaAbstractDirectory* LuaBoostFilesystemProvider::createDirectory( const std::string& path, bool create_parents )
			{
				boost::system::error_code error;
				using namespace boost::filesystem;

				bool result = create_parents ? create_directories( path, error ) : create_directory( path, error );

				if( !result )
				{
					std::clog << error.message() << std::endl;
					return NULL;
				}

				return new LuaBoostDirectory( path.c_str() );
			}

			LUA_NUMBER LuaBoostFilesystemProvider::getTimestamp( const std::string& path ) const
			{
				return LUA_NUMBER( boost::filesystem::last_write_time( path ) );
			}

			LUA_NUMBER LuaBoostFilesystemProvider::getFileSize( const std::string& path ) const
			{
				return LUA_NUMBER( boost::filesystem::file_size( path ) );
			}

			LuaAbstractFile* LuaBoostFilesystemProvider::getDefaultStdin() const
			{
				return m_Stdin;
			}

			LuaAbstractFile* LuaBoostFilesystemProvider::getDefaultStdout() const
			{
				return m_Stdout;
			}

			LuaAbstractFile* LuaBoostFilesystemProvider::getDefaultStderr() const
			{
				return m_Stderr; 
			}

			std::string LuaBoostFilesystemProvider::getCurrentDirectory() const
			{
				return boost::filesystem::current_path( ).string();
			}

			void LuaBoostFilesystemProvider::setCurrentDirectory( const std::string& path ) const
			{
				boost::filesystem::current_path( path );
			}

			LuaBoostFilesystemProvider::LuaBoostFilesystemProvider()
			{
				m_Stdin  = new LuaBoostFile( LuaBoostFile::StdInput  );
				m_Stdout = new LuaBoostFile( LuaBoostFile::StdOutput );
				m_Stderr = new LuaBoostFile( LuaBoostFile::StdError  );
			}

			LuaBoostFilesystemProvider::~LuaBoostFilesystemProvider()
			{
				delete m_Stdin;
				delete m_Stdout;
				delete m_Stderr;
			}

			LuaAbstractFile* LuaBoostFilesystemProvider::getTempFile() const
			{
				LuaBoostFile* file = new LuaBoostFile( ::tmpfile() );
				return file;
			}

		}
	}
}