
set(TARGET_NAME HeaderTool)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
endif()

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Tool")

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${TARGET_NAME} PRIVATE llvm)
target_link_libraries(${TARGET_NAME} PRIVATE mustache)

set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${ENGINE_ROOT_DIR}/src/_generated")
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${ENGINE_ROOT_DIR}/src/_generated")

if (CMAKE_HOST_WIN32)
	set(CLANG_LIB_PATH ${LLVM_BUILD_DIR}/bin/libclang.dll)
endif()


add_custom_command(TARGET ${TARGET_NAME} POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy "${CLANG_LIB_PATH}" $<TARGET_FILE_DIR:${TARGET_NAME}>
)
