
if(NOT TARGET shader_conductor)
    add_library(shader_conductor INTERFACE)
    target_include_directories(shader_conductor INTERFACE ${THIRD_PARTY_DIR}/ShaderConductor/Include/)
    if (COMPILE_SHADER_CONDUCTOR)
        ExternalProject_Add(ShaderConductor
            PREFIX ShaderConductor
            SOURCE_DIR ${THIRD_PARTY_DIR}/ShaderConductor
            BINARY_DIR ${THIRD_PARTY_DIR}/ShaderConductor/build
            CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_CONFIGURATION_TYPES=Release
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_INSTALL_PREFIX=${STAGING_DIR}
            -DLLVM_BUILD_BENCHMARKS=OFF
            -DLLVM_BUILD_EXAMPLES=OFF
            -DLLVM_BUILD_INSTRUMENTED_COVERAGE=OFF
            -DLLVM_BUILD_TESTS=OFF
            -DLLVM_BUILD_TOOLS=OFF
            -DLLVM_INCLUDE_BENCHMARKS=OFF
            -DLLVM_INCLUDE_EXAMPLES=OFF
            -DLLVM_INCLUDE_TESTS=OFF
            -DCMAKE_CXX_FLAGS=/wd4189
            #BUILD_IN_SOURCE ON
            BUILD_COMMAND ${CMAKE_COMMAND} --build ${THIRD_PARTY_DIR}/ShaderConductor/build --config Release
        )
        add_dependencies(shader_conductor ShaderConductor)
    endif()
    
    set(SHADER_CONDUCTOR_BIN_DIR ${LLVM_DIR}/build/lib/Release CACHE STRING "")
    if (CMAKE_HOST_WIN32)
        target_link_libraries(llvm INTERFACE ${THIRD_PARTY_DIR}/ShaderConductor/build/lib/Release/ShaderConductor.lib)
    endif()

endif()