#pragma once;

#include "lua/io/LuaAbstractFile.h"
#include <cstdio>

namespace lua
{
	namespace io
	{
		namespace details
		{
			class LuaBoostFile : public LuaAbstractFile
			{
			public:
				LuaBoostFile( const char* path, const char* mode );
				explicit LuaBoostFile( FILE* handle );

				enum StdFile
				{
					StdInput,
					StdOutput,
					StdError
				};

				explicit LuaBoostFile( StdFile std_f );

				~LuaBoostFile();

				virtual bool readNumber( LUA_NUMBER& number );
				virtual bool readLine( std::string& line );
				virtual bool read( std::vector<char>& data, size_t num_bytes );

				virtual bool writeNumber( LUA_NUMBER number );
				virtual bool writeBuffer( const char* data, size_t size );

				virtual bool flush();

				virtual bool close();

				virtual bool seek( SeekMode mode, long offset, long& out_position );

				virtual bool setvbuf( VBufMode mode, size_t size );

				virtual std::string getLastError() const;
				virtual int         getLastErrorCode() const;
				virtual bool        hasFailed() const;

				virtual bool isOpen() const;
				virtual bool isStdFile() const;
				virtual bool isEOF() const;
			private:
				FILE*   m_FileHandle;
				int     m_ErrorCode;
				bool    m_OwnsFile;
				bool    m_IsStdFile;
			};
		}
	}
}