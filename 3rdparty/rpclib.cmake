
if(NOT TARGET rpclib)
	add_subdirectory(rpclib)
    set_target_properties(rpc PROPERTIES FOLDER 3rdparty)
    add_library (rpclib ALIAS rpc)
    #add_library(rpclib INTERFACE)
    #target_include_directories(rpclib INTERFACE ${THIRD_PARTY_DIR}/rpclib/include)
    #ExternalProject_Add(rpc
    #    PREFIX rpclib
    #    SOURCE_DIR ${THIRD_PARTY_DIR}/rpclib
    #    BINARY_DIR ${THIRD_PARTY_DIR}/rpclib/build
    #    CMAKE_ARGS
    #    -DCMAKE_BUILD_TYPE=Release
    #    -DCMAKE_CONFIGURATION_TYPES=Release
    #    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    #    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    #    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    #    -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_DIR}/rpclib/build
    #    BUILD_COMMAND ${CMAKE_COMMAND} --build ${THIRD_PARTY_DIR}/rpclib/build --config Release
    #)
    #add_dependencies(rpclib rpc)
    #
    #if (CMAKE_HOST_WIN32)
    #    target_link_libraries(rpclib INTERFACE ${THIRD_PARTY_DIR}/rpclib/build/Release/rpc.lib)
    #endif()
endif()