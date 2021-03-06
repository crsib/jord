#include "rc/private/RCBuilder.h"
#include "lua.hpp"

#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"

#include "rc/RCException.h"
#include <stack>

namespace boost
{
	namespace filesystem
	{
		template < >
		path& path::append< typename path::iterator >( typename path::iterator begin, typename path::iterator end, const codecvt_type& cvt)
		{ 
			for( ; begin != end ; ++begin )
				*this /= *begin;
			return *this;
		}
	}
}

namespace rc
{
	namespace
	{
		// From a (rejected) proposal to boost.filesystem extension
		// Return path when appended to a_From will resolve to same as a_To
		boost::filesystem::path make_relative( boost::filesystem::path a_From, boost::filesystem::path a_To )
		{
			a_From = boost::filesystem::absolute( a_From ); a_To = boost::filesystem::absolute( a_To );
			boost::filesystem::path ret;
			boost::filesystem::path::const_iterator itrFrom( a_From.begin() ), itrTo( a_To.begin() );
			// Find common base
			for( boost::filesystem::path::const_iterator toEnd( a_To.end() ), fromEnd( a_From.end() ) ; itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo; ++itrFrom, ++itrTo );
			// Navigate backwards in directory to reach previously found base
			for( boost::filesystem::path::const_iterator fromEnd( a_From.end() ); itrFrom != fromEnd; ++itrFrom )
			{
				if( (*itrFrom) != "." )
					ret /= "..";
			}
			// Now navigate down the directory branch
			ret.append( itrTo, a_To.end() );
			return ret;
		}
	}

	using namespace boost;

	RCBuilder::RCBuilder()
	{
		Root::TOCItem root;
		root.Type         = Root::TOCItem::Directory;
		root.EntriesCount = 0;
		root.Name         = ":/";
		root.FileSize     = 0;

		m_Root = RootPtr( new Root( &root, NULL ) );
	}

	RCBuilder::~RCBuilder()
	{

	}

	void RCBuilder::parseConfig( const std::string& config_path )
	{
		filesystem::path cfg_path( config_path );

		if( !filesystem::exists( cfg_path ) )
			throw RCException( "Failed to find config file" );

		uintmax_t config_size = filesystem::file_size( config_path );

		
		filesystem::ifstream stream;

		stream.open( cfg_path, std::ios_base::in );
		if(!stream.is_open())
			throw RCException( "Failed to open config file" );

		char* cfg = new char[ size_t( config_size + uintmax_t(8) ) ];

		memset( cfg, 0, size_t( config_size + uintmax_t(8) ) );
		memcpy( cfg, "return ", 7 );

		stream.read( cfg + 7, config_size );

		lua_State* l = lua_open();

		struct state_guard 
		{ 
			lua_State* state; 
			state_guard( lua_State* ls) : state( ls ) {} 
			~state_guard() { lua_close( state ); } 
		} guard(l);

		bool parsed = !luaL_dostring( l, cfg );
		delete [] cfg;

		if( !parsed  )
			throw RCException( lua_tostring( l, -1 ) );

		if( !lua_istable( l, -1 ) )
			throw RCException( "Unexpected data in the config file" );

		// Great, stack top now contains something, that is possibly a config

		// Retrieve all the entries
		lua_pushnil( l );

		while( lua_next( l, -2 ) )
		{
			if( !lua_istable( l, -1 ) )
				throw RCException( "Unexpected data in the config file" );

			parceConfigElement( l );

			lua_pop( l, 1 );
		}

		m_ModuleName = filesystem::path( config_path ).stem().string();
	}

	void RCBuilder::writeFile( const std::string& output_path )
	{
		filesystem::ofstream stream;

		stream.open( output_path, std::ios_base::out );

		if( !stream.is_open( ) )
			throw RCException( "Failed to open the file for writing" );

		stream 
			<< "//Auto generated by rcgen.\n"
			<< "\n"
			<< "#include \"rc/Root.h\"\n\n";

		// Prepare TOC
		stream << "static const rc::Root::TOCItem __toc[] = {\n\t";

		size_t items_written = 0;

		static const size_t items_per_string = 3;

		std::stack< std::pair< Directory::children_list_t, size_t > > directories;

		Directory::children_list_t temp_list;

		m_Root->getRootDirectory()->getChildren( temp_list );
		if( temp_list.size() )
			directories.push( std::make_pair(temp_list, 0) );

		stream << "{ rc::Root::TOCItem::Directory, \":/\", " << temp_list.size() << ", 0 }, ";
		++items_written;
		
		while( !directories.empty() )
		{
			if( ( items_written % items_per_string ) == 0 )
				stream << "\n\t";

			++items_written;

			NodePtr node = directories.top().first[ directories.top().second ];
			directories.top().second++;
			
			Directory* dir = dynamic_cast< Directory* >( node.get() );

			if( dir )
			{
				temp_list.clear();
				dir->getChildren( temp_list );
				if( temp_list.size() )
					directories.push( std::make_pair(temp_list, 0) );

				stream << "{ rc::Root::TOCItem::Directory, \"" << dir->getName() << "\", " << temp_list.size() << ", 0 }, ";
			}
			else
			{
				File* file = static_cast< File* >( node.get() );
				stream << "{ rc::Root::TOCItem::File, \"" << file->getName() << "\", " << file->getOffset() << ", " << file->getSize() << " }, ";
			}

			while( !directories.empty() && (directories.top().first.size() <= directories.top().second) )
				directories.pop();
		}

		if( ( items_written % items_per_string ) == 0 )
			stream << "\n\t";

		stream << "{ rc::Root::TOCItem::File, \"\", 0, 0 } \n};\n\n";

		// Write down the files
		stream << "static const uint8_t __data[] = {";
		static const size_t bytes_per_row = 30;
		size_t bytes_written = 0;

		for( size_t i = 0; i < m_EntriesList.size(); ++i )
		{
			filesystem::ifstream input;
			input.open( m_EntriesList[i].path, std::ios_base::in | std::ios_base::binary );

			if(!input.is_open())
				throw RCException( std::string("Failed to open '") + m_EntriesList[i].path + "'");

			stream << "\n\t// " <<  m_EntriesList[i].path << " " << m_EntriesList[i].size << "\n\t";

			for( size_t j = 0; j < m_EntriesList[i].size; ++j )
			{
				if( (bytes_written % bytes_per_row ) == 0 )
					stream << "\n\t";
				++bytes_written;

				char data;
				input.read( &data, 1 );

				stream  << int( data ) << ", ";
			}
		}

		if( (bytes_written % bytes_per_row ) == 0 )
			stream << "\n\t";
		stream << "0\n};\n\n";

		stream << "extern rc::RootPtr " << m_ModuleName << "_open();\n\n";
		stream << "rc::RootPtr " << m_ModuleName << "_open()\n{\n";
		stream << "\treturn rc::RootPtr( new rc::Root( __toc, __data ) );\n";
		stream << "}\n";
	}

	void RCBuilder::addEntry( const std::string& base_path, const std::string& rp, const std::string& path, bool recursive )
	{
		filesystem::path entry_path( rp );
		entry_path /= path;

		if( filesystem::is_regular_file( entry_path ) )
			addFile( base_path, rp, path );
		else if( filesystem::is_directory( entry_path) )
		{
			for( filesystem::directory_iterator it( entry_path ), end; it != end; ++it )
			{
				filesystem::path p = it->path();

				filesystem::path rel_path = make_relative( rp, p );

				if( filesystem::is_directory( p ) && recursive )
					addEntry( base_path, rp, rel_path.string(), recursive );
				else if( filesystem::is_regular_file( p ) )
					addFile( base_path, rp, rel_path.string() );
			}
		}
	}

	void RCBuilder::addFile( const std::string& _base_path, const std::string& rp, const std::string& path )
	{
		std::string base_path = _base_path;
		if( base_path.substr( 0, 2 ) == ":/" )
			base_path = base_path.substr( 2 );

		Directory* current = m_Root->getRootDirectory();

		filesystem::path base_path_decomp(base_path);
		filesystem::path rel_path( path );

		base_path_decomp /= rel_path.parent_path();

		for( filesystem::path::iterator it = base_path_decomp.begin(), end = base_path_decomp.end(); it != end; ++it )
		{
			std::string dir = it->string();

			if( dir.empty() || dir == "." || dir == ".." )
				continue ;

			NodePtr node = current->find( dir );
			
			if( !node )
				current = new Directory( dir, current );
			else
				current = dynamic_cast<Directory*>( node.get() );

			if( !current )
				throw RCException( "RC tree error" );
		}

		//for( path::iterator  )
		rel_path = filesystem::path( rp ) / rel_path;
		size_t size   = size_t( filesystem::file_size( rel_path ) );
		size_t offset = m_EntriesList.empty() ? 0 : m_EntriesList.back().offset + m_EntriesList.back().size;

		new File( offset, size, rel_path.filename().string(), current );

		m_EntriesList.push_back( file_entry_t( rel_path.string(), size, offset ) );
	}

	void RCBuilder::parceConfigElement( lua_State* l )
	{
		lua_getfield( l, -1, "base_path" );
		std::string base_path = ":/";

		if( lua_isstring( l, -1 ) )
			base_path = lua_tostring( l, -1 );

		lua_pop( l, 1 );

		lua_getfield( l, -1, "resource_path" );
		std::string resource_path = ".";

		if( lua_isstring( l, -1 ) )
			resource_path = lua_tostring( l, -1 );

		lua_pop( l, 1 );

		lua_getfield( l, -1, "entries" );

		lua_pushnil( l );

		while( lua_next( l, -2 ) )
		{
			if( lua_istable( l, -1 ) )
			{
				lua_pushnumber( l, 1 );
				lua_gettable( l, -2 );

				std::string path = ".";

				if( lua_isstring( l, -1 ) )
					path = lua_tostring( l, -1 );

				lua_pop( l, 1 );

				lua_getfield( l, -1, "recursive" );
				bool recursive = lua_toboolean( l, -1 ) != 0;
				lua_pop( l, 1 );

				addEntry( base_path, resource_path, path, recursive );
			}
			else
				addEntry( base_path, resource_path, lua_tostring( l, -1 ), false );

			lua_pop( l, 1 );
		}

		lua_pop( l, 1 );
	}

}