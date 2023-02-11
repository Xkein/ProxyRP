
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty" CACHE STRING "")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT WIN32)
    message(FATAL_ERROR "not support other platform except for Win32 yet.")
endif()

include(vulkan.cmake)

if(NOT TARGET stb)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${THIRD_PARTY_DIR}/stb)
endif()

if(NOT TARGET assimp)
    option(ASSIMP_BUILD_TESTS "" OFF)
    option(ASSIMP_INSTALL "" OFF)
    option(ASSIMP_BUILD_ASSIMP_VIEW "" OFF)
    add_subdirectory(assimp)
    set_target_properties(assimp PROPERTIES FOLDER 3rdparty/assimp)
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER 3rdparty/assimp)
endif()

if(NOT TARGET glm)
    find_file(_glm_hpp "glm.hpp" REQUIRED PATHS ${vulkan_include}/glm)

    add_library(glm INTERFACE)
    target_compile_definitions(glm INTERFACE
        GLM_FORCE_DEPTH_ZERO_TO_ONE)
    target_include_directories(glm INTERFACE ${vulkan_include})
    # set_target_properties(glm PROPERTIES FOLDER 3rdparty)
endif()

if(NOT TARGET spdlog)
    option(SPDLOG_BUILD_EXAMPLE "" OFF)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER 3rdparty)
endif()

if(NOT TARGET eigen)
    add_library(eigen INTERFACE)
    target_include_directories(eigen INTERFACE ${THIRD_PARTY_DIR}/eigen)
endif()

if(NOT TARGET json)
    set(JSON_Install OFF CACHE INTERNAL "")
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    add_subdirectory(json)
    add_library (json ALIAS nlohmann_json)
endif()

if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER 3rdparty/glfw)
    set_target_properties(update_mappings PROPERTIES FOLDER 3rdparty/glfw)
endif()

include(imgui.cmake)

include(boost.cmake)

include(rpclib.cmake)

if(NOT TARGET rttr)
    option(BUILD_STATIC "" ON)
    option(BUILD_WITH_STATIC_RUNTIME_LIBS "" OFF)
    option(BUILD_UNIT_TESTS "" OFF)
    option(BUILD_BENCHMARKS "" OFF)
    option(BUILD_EXAMPLES "" OFF)
    option(BUILD_DOCUMENTATION "" OFF)
    option(BUILD_INSTALLER "" OFF)
    option(BUILD_PACKAGE "" OFF)
    option(USE_PCH "" OFF)
    add_subdirectory(rttr)
    set_target_properties(rttr_core PROPERTIES FOLDER 3rdparty/rttr)
    set_target_properties(rttr_core_lib PROPERTIES FOLDER 3rdparty/rttr)
endif()

if(NOT TARGET taskflow)
    add_library(taskflow INTERFACE)
    target_include_directories(taskflow INTERFACE ${THIRD_PARTY_DIR}/taskflow/)
endif()

if(NOT TARGET mustache)
    add_library(mustache INTERFACE)
    target_include_directories(mustache INTERFACE ${THIRD_PARTY_DIR}/Mustache/)
endif()

include(llvm.cmake)

include(ShaderConductor.cmake)
