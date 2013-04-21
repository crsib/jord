#include "rc/Node.h"
#include "rc/Directory.h"
#include "rc/RCException.h"

rc::Node::Node( const std::string& name /*= ""*/, Node* parent /*= nullptr */ )
	: m_Name( name ), m_Parent( parent )
{
	if( parent )
	{
		rc::Directory* directory = dynamic_cast< rc::Directory* >( parent );

		// Throw is not really safe here, as m_Name may leak 
		if( !directory ) 
			throw RCException( "Invalid parent node" );

		directory->addChild( NodePtr( this ) );

		m_Owner = parent->m_Owner;
	}
}

void rc::Node::setOwner( Root* root )
{
	m_Owner = root;
}

std::string rc::Node::getFullName() const
{
	return m_Parent ? m_Parent->getFullName() + m_Name : m_Name;
}

