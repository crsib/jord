#include "rc/Directory.h"
#include "rc/RCException.h"

#include <boost/algorithm/string.hpp>

namespace rc
{

	Directory::Directory( const std::string& name /*= ""*/, Node* parent /*= nullptr */ )
		: Node( name, parent )
	{}

	void Directory::addChild( const NodePtr& child )
	{
		if(!child)
			return ;

		m_ChildrenLookup[ child->getName() ] = child;
		child->setParent( this );
	}

	void Directory::addChildren( const children_list_t& children )
	{
		for( children_list_t::const_iterator it = children.begin(), end = children.end(); it != end; ++it )
			addChild( *it );
	}

	void Directory::getChildren( children_list_t& children, bool recursive )
	{
		getChildren( children, []( const NodePtr& node ) { return true; }, recursive );
	}

	rc::NodePtr Directory::find( const std::string& name, bool recursive /*= false */ ) const
	{
		if(!recursive)
		{
			children_lookup_t::const_iterator it = m_ChildrenLookup.find( name );
			
			return it != m_ChildrenLookup.end() ? it->second : NodePtr();
		}
		else
		{
			std::vector< std::string > tokens;
			boost::algorithm::split( tokens, name, boost::is_any_of( std::string( "/" ) ) );

			const Directory* current = this;

			for( size_t i = 0; i < (tokens.size() - 1); ++i )
			{
				std::string token = tokens[i];

				children_lookup_t::const_iterator it = current->m_ChildrenLookup.find( token );

				if(  it == current->m_ChildrenLookup.end() )
					return NodePtr();

				Directory* child = dynamic_cast<Directory*>( it->second.get() );
				if( !child )
					return NodePtr();

				current = child;
			}

			children_lookup_t::const_iterator it = current->m_ChildrenLookup.find( tokens.back() );
			return it != current->m_ChildrenLookup.end() ? it->second : NodePtr();
		}
		
		return NodePtr();
	}

	void Directory::setOwner( Root* root )
	{
		for( children_lookup_t::iterator it = m_ChildrenLookup.begin(), end = m_ChildrenLookup.end(); it != end; ++it )
			it->second->setOwner( root );

		m_Owner = root;
	}

}

