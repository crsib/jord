set( CORE_INCLUDES
	include/core.h

	include/lua/rc_lua_loader.h
)

set( CORE_SOURCES
	source/core.cpp

	source/lua/rc_lua_loader.cpp
)

set( CORE_RC_DEPS 
	lua/base/dump.lua
	lua/base/copy.lua
	lua/base/split.lua

	lua/json/json.lua
	lua/json/rpc.lua 
)

source_group( "lua\\base" FILES 
	lua/base/dump.lua
	lua/base/copy.lua
	lua/base/split.lua
)

source_group( "lua\\json" FILES 
	lua/json/json.lua
	lua/json/rpc.lua 
)

source_group( "include" FILES
	include/core.h
)

source_group( "include\\lua" FILES
	include/lua/rc_lua_loader.h
)


source_group( "source" FILES
	source/core.cpp
)

source_group( "source\\lua" FILES
	source/lua/rc_lua_loader.cpp
)

add_custom_command( 
	OUTPUT core_rc.cpp 
	COMMAND rc -o ${CMAKE_CURRENT_BINARY_DIR}/core_rc.cpp core.rc 
	MAIN_DEPENDENCY core.rc 
	DEPENDS ${CORE_RC_DEPS} rc
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
)

set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/core_rc.cpp PROPERTIES GENERATED true )

add_library( core STATIC ${CORE_INCLUDES} ${CORE_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/core_rc.cpp ${CORE_RC_DEPS} )
set_target_properties ( core PROPERTIES FOLDER "rc" )