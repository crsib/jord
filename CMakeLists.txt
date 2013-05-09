cmake_minimum_required ( VERSION 2.8.10 )

project( Tester )

set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/static )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )

if( WIN32)
	set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin )
	set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin )
	set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/bin )
	set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin )
endif()

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY 	 ${CMAKE_SOURCE_DIR}/bin )

if( WIN32)
	set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin )
	set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin )
	set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/bin )
	set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin )
endif()

if( MSVC )
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -D_USE_MATH_DEFINES -Dsnprintf=_snprintf -D_SCL_SECURE_NO_WARNINGS )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251" )
else()
	if( CMAKE_COMPILER_IS_GNUCC )
		set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-march=i686" )
	endif()
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package( Boost COMPONENTS  filesystem system date_time regex atomic serialization thread program_options )
add_definitions( -DBOOST_ALL_NO_LIB )

include_directories( 3party/lua/src 3party/luabind 3party/sqlite3 ${Boost_INCLUDE_DIR} 3party/process )

add_subdirectory( 3party/sqlite3 )
add_subdirectory( 3party/lsqlite3 )
add_subdirectory( 3party/lua/src )
add_subdirectory( 3party/luabind )

include_directories( rc/include )
add_subdirectory( rc )

include_directories( core/include )
add_subdirectory( core )

add_subdirectory( admin )

add_subdirectory( tests ) 