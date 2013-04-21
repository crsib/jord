#pragma once;

#include <string>
#include <memory>

namespace rc
{
	class Root;

	class  Node : public std::enable_shared_from_this<Node>
	{
		Node( const Node& );
		Node& operator = ( const Node& );
	public:
		// If the parent is non-null
		Node( const std::string& name = "", Node* parent = nullptr );

		virtual ~Node() {}

		const std::string&  getName() const { return m_Name; }		

		Node*               getParent() const { return m_Parent; }
		void                setParent( Node* node ) { m_Parent = node; }

		virtual void        setOwner( Root* root );
		Root*               getOwner() const { return m_Owner; }

		std::string         getFullName() const;
	protected:
		std::string    m_Name;
		Node*          m_Parent;

		Root*          m_Owner;
	};

	typedef std::shared_ptr< Node > NodePtr;
}