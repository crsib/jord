set( INCLUDES
	
)

set( SOURCES
	io_test.cpp
)

set( RC_DEPS 
	lua/files.lua
)

source_group( "lua" FILES 
	lua/files.lua
)

source_group( "include" FILES

)


source_group( "source" FILES
	io_test.cpp
)

add_custom_command( 
	OUTPUT io_test_rc.cpp 
	COMMAND rc -o ${CMAKE_CURRENT_BINARY_DIR}/io_test_rc.cpp io_test.rc 
	MAIN_DEPENDENCY io_test.rc 
	DEPENDS ${RC_DEPS} rc
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
)

set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/io_test_rc.cpp PROPERTIES GENERATED true )

include_directories( include )

add_executable( io_test  ${INCLUDES} ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/io_test_rc.cpp ${ADMIN_RC_DEPS} )
target_link_libraries( io_test rclib core Lua luabind ${Boost_LIBRARIES} )

set_target_properties ( io_test PROPERTIES FOLDER "tests" )