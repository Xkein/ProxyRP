
set(TARGET_NAME ProxyRP)

set(ENGINE_ROOT_DIR "${PROXYRP_ROOT_DIR}/src")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ProxyRP)
endif()


file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})
add_executable(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_compile_definitions(${TARGET_NAME} PRIVATE GLFW_INCLUDE_VULKAN)

target_compile_definitions(${TARGET_NAME} PRIVATE GLM_FORCE_RADIANS)
target_compile_definitions(${TARGET_NAME} PRIVATE GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
target_compile_definitions(${TARGET_NAME} PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE)

target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/runtime>)

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

target_link_libraries(${TARGET_NAME} PUBLIC ShaderConductor)

target_link_libraries(${TARGET_NAME} PUBLIC taskflow)

target_link_libraries(${TARGET_NAME}
    PUBLIC
        Boost::asio
        Boost::interprocess
        Boost::process
)

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")
