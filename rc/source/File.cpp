#include "rc/File.h"

namespace rc
{
	File::File( size_t initial_offset, size_t size, const std::string& name, Node* parent /*= nullptr */ )
		: Node( name, parent ), m_Offset( initial_offset ), m_Size( size )
	{}
}