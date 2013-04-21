#pragma once;

#include "rc/Root.h"

#define INIT_RESOURCE( name ) extern rc::RootPtr name##_open();\
	rc::add_resource( name##_open() );

namespace rc
{
	void     add_resource( const RootPtr& resource_root );

	NodePtr  find_node( const std::string& path );
	void const*  get_file_pointer( const NodePtr& node );
	inline void const*  get_file_pointer( const std::string& path )
	{ get_file_pointer( find_node( path ) ); }

	size_t       get_file_size( const NodePtr& node );
	inline size_t       get_file_size( const std::string& path )
	{ get_file_size( find_node( path ) ); }
}