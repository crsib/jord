#include "rc/rc.h"

#include <vector>

#include <boost/thread/mutex.hpp>

namespace rc
{
	class RCOwner;

	class KillRCOwner
	{
	public:
		KillRCOwner() : m_KillMe(nullptr) {}
		~KillRCOwner();
		void setRCOwner( RCOwner* owner ) { m_KillMe = owner; }
	private:
		RCOwner*    m_KillMe;
	};

	class RCOwner
	{
		static RCOwner* instance;
	public:
		static RCOwner& GetInstance() 
		{
			//Thread safe in C++ 11
			if( !instance )
			{
				boost::mutex::scoped_lock lock( m_Mutex );
				if(!instance)
				{
					instance = new RCOwner;
					m_Killer.setRCOwner( instance );
				}
			}
			
			return *instance;
		}

		void     addResource( const RootPtr& resource_pack )
		{
			boost::mutex::scoped_lock lock( m_Mutex );
			m_Resources.push_back( resource_pack ); 
		}

		NodePtr  findNode( const std::string& path ) const 
		{
			boost::mutex::scoped_lock lock( m_Mutex );
			for( size_t i = 0; i < m_Resources.size(); ++i )
			{
				RootPtr root = m_Resources[ i ];
				NodePtr node = root->find( path );

				if( node )
					return node;
			}
			return NodePtr();
		}

		void const* getFilePointer( const NodePtr& node ) const
		{
			if( !node )
				return nullptr;

			return node->getOwner()->getFilePointer( node );
		}

		size_t     getFileSize( const NodePtr& node ) const
		{
			if( !node )
				return 0;

			return node->getOwner()->getFileSize( node );
		}
	private:
		std::vector< RootPtr >  m_Resources;
		static boost::mutex     m_Mutex;
		static KillRCOwner      m_Killer;
	};

	RCOwner*     RCOwner::instance = nullptr;
	boost::mutex RCOwner::m_Mutex;
	KillRCOwner  RCOwner::m_Killer;
 
	KillRCOwner::~KillRCOwner()
	{
		delete m_KillMe;
	}

	void add_resource( const RootPtr& resource_root )
	{
		RCOwner::GetInstance().addResource( resource_root );
	}

	NodePtr find_node( const std::string& path )
	{
		return RCOwner::GetInstance().findNode( path );
	}

	void const* get_file_pointer( const NodePtr& node )
	{
		return RCOwner::GetInstance().getFilePointer( node );
	}

	size_t       get_file_size( const NodePtr& node )
	{
		return RCOwner::GetInstance().getFileSize( node );
	}
}
