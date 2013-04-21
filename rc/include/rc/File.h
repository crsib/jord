#pragma once;

#include "rc/Node.h"

#include <cstdlib>

namespace rc
{
	class File : public Node
	{
		File( const File& );
		File& operator = ( const File& );
	public:
		File( size_t initial_offset, size_t size, const std::string& name, Node* parent = nullptr );

		size_t       getOffset() const { return m_Offset; }
		size_t       getSize()   const { return m_Size;   }
	private:
		size_t       m_Offset;
		size_t       m_Size;
	};
}