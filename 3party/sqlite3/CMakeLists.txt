set( SQLITE_SOURCES sqlite3.c sqlite3.h sqlite3ext.h )
add_library( sqlite3 STATIC ${SQLITE_SOURCES} )
set_target_properties ( sqlite3 PROPERTIES FOLDER "3party" )