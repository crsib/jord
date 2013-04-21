set( RC_HEADERS 
	./include/rc/Directory.h
	./include/rc/File.h
	./include/rc/Node.h
	./include/rc/Root.h
	./include/rc/rc.h
	./include/rc/RCException.h

	./include/rc/private/RCBuilder.h
)

set( RC_SOURCES 
	./source/Directory.cpp
	./source/File.cpp
	./source/Node.cpp
	./source/Root.cpp
	./source/rc.cpp

	./source/private/RCBuilder.cpp
)

set( RC_APP ./source/private/rc_main.cpp )

source_group( "include" FILES 
	./include/rc/Directory.h
	./include/rc/File.h
	./include/rc/Node.h
	./include/rc/Root.h
	./include/rc/rc.h
	./include/rc/RCException.h
)

source_group( "include\\private" FILES
	./include/rc/private/RCBuilder.h
)

source_group( "source" FILES
	./source/Directory.cpp
	./source/File.cpp
	./source/Node.cpp
	./source/Root.cpp
	./source/rc.cpp
)

source_group( "source\\private" FILES
	./source/private/RCBuilder.cpp
)

source_group( "app" FILES ./source/private/rc_main.cpp )

add_library( rclib STATIC ${RC_HEADERS} ${RC_SOURCES} )
set_target_properties ( rclib PROPERTIES FOLDER "rc" )

add_executable( rc ${RC_APP} )
target_link_libraries( rc rclib Lua luabind ${Boost_LIBRARIES} )
set_target_properties ( rc PROPERTIES FOLDER "rc" )