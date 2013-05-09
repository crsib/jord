#pragma once;

#include "lua/io/LuaFilesystemProvider.h"

namespace lua
{
	namespace io
	{
		namespace details
		{
			class LuaBoostFilesystemProvider : public LuaFilesystemProvider
			{
			public:
				LuaBoostFilesystemProvider();
				~LuaBoostFilesystemProvider();

				virtual bool pathExists( const std::string& path ) const;

				virtual bool isRegularFile( const std::string& path ) const;

				virtual bool isDirectory( const std::string& path ) const;

				virtual bool isSymlink( const std::string& path ) const;

				virtual bool isEmpty( const std::string& path ) const;

				virtual LuaAbstractFile* getFile( const std::string& path, const std::string& mode ) const;

				virtual LuaAbstractFile* getTempFile() const;

				virtual LuaAbstractDirectory* getDirectory( const std::string& path ) const;

				virtual LuaAbstractDirectory* createDirectory( const std::string& path, bool create_parents );

				virtual LUA_NUMBER getTimestamp( const std::string& path ) const;

				virtual LUA_NUMBER getFileSize( const std::string& path ) const;

				virtual LuaAbstractFile* getDefaultStdin() const;

				virtual LuaAbstractFile* getDefaultStdout() const;

				virtual LuaAbstractFile* getDefaultStderr() const;

				virtual std::string           getCurrentDirectory() const;
				virtual void                  setCurrentDirectory( const std::string& path ) const;
			private:
				LuaAbstractFile*         m_Stdin;
				LuaAbstractFile*         m_Stdout;
				LuaAbstractFile*         m_Stderr;
			};
		}
	}
}