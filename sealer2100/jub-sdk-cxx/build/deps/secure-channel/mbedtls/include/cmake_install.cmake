# Install script for directory: /Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

set(CMAKE_BINARY_DIR "/Users/mackun/HyperPay-Jub_SDK_CXX/build")

if(NOT PLATFORM_NAME)
  if(NOT "$ENV{PLATFORM_NAME}" STREQUAL "")
    set(PLATFORM_NAME "$ENV{PLATFORM_NAME}")
  endif()
  if(NOT PLATFORM_NAME)
    set(PLATFORM_NAME iphoneos)
  endif()
endif()

if(NOT EFFECTIVE_PLATFORM_NAME)
  if(NOT "$ENV{EFFECTIVE_PLATFORM_NAME}" STREQUAL "")
    set(EFFECTIVE_PLATFORM_NAME "$ENV{EFFECTIVE_PLATFORM_NAME}")
  endif()
  if(NOT EFFECTIVE_PLATFORM_NAME)
    set(EFFECTIVE_PLATFORM_NAME -iphoneos)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/aes.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/aesni.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/arc4.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/asn1.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/asn1write.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/base64.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/bignum.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/blowfish.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/bn_mul.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/camellia.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ccm.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/certs.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/check_config.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/cipher.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/cipher_internal.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/cmac.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/compat-1.3.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/config.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ctr_drbg.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/debug.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/des.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/dhm.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ecdh.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ecdsa.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ecjpake.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ecp.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/entropy.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/entropy_poll.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/error.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/gcm.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/havege.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/hmac_drbg.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/md.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/md2.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/md4.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/md5.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/md_internal.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/memory_buffer_alloc.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/net.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/net_sockets.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/oid.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/padlock.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/pem.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/pk.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/pk_internal.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/pkcs11.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/pkcs12.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/pkcs5.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/platform.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/platform_time.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ripemd160.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/rsa.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/sha1.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/sha256.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/sha512.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ssl.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ssl_cache.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ssl_ciphersuites.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ssl_cookie.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ssl_internal.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/ssl_ticket.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/threading.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/timing.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/version.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/x509.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/x509_crl.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/x509_crt.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/x509_csr.h"
    "/Users/mackun/HyperPay-Jub_SDK_CXX/deps/secure-channel/mbedtls/include/mbedtls/xtea.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/mackun/HyperPay-Jub_SDK_CXX/build/deps/secure-channel/mbedtls/include/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
