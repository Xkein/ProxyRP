
set(TARGET_NAME ProxyRP)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ProxyRP)
endif()


file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
# used by precompile
set(RUNTIME_HEADS "${HEADER_FILES}" PARENT_SCOPE)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})
add_executable(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 20)

target_compile_definitions(${TARGET_NAME} PRIVATE GLFW_INCLUDE_VULKAN)

target_compile_definitions(${TARGET_NAME} PRIVATE GLM_FORCE_RADIANS)
target_compile_definitions(${TARGET_NAME} PRIVATE GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
target_compile_definitions(${TARGET_NAME} PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/src>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/src/runtime>)

# Link dependencies    
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC json)
target_link_libraries(${TARGET_NAME} PUBLIC spdlog)
target_link_libraries(${TARGET_NAME} PUBLIC Vulkan)
target_link_libraries(${TARGET_NAME} PUBLIC assimp)
target_link_libraries(${TARGET_NAME} PUBLIC stb)
target_link_libraries(${TARGET_NAME} PUBLIC RTTR::Core_Lib)
target_link_libraries(${TARGET_NAME} PUBLIC eigen)
target_link_libraries(${TARGET_NAME} PUBLIC glm)
target_link_libraries(${TARGET_NAME} PUBLIC shader_conductor)
target_link_libraries(${TARGET_NAME} PUBLIC taskflow)
target_link_libraries(${TARGET_NAME}
    PUBLIC
        Boost::asio
        Boost::interprocess
        Boost::process
)

set(ENGINE_OUTPUT_DIR "${ENGINE_ROOT_DIR}/bin")
set(ENGINE_ASSET_DIR "${ENGINE_ROOT_DIR}/asset")

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ENGINE_OUTPUT_DIR}"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/" "${ENGINE_OUTPUT_DIR}"
	COMMAND ${CMAKE_COMMAND} -E remove_directory "${ENGINE_OUTPUT_DIR}/asset"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENGINE_ASSET_DIR}" "${ENGINE_OUTPUT_DIR}/asset"
)


