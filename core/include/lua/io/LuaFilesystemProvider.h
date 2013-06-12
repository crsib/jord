#pragma once;

#include <string>
#include <lua.hpp>

namespace lua
{
	namespace io
	{
		class LuaAbstractFile;
		class LuaAbstractDirectory;

		class LuaFilesystemProvider
		{
		public:
			LuaFilesystemProvider() {}
			virtual ~LuaFilesystemProvider();

			virtual bool                  pathExists( const std::string& path ) const = 0;

			virtual bool                  isRegularFile( const std::string& path ) const = 0;
			virtual bool                  isDirectory( const std::string& path ) const = 0;
			virtual bool                  isSymlink( const std::string& path ) const = 0;

			virtual bool                  isEmpty( const std::string& path ) const = 0;

			virtual LuaAbstractFile*      getFile( const std::string& path, const std::string& mode ) const = 0;
			virtual LuaAbstractFile*      getTempFile() { return NULL; }

			virtual LuaAbstractDirectory* getDirectory( const std::string& path ) const = 0;
			virtual LuaAbstractDirectory* createDirectory( const std::string& path, bool create_parents ) { return NULL; }

			virtual LUA_NUMBER            getTimestamp( const std::string& path ) const = 0;
			virtual LUA_NUMBER            getFileSize( const std::string& path ) const = 0;

			virtual LuaAbstractFile*      getDefaultStdin()  const = 0;
			virtual LuaAbstractFile*      getDefaultStdout() const = 0;
			virtual LuaAbstractFile*      getDefaultStderr() const = 0;

			virtual std::string           getCurrentDirectory() const = 0;
			virtual void                  setCurrentDirectory( const std::string& path ) const = 0;
		};
	}
}