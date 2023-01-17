
set(TARGET_NAME HeaderTool)

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Tool")

target_link_libraries(${TARGET_NAME} PRIVATE llvm)
target_link_libraries(${TARGET_NAME} PRIVATE mustache)
