cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(ProxyRP VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CMakeDependentOption)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

set(PROXYRP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_INSTALL_PREFIX "${PROXYRP_ROOT_DIR}/bin")
set(BINARY_ROOT_DIR "${CMAKE_INSTALL_PREFIX}/")

set(ENGINE_ROOT_DIR "${PROXYRP_ROOT_DIR}")

add_subdirectory(3rdparty)
add_subdirectory(src/shaders)
add_subdirectory(src)

