file(GLOB LUABIND_SOURCE "src/*.cpp")
add_library( luabind STATIC ${LUABIND_SOURCE} )
set_target_properties ( luabind PROPERTIES FOLDER "3party" )