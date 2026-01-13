cmake_minimum_required(VERSION 3.14.0)

project(JubSDK VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/./outputs/${CMAKE_SYSTEM_NAME})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/./outputs/${CMAKE_SYSTEM_NAME})

if("$ENV{PREFIX}" STREQUAL "")
    set(PREFIX "${CMAKE_SOURCE_DIR}/build/local")
else()
    set(PREFIX "$ENV{PREFIX}")
endif()


foreach(opt IN ITEMS ENABLE_COMMONCRYPTO ENABLE_GNUTLS ENABLE_MBEDTLS ENABLE_OPENSSL ENABLE_WINDOWS_CRYPTO)
    set(${opt} OFF CACHE BOOL "" FORCE)
endforeach()


set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries")
add_subdirectory(deps/libzip)

#不知道为什么，Libzip的Cmake测试不准，手动修改变量重新生成config
unset(HAVE_MEMCPY_S CACHE)
unset(HAVE_STRNCPY_S CACHE)
unset(HAVE_STRERROR_S CACHE)
unset(HAVE_FTS_H CACHE)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/deps/libzip/config.h.in ${PROJECT_BINARY_DIR}/deps/libzip/config.h)

set(ENABLE_TESTING OFF CACHE BOOL "Disable building mbedtls tests")
set(ENABLE_PROGRAMS OFF CACHE BOOL "Disable building mbedtls programs")
add_subdirectory(deps/secure-channel/mbedtls)
add_subdirectory(src)
add_subdirectory(trezor-crypto)



# add_subdirectory(JubSDKCore)
# add_subdirectory(test)
