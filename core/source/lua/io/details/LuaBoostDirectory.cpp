#include "lua/io/details/LuaBoostDirectory.h"
#include "boost/filesystem.hpp"
namespace lua
{
	namespace io
	{
		namespace details
		{
			LuaBoostDirectory::LuaBoostDirectory( const char* path )
				: m_CurrentIterator( new boost::filesystem::directory_iterator ), 
				m_DirectoryPath( path )
			{
			}

			LuaBoostDirectory::~LuaBoostDirectory()
			{ delete m_CurrentIterator; }

			std::string LuaBoostDirectory::getFirstEntry()
			{
				*m_CurrentIterator = boost::filesystem::directory_iterator( m_DirectoryPath );
				return (*m_CurrentIterator)->path().string();
			}

			std::string LuaBoostDirectory::getNextEntry()
			{
				if( *m_CurrentIterator == boost::filesystem::directory_iterator() )
					return "";
				return (*m_CurrentIterator)->path().string();
			}
		}
	}
}
