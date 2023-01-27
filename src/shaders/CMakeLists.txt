
set(TARGET_NAME "Shaders")

file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS "*.hlsl" "*.glsl")

SOURCE_GROUP("shaders" FILES ${SHADER_FILES})

add_custom_target(${TARGET_NAME} SOURCES ${SHADER_FILES})

set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "Engine" )

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ENGINE_OUTPUT_DIR}/shaders/common"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SHADER_FILES}" "${ENGINE_OUTPUT_DIR}/shaders"
)

