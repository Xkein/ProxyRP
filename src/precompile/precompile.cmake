

set(PRECOMPILE_TARGET "PreCompile")

set(PRECOMPILE_TOOLS_PATH ${ENGINE_ROOT_DIR}/src/_generated)
set(PRECOMPILE_PARAMS_IN_PATH "${ENGINE_ROOT_DIR}/src/precompile/precompile.txt")
configure_file(${PRECOMPILE_PARAMS_IN_PATH} "${PRECOMPILE_TOOLS_PATH}/precompile.txt")
configure_file("${ENGINE_ROOT_DIR}/src/precompile/include_directories.txt" "${PRECOMPILE_TOOLS_PATH}/include_directories.txt")


if (CMAKE_HOST_WIN32)
	set(HEADER_TOOL ${PRECOMPILE_TOOLS_PATH}/HeaderTool.exe)
    set(sys_include "*") 
else()
	message(FATAL_ERROR "OS not supported!")
endif()

file(GLOB_RECURSE TEMPLATE_FILES CONFIGURE_DEPENDS "${ENGINE_ROOT_DIR}/src/template/*.mustache")

SOURCE_GROUP("template" FILES ${TEMPLATE_FILES})

add_custom_target(${PRECOMPILE_TARGET} ALL

COMMAND ${CMAKE_COMMAND} -E echo "********************************************************"
COMMAND ${CMAKE_COMMAND} -E echo "**** Precompile begin ****"

COMMAND ${HEADER_TOOL} "${PRECOMPILE_TOOLS_PATH}/precompile.txt" "${PRECOMPILE_TOOLS_PATH}/include_directories.txt" ${sys_include} "ProxyRP" 0

COMMAND ${CMAKE_COMMAND} -E echo "**** Precompile finished ****"
COMMAND ${CMAKE_COMMAND} -E echo "********************************************************"

SOURCES ${TEMPLATE_FILES}
)

