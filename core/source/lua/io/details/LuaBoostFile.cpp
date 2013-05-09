#include "lua/io/details/LuaBoostFile.h"

namespace lua
{
	namespace io
	{
		namespace details
		{
			static char __file_buffer[ LUAL_BUFFERSIZE ];

			bool LuaBoostFile::readNumber( LUA_NUMBER& number )
			{
				clearerr( m_FileHandle );
				bool result =  1 == fscanf( m_FileHandle, LUA_NUMBER_SCAN, &number );
				if( !result )
					m_ErrorCode = errno;
				return result;
			}

			bool LuaBoostFile::readLine( std::string& line )
			{
				clearerr( m_FileHandle );
				size_t size = line.size();

				while( true )
				{
					size_t l;

					if( fgets( __file_buffer, LUAL_BUFFERSIZE, m_FileHandle ) == NULL )
					{ 
						// EOF
						bool result = (line.size() - size) > 0;
						if( !result )
							m_ErrorCode = errno;
						return result;
					}

					l = strlen( __file_buffer );
					if (l == 0 || __file_buffer[ l - 1 ] != '\n')
						line += __file_buffer;
					else {
						__file_buffer[ l - 1 ] = '\0';
						line += __file_buffer;
						return true;  
					}
				}

				return false;
			}

			bool LuaBoostFile::read( std::vector<char>& data, size_t num_bytes )
			{
				clearerr( m_FileHandle );
				size_t buf_size = data.size();
				size_t nr;
				size_t read_count;
				do 
				{
					read_count = num_bytes < LUAL_BUFFERSIZE ? num_bytes : LUAL_BUFFERSIZE;
					nr = fread( __file_buffer, sizeof(char), read_count, m_FileHandle );

					num_bytes -= nr;

					data.insert( data.end(), __file_buffer, __file_buffer + nr );
				} while( num_bytes && nr == read_count );  

				bool result = ( data.size() - buf_size ) > 0;
				if( !result )
					m_ErrorCode = errno;

				return ( data.size() - buf_size ) > 0;
			}

			bool LuaBoostFile::writeNumber( LUA_NUMBER number )
			{
				bool result = fprintf( m_FileHandle, LUA_NUMBER_FMT, number ) > 0;
				if( !result )
					m_ErrorCode = errno;

				return result;
			}

			bool LuaBoostFile::writeBuffer( const char* data, size_t size )
			{
				bool result = fwrite( data, sizeof(char), size, m_FileHandle ) == size;
				if( !result )
					m_ErrorCode = errno;
				return result;
			}

			bool LuaBoostFile::flush()
			{
				bool result = 0 == fflush( m_FileHandle );
				if( !result )
					m_ErrorCode = errno;
				return result;
			}

			bool LuaBoostFile::close()
			{
				bool result = m_OwnsFile && m_FileHandle && (0 == fclose( m_FileHandle ) );
				if( !result )
					m_ErrorCode = errno;
				m_FileHandle = NULL;
				return result;
			}

			bool LuaBoostFile::seek( SeekMode mode, long offset, long& out_position )
			{
				static const int __mode[] = { SEEK_SET, SEEK_CUR, SEEK_END };
				bool result = 0 == fseek( m_FileHandle, offset, __mode[ mode ] );
				if( !result )
					m_ErrorCode = errno;
				out_position = ftell( m_FileHandle );
				return result;
			}

			bool LuaBoostFile::setvbuf( VBufMode mode, size_t size )
			{
				static const int __mode[] = {_IONBF, _IOFBF, _IOLBF};

				bool result = 0 == ::setvbuf( m_FileHandle, NULL, __mode[mode], size );
				if( !result )
					m_ErrorCode = errno;
				return result;
			}

			std::string LuaBoostFile::getLastError() const
			{
				return strerror( m_ErrorCode );
			}

			LuaBoostFile::LuaBoostFile( const char* path, const char* mode ) 
				: LuaAbstractFile( path ), m_OwnsFile( true ), m_IsStdFile( false )
			{
				m_FileHandle = fopen( path, mode );

				if( !m_FileHandle )
					m_ErrorCode = errno;
			}

			LuaBoostFile::LuaBoostFile( FILE* handle )
				: LuaAbstractFile( "" ), m_FileHandle(handle), m_OwnsFile( true ), m_IsStdFile( false )
			{

			}

			static const char* __std_name[] = { "stdin", "stdout", "stderr" };

			LuaBoostFile::LuaBoostFile( StdFile std_f )
				: LuaAbstractFile( __std_name[ std_f ] ), m_OwnsFile( false ), m_IsStdFile( true )
			{
				static FILE* __std_files[] = { stdin, stdout, stderr };
				m_FileHandle = __std_files[ std_f ];
			}

			LuaBoostFile::~LuaBoostFile()
			{
				if( m_OwnsFile && m_FileHandle )
					fclose( m_FileHandle );
			}

			bool LuaBoostFile::isOpen() const
			{
				return NULL != m_FileHandle;
			}

			bool LuaBoostFile::isStdFile() const
			{
				return m_IsStdFile;
			}

			int LuaBoostFile::getLastErrorCode() const
			{
				return m_ErrorCode;
			}

			bool LuaBoostFile::hasFailed() const
			{
				bool err = ferror( m_FileHandle ) != 0;
				if( err ) 
					const_cast<LuaBoostFile*>(this)->m_ErrorCode = errno;
				return err;
			}

			bool LuaBoostFile::isEOF() const
			{
				long c = ::getc( m_FileHandle );
				ungetc( c, m_FileHandle );
				clearerr( m_FileHandle );
				return c == EOF; 
			}

		}
	}
}