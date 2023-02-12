
if(NOT TARGET boost)
    add_library(boost INTERFACE)
    target_include_directories(boost INTERFACE
        ${THIRD_PARTY_DIR}/boost/libs/asio/include
        ${THIRD_PARTY_DIR}/boost/libs/align/include
        ${THIRD_PARTY_DIR}/boost/libs/assert/include
        ${THIRD_PARTY_DIR}/boost/libs/config/include
        ${THIRD_PARTY_DIR}/boost/libs/core/include
        ${THIRD_PARTY_DIR}/boost/libs/static_assert/include
        ${THIRD_PARTY_DIR}/boost/libs/throw_exception/include
        ${THIRD_PARTY_DIR}/boost/libs/array/include
        ${THIRD_PARTY_DIR}/boost/libs/bind/include
        ${THIRD_PARTY_DIR}/boost/libs/chrono/include
        ${THIRD_PARTY_DIR}/boost/libs/integer/include
        ${THIRD_PARTY_DIR}/boost/libs/move/include
        ${THIRD_PARTY_DIR}/boost/libs/mpl/include
        ${THIRD_PARTY_DIR}/boost/libs/predef/include
        ${THIRD_PARTY_DIR}/boost/libs/preprocessor/include
        ${THIRD_PARTY_DIR}/boost/libs/type_traits/include
        ${THIRD_PARTY_DIR}/boost/libs/utility/include
        ${THIRD_PARTY_DIR}/boost/libs/io/include
        ${THIRD_PARTY_DIR}/boost/libs/ratio/include
        ${THIRD_PARTY_DIR}/boost/libs/rational/include
        ${THIRD_PARTY_DIR}/boost/libs/system/include
        ${THIRD_PARTY_DIR}/boost/libs/variant2/include
        ${THIRD_PARTY_DIR}/boost/libs/mp11/include
        ${THIRD_PARTY_DIR}/boost/libs/winapi/include
        ${THIRD_PARTY_DIR}/boost/libs/typeof/include
        ${THIRD_PARTY_DIR}/boost/libs/coroutine/include
        ${THIRD_PARTY_DIR}/boost/libs/context/include
        ${THIRD_PARTY_DIR}/boost/libs/pool/include
        ${THIRD_PARTY_DIR}/boost/libs/smart_ptr/include
        ${THIRD_PARTY_DIR}/boost/libs/exception/include
        ${THIRD_PARTY_DIR}/boost/libs/tuple/include
        ${THIRD_PARTY_DIR}/boost/libs/date_time/include
        ${THIRD_PARTY_DIR}/boost/libs/algorithm/include
        ${THIRD_PARTY_DIR}/boost/libs/concept_check/include
        ${THIRD_PARTY_DIR}/boost/libs/function/include
        ${THIRD_PARTY_DIR}/boost/libs/type_index/include
        ${THIRD_PARTY_DIR}/boost/libs/container_hash/include
        ${THIRD_PARTY_DIR}/boost/libs/describe/include
        ${THIRD_PARTY_DIR}/boost/libs/iterator/include
        ${THIRD_PARTY_DIR}/boost/libs/conversion/include
        ${THIRD_PARTY_DIR}/boost/libs/detail/include
        ${THIRD_PARTY_DIR}/boost/libs/function_types/include
        ${THIRD_PARTY_DIR}/boost/libs/fusion/include
        ${THIRD_PARTY_DIR}/boost/libs/functional/include
        ${THIRD_PARTY_DIR}/boost/libs/optional/include
        ${THIRD_PARTY_DIR}/boost/libs/range/include
        ${THIRD_PARTY_DIR}/boost/libs/regex/include
        ${THIRD_PARTY_DIR}/boost/libs/unordered/include
        ${THIRD_PARTY_DIR}/boost/libs/lexical_cast/include
        ${THIRD_PARTY_DIR}/boost/libs/container/include
        ${THIRD_PARTY_DIR}/boost/libs/intrusive/include
        ${THIRD_PARTY_DIR}/boost/libs/numeric/conversion/include
        ${THIRD_PARTY_DIR}/boost/libs/tokenizer/include
        ${THIRD_PARTY_DIR}/boost/libs/interprocess/include
        ${THIRD_PARTY_DIR}/boost/libs/process/include
        ${THIRD_PARTY_DIR}/boost/libs/filesystem/include
    )

    ExternalProject_Add(Boost
        PREFIX Boost
        SOURCE_DIR ${THIRD_PARTY_DIR}/boost
        BINARY_DIR ${THIRD_PARTY_DIR}/boost/build
        CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_CONFIGURATION_TYPES=Release
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_DIR}/boost/build
        BUILD_COMMAND ${CMAKE_COMMAND} --build ${THIRD_PARTY_DIR}/boost/build --config Release
    )
    add_dependencies(boost Boost)
    
    if (CMAKE_HOST_WIN32)
        #target_link_libraries(boost INTERFACE
        #    ${THIRD_PARTY_DIR}/boost/libs/chrono/Release/libboost_chrono-vc143-mt-gd-x64-1_81d.lib
        #    ${THIRD_PARTY_DIR}/boost/libs/coroutine/Release/libboost_coroutine-vc143-mt-gd-x64-1_81d.lib
        #    ${THIRD_PARTY_DIR}/boost/libs/context/Release/libboost_context-vc143-mt-gd-x64-1_81d.lib
        #    ${THIRD_PARTY_DIR}/boost/libs/date_time/Release/libboost_date_time-vc143-mt-gd-x64-1_81d.lib
        #    ${THIRD_PARTY_DIR}/boost/libs/container/Release/libboost_container-vc143-mt-gd-x64-1_81d.lib
        #    ${THIRD_PARTY_DIR}/boost/libs/exception/Release/libboost_exception-vc143-mt-gd-x64-1_81d.lib
        #    ${THIRD_PARTY_DIR}/boost/libs/filesystem/Release/libboost_filesystem-vc143-mt-gd-x64-1_81d.lib
        #)
    endif()
endif()