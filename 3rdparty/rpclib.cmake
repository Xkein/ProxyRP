
if(NOT TARGET rpclib)
	add_subdirectory(rpclib)
    set_target_properties(rpc PROPERTIES FOLDER 3rdparty)
    add_library (rpclib ALIAS rpc)
endif()