#pragma once;
#include "lua/io/LuaAbstractDirectory.h"

namespace boost
{
	namespace filesystem
	{
		class directory_iterator;
	}
}

namespace lua
{
	namespace io
	{
		namespace details
		{
			class LuaBoostDirectory : public LuaAbstractDirectory
			{
			public:
				explicit LuaBoostDirectory( const char* path );
				~LuaBoostDirectory();

				virtual std::string getFirstEntry();
				virtual std::string getNextEntry();
			private:
				boost::filesystem::directory_iterator* m_CurrentIterator;
				std::string                            m_DirectoryPath;
			};
		}
	}
}
