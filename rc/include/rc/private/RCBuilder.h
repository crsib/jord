#pragma once

#include "rc/Root.h"

struct lua_State;

namespace rc
{
	class RCBuilder
	{
		RCBuilder( const RCBuilder& );
		RCBuilder& operator = ( const RCBuilder& );
	public:
		RCBuilder();
		~RCBuilder();

		void     parseConfig( const std::string& config_path );
		void     writeFile( const std::string& output_path );
	private:
		void     addEntry( const std::string& base_path, const std::string& rp, const std::string& path, bool recursive );

		void     addFile(       const std::string& base_path, const std::string& rp, const std::string& path );

		void     parceConfigElement( lua_State* l );

		struct file_entry_t 
		{
			std::string   path;
			size_t        size;
			size_t        offset;

			file_entry_t( const std::string& fpath, size_t fsize, size_t foffset )
				: path( fpath ), size( fsize ), offset( foffset ) 
			{}

			file_entry_t( const file_entry_t& rhs )
				: path( rhs.path ), size( rhs.size ), offset( rhs.offset )
			{}

			file_entry_t& operator = ( const file_entry_t& rhs )
			{
				path = rhs.path; size = rhs.size;
				offset = rhs.offset; return *this;
			}
		};

		typedef std::vector< file_entry_t > file_entry_list_t;

		file_entry_list_t  m_EntriesList;

		RootPtr     m_Root;
		std::string m_ModuleName;
	};
}