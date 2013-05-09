set( CORE_INCLUDES
	include/core.h

	include/lua/rc_lua_loader.h
	include/lua/RegisterCustomClass.h

	include/lua/io/details/LuaBoostDirectory.h
	include/lua/io/details/LuaBoostFile.h
	include/lua/io/details/LuaBoostFilesystemProvider.h
	include/lua/io/details/LuaBoostProcess.h
	include/lua/io/details/LuaBoostProcessProvider.h
	include/lua/io/details/LuaResourceDirectory.h
	include/lua/io/details/LuaResourceFile.h
	include/lua/io/details/LuaResourceFilesystemProvider.h

	include/lua/io/LuaDirectoryAdapter.h
	include/lua/io/LuaFileAdapter.h
	include/lua/io/LuaFilesystemProvider.h
	include/lua/io/LuaIOManager.h
	include/lua/io/lua_io.h
	include/lua/io/LuaAbstractFile.h
	include/lua/io/LuaAbstractDirectory.h

)

set( CORE_SOURCES
	source/core.cpp

	source/lua/rc_lua_loader.cpp

	source/lua/io/details/LuaBoostDirectory.cpp
	source/lua/io/details/LuaBoostFile.cpp
	source/lua/io/details/LuaBoostFilesystemProvider.cpp
	source/lua/io/details/LuaBoostProcess.cpp
	source/lua/io/details/LuaBoostProcessProvider.cpp
	source/lua/io/details/LuaResourceDirectory.cpp
	source/lua/io/details/LuaResourceFile.cpp
	source/lua/io/details/LuaResourceFilesystemProvider.cpp

	source/lua/io/LuaDirectoryAdapter.cpp
	source/lua/io/LuaFileAdapter.cpp
	source/lua/io/LuaFilesystemProvider.cpp
	source/lua/io/LuaIOManager.cpp
	source/lua/io/lua_io.cpp

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
	include/lua/RegisterCustomClass.h
)

source_group( "include\\lua\\io" FILES 
	include/lua/io/LuaDirectoryAdapter.h
	include/lua/io/LuaFileAdapter.h
	include/lua/io/LuaFilesystemProvider.h
	include/lua/io/LuaIOManager.h
	include/lua/io/lua_io.h
	include/lua/io/LuaAbstractFile.h
	include/lua/io/LuaAbstractDirectory.h
)

source_group( "include\\lua\\io\\details" FILES
	include/lua/io/details/LuaBoostDirectory.h
	include/lua/io/details/LuaBoostFile.h
	include/lua/io/details/LuaBoostFilesystemProvider.h
	include/lua/io/details/LuaBoostProcess.h
	include/lua/io/details/LuaBoostProcessProvider.h
	include/lua/io/details/LuaResourceDirectory.h
	include/lua/io/details/LuaResourceFile.h
	include/lua/io/details/LuaResourceFilesystemProvider.h
)

source_group( "source" FILES
	source/core.cpp
)

source_group( "source\\lua" FILES
	source/lua/rc_lua_loader.cpp
)

source_group( "source\\lua\\io" FILES 
	source/lua/io/LuaDirectoryAdapter.cpp
	source/lua/io/LuaFileAdapter.cpp
	source/lua/io/LuaFilesystemProvider.cpp
	source/lua/io/LuaIOManager.cpp
	source/lua/io/lua_io.cpp
)

source_group( "source\\lua\\io\\details" FILES
	source/lua/io/details/LuaBoostDirectory.cpp
	source/lua/io/details/LuaBoostFile.cpp
	source/lua/io/details/LuaBoostFilesystemProvider.cpp
	source/lua/io/details/LuaBoostProcess.cpp
	source/lua/io/details/LuaBoostProcessProvider.cpp
	source/lua/io/details/LuaResourceDirectory.cpp
	source/lua/io/details/LuaResourceFile.cpp
	source/lua/io/details/LuaResourceFilesystemProvider.cpp
)

add_custom_command( 
	OUTPUT core_rc.cpp 
	COMMAND rc -o ${CMAKE_CURRENT_BINARY_DIR}/core_rc.cpp core.rc 
	MAIN_DEPENDENCY core.rc 
	DEPENDS ${CORE_RC_DEPS} rc
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
)

set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/core_rc.cpp PROPERTIES GENERATED true )

add_definitions( -DJORD_STATIC )

add_library( core STATIC ${CORE_INCLUDES} ${CORE_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/core_rc.cpp ${CORE_RC_DEPS} )
set_target_properties ( core PROPERTIES FOLDER "rc" )