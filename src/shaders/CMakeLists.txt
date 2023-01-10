
set(TARGET_NAME "Shaders")

file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS "*.hlsl" "*.glsl")

SOURCE_GROUP("shaders" FILES ${SHADER_FILES})

add_custom_target(${TARGET_NAME} SOURCES ${SHADER_FILES})

set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "Engine" )
