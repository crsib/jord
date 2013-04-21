set( ADMIN_INCLUDES
	
)

set( ADMIN_SOURCES
	source/admin.cpp

)

set( ADMIN_RC_DEPS 
	lua/entry.lua
)

source_group( "lua" FILES 
	lua/entry.lua
)

source_group( "include" FILES

)


source_group( "source" FILES
	source/admin.cpp
)

add_custom_command( 
	OUTPUT admin_rc.cpp 
	COMMAND rc -o ${CMAKE_CURRENT_BINARY_DIR}/admin_rc.cpp admin.rc 
	MAIN_DEPENDENCY admin.rc 
	DEPENDS ${ADMIN_RC_DEPS} rc
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
)

set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/admin_rc.cpp PROPERTIES GENERATED true )

include_directories( include )

add_executable( admin  ${ADMIN_INCLUDES} ${ADMIN_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/admin_rc.cpp ${ADMIN_RC_DEPS} )
target_link_libraries( admin rclib core Lua luabind ${Boost_LIBRARIES} )

set_target_properties ( admin PROPERTIES FOLDER "core" )