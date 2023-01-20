
if(NOT TARGET llvm)
    set(LLVM_DIR ${THIRD_PARTY_DIR}/llvm-project)
    set(LLVM_BUILD_DIR ${LLVM_DIR}/build/Release CACHE STRING "")
    add_library(llvm INTERFACE)
    target_include_directories(llvm INTERFACE
        ${LLVM_DIR}/clang/include
        ${LLVM_DIR}/llvm/include)
    if (COMPILE_LLVM)
        ExternalProject_Add(llvm-project
            PREFIX llvm-project
            SOURCE_DIR ${LLVM_DIR}/llvm
            BINARY_DIR ${LLVM_DIR}/build
            CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_CONFIGURATION_TYPES=Release
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_INSTALL_PREFIX=${STAGING_DIR}
            -DLLVM_ENABLE_PROJECTS=clang
            -DLLVM_TARGETS_TO_BUILD=host
            -DLLVM_BUILD_BENCHMARKS=OFF
            -DLLVM_BUILD_EXAMPLES=OFF
            -DLLVM_BUILD_INSTRUMENTED_COVERAGE=OFF
            -DLLVM_BUILD_TESTS=OFF
            -DLLVM_BUILD_TOOLS=OFF
            -DLLVM_INCLUDE_BENCHMARKS=OFF
            -DLLVM_INCLUDE_EXAMPLES=OFF
            -DLLVM_INCLUDE_TESTS=OFF
            -DCLANG_BUILD_TOOLS=OFF
            #BUILD_IN_SOURCE ON
            BUILD_COMMAND ${CMAKE_COMMAND} --build ${LLVM_DIR}/build --config Release
        )
        add_dependencies(llvm llvm-project)
    endif()
    
    if (CMAKE_HOST_WIN32)
        target_link_libraries(llvm INTERFACE ${LLVM_BUILD_DIR}/lib/libclang.lib)
    endif()

endif()