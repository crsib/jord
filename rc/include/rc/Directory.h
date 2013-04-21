#pragma once

#include <vector>
#include <unordered_map>
#include "rc/Node.h"

namespace rc
{
	class Directory : public Node
	{
		Directory( const Directory& );
		Directory& operator = ( const Directory& );
	public:
		Directory( const std::string& name = "", Node* parent = nullptr );

		typedef std::vector< NodePtr > children_list_t;

		void          addChild( const NodePtr& child );
		void          addChildren( const children_list_t& children );

		NodePtr       find( const std::string& name, bool recursive = false ) const;

		// Predicate is a functor accepting const NodePtr& and returning true, if the child should be added to the list
		template< typename Predicate >
		void          getChildren( children_list_t& children, Predicate pred, bool recursive ) const;
		
		void          getChildren( children_list_t& children, bool recursive = false );

		void          setOwner( Root* root );
	private:
		typedef std::unordered_map< std::string, NodePtr> children_lookup_t;

		children_lookup_t        m_ChildrenLookup;
	};

	template< typename Predicate >
	void Directory::getChildren( children_list_t& children, Predicate pred, bool recursive ) const
	{
		for( children_lookup_t::const_iterator it = m_ChildrenLookup.begin(), end = m_ChildrenLookup.end(); it != end; ++it )
		{
			if( pred( it->second ) )
				children.push_back( it->second );

			if( recursive )
			{
				Directory* dir = dynamic_cast< Directory* >( it->second.get() );
				if( dir )
					dir->getChildren( children, pred, recursive );
			}
		}
	}

}