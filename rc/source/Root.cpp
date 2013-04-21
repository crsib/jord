#include "rc/Root.h"
#include <cassert>

#include <vector>
#include <stack>

namespace rc
{
	
	Root::Root( const TOCItem* toc, uint8_t const * const base_address )
		: m_BaseAddress( base_address )
	{
		assert( toc->Type == TOCItem::Directory ); //Root node must be a directory

		m_RootDirectory = new Directory( ":/" );
		m_RootDirectory->setOwner( this );

		if( !toc->EntriesCount )
			return ;
		
		typedef std::pair< Directory*, size_t > toc_parser_entry_t;
		typedef std::stack< toc_parser_entry_t > toc_parser_state_t;

		toc_parser_state_t state;
		state.push( std::make_pair( m_RootDirectory, toc->EntriesCount ) );
		
		const TOCItem* current = toc;

		while( !state.empty() )
		{
			++current;
			state.top().second--;

			if( current->Type == TOCItem::Directory )
			{
				if( current->EntriesCount )
				{
					Directory* new_dir = new Directory( current->Name, state.top().first );
					state.push( std::make_pair( new_dir, current->EntriesCount ) );
				}
			}
			else
				new File( current->Offset, current->FileSize, current->Name, state.top().first );
			
			while( !state.empty() && !state.top().second )
				state.pop();
		}
	}


	Root::~Root()
	{
		delete m_RootDirectory;
	}

	NodePtr Root::find( const std::string& path ) const
	{
		return m_RootDirectory->find( path.substr( 2 ), true );
	}

	void const* Root::getFilePointer( const NodePtr& node ) const
	{
		if( !node )
			return NULL;

		File* file = dynamic_cast<File*>( node.get() );

		if( !file )
			return NULL;

		return m_BaseAddress + file->getOffset();
	}

	size_t Root::getFileSize( const NodePtr& node ) const
	{
		if( !node )
			return NULL;

		File* file = dynamic_cast<File*>( node.get() );

		if( !file )
			return NULL;

		return file->getSize();
	}
}