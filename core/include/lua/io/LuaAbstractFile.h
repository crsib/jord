#pragma once;

#include <string>
#include <vector>

#include <lua.hpp>

namespace lua
{
	namespace io
	{
		class LuaAbstractFile
		{
			LuaAbstractFile( const LuaAbstractFile& );
			LuaAbstractFile& operator = ( const LuaAbstractFile& );
		public:
			explicit LuaAbstractFile( const std::string& filename )
				: m_Filename( filename )
			{}

			virtual ~LuaAbstractFile() {}

			virtual bool   readNumber( LUA_NUMBER& number ) = 0;
			virtual bool   readLine( std::string& line ) = 0;
			virtual bool   read( std::vector<char>& data, size_t num_bytes = ~((size_t)0 ) ) = 0;

			virtual bool   writeNumber( LUA_NUMBER number ) { return false; }
			virtual bool   writeBuffer( const char* data, size_t size ) { return false; }

			virtual bool   flush() { return false; }
			virtual bool   close() { return false; }

			enum SeekMode
			{
				Set,
				Cur,
				End
			};

			virtual bool   seek( SeekMode mode, long offset, long& out_position ) = 0;

			enum VBufMode
			{
				No,
				Full,
				Line
			};

			virtual bool setvbuf( VBufMode mode, size_t size = LUAL_BUFFERSIZE  ) = 0;

			virtual std::string getLastError() const = 0;
			virtual int         getLastErrorCode() const = 0;
			virtual bool        hasFailed() const = 0 ;

			const std::string& getFileName() const { return m_Filename; }

			virtual bool       isOpen() const = 0;
			virtual bool       isStdFile() const = 0;
			virtual bool       isEOF() const = 0;
		protected:
			std::string    m_Filename;
		};
	}
}