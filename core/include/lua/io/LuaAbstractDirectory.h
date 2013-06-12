#pragma once;

#include <string>

namespace lua
{
	namespace io
	{
		class LuaAbstractDirectory
		{
		public:
			virtual ~LuaAbstractDirectory() {}

			// Both functions may modify the state
			virtual std::string getFirstEntry() = 0;
			virtual std::string getNextEntry() = 0;
			virtual std::string path() const = 0;
		};
	}
}