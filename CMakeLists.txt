cmake_minimum_required(VERSION 3.13)
project(pj-ext)
IF (APPLE)
    message("macOS:")
ELSEIF (UNIX)
    message("Unix:")
ENDIF (APPLE)
message("\tprefix: ${CMAKE_FIND_LIBRARY_PREFIXES}")
message("\tsuffix: ${CMAKE_FIND_LIBRARY_SUFFIXES}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


include_directories(./include)
#include_directories(./fvad/include)
#include_directories(./opus/include)
#include_directories(./openssl/include)
#include_directories(./pjproject/pjlib/include)
#include_directories(./pjproject/pjlib-util/include)
#include_directories(./pjproject/pjmedia/include)
#include_directories(./pjproject/pjnath/include)
#include_directories(./pjproject/pjsip/include)
#include_directories(./pjproject/third_party/include)

#link_directories(/usr/local/lib)
link_directories(fvad/src/.libs)
link_directories(opus/.libs)
link_directories(openssl)
link_directories(opencore-amr/amrnb/.libs)
link_directories(pjproject/pjlib/lib)
link_directories(pjproject/pjlib-util/lib)
link_directories(pjproject/pjmedia/lib)
link_directories(pjproject/pjnath/lib)
link_directories(pjproject/pjsip/lib)
link_directories(pjproject/third_party/lib)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
set(CMAKE_C_FLAGS "-Wall -fPIC")
set(CMAKE_CXX_FLAGS "-Wall -fPIC")

set(CMAKE_CXX_STANDARD 17)

#add_library(pjsua_rs main.cpp pi_port.cpp pi_port.h thread_aff.cpp thread_aff.h)
add_library(pj-ext library.cpp library.h)
#add_library(pidato-run STATIC library.cpp library.h
#        fvad/src/.libs/libfvad.a
#        opus/.libs/libopus.a
#        openssl/libcrypto.a
#        openssl/libssl.a
#        )
add_executable(pj-ext-run library.h library.cpp main.cpp)
set_property(TARGET pj-ext PROPERTY POSITION_INDEPENDENT_CODE ON)
#set_property(TARGET pidato-run PROPERTY POSITION_INDEPENDENT_CODE ON)
#add_library(pidato-lib library.cpp library.h)
#add_executable(pjsua_rs main.cpp)

#IF (APPLE)
#    find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
#    find_library(CORE_AUDIO_LIBRARY CoreAudio)
#    find_library(APP_SERVICES_LIBRARY ApplicationServices)
#    set(EXTRA_LIBS ${CORE_FOUNDATION_LIBRARY} ${CORE_AUDIO_LIBRARY} ${APP_SERVICES_LIBRARY})
#ENDIF (APPLE)

#target_link_libraries(pidato
#        -lstdc++
#        -luuid
#        -lpthread
#        fvad/src/.libs/libfvad.a
#        opus/.libs/libopus.a
#        openssl/libssl.a
#        openssl/libcrypto.a
#        pjproject/pjsip/lib/libpjsip-x86_64-unknown-linux-gnu.a
#        pjproject/pjsip/lib/libpjsip-ua-x86_64-unknown-linux-gnu.a
#        pjproject/pjsip/lib/libpjsip-simple-x86_64-unknown-linux-gnu.a
#        pjproject/pjlib/lib/libpj-x86_64-unknown-linux-gnu.a
#        pjproject/pjmedia/lib/libpjmedia-x86_64-unknown-linux-gnu.a
#        pjproject/pjmedia/lib/libpjmedia-codec-x86_64-unknown-linux-gnu.a
#        pjproject/pjmedia/lib/libpjmedia-audiodev-x86_64-unknown-linux-gnu.a
#        pjproject/pjnath/lib/libpjnath-x86_64-unknown-linux-gnu.a
#        pjproject/pjlib-util/lib/libpjlib-util-x86_64-unknown-linux-gnu.a
#        pjproject/third_party/lib/libsrtp-x86_64-unknown-linux-gnu.a
#        pjproject/third_party/lib/libresample-x86_64-unknown-linux-gnu.a
#        pjproject/third_party/lib/libgsmcodec-x86_64-unknown-linux-gnu.a
#        pjproject/third_party/lib/libspeex-x86_64-unknown-linux-gnu.a
#        pjproject/third_party/lib/libilbccodec-x86_64-unknown-linux-gnu.a
#        pjproject/third_party/lib/libg7221codec-x86_64-unknown-linux-gnu.a
#        pjproject/third_party/lib/libwebrtc-x86_64-unknown-linux-gnu
#        pjproject/pjsip/lib/libpjsua-x86_64-unknown-linux-gnu
#        pjproject/pjsip/lib/libpjsua2-x86_64-unknown-linux-gnu
#)


if (APPLE)
    target_link_libraries(pj-ext
            "-framework CoreAudio -framework CoreServices -framework AudioUnit -framework AudioToolbox -framework Foundation -framework AppKit -framework AVFoundation -framework CoreGraphics -framework QuartzCore -framework CoreVideo -framework CoreMedia -framework VideoToolbox -framework Security"
            dl
            m
            pthread
#            opencore-amrnb
#            fvad
#            opus
#            ssl
#            crypto
#            pjsip-x86_64-apple-darwin19.3.0
#            pjsip-ua-x86_64-apple-darwin19.3.0
#            pjsip-simple-x86_64-apple-darwin19.3.0
#            pj-x86_64-apple-darwin19.3.0
#            pjmedia-x86_64-apple-darwin19.3.0
#            pjmedia-codec-x86_64-apple-darwin19.3.0
#            pjmedia-audiodev-x86_64-apple-darwin19.3.0
#            pjnath-x86_64-apple-darwin19.3.0
#            pjlib-util-x86_64-apple-darwin19.3.0
#            srtp-x86_64-apple-darwin19.3.0
#            resample-x86_64-apple-darwin19.3.0
#            gsmcodec-x86_64-apple-darwin19.3.0
#            speex-x86_64-apple-darwin19.3.0
#            ilbccodec-x86_64-apple-darwin19.3.0
#            g7221codec-x86_64-apple-darwin19.3.0
#            webrtc-x86_64-apple-darwin19.3.0
#            pjsua-x86_64-apple-darwin19.3.0
#            pjsua2-x86_64-apple-darwin19.3.0
            )

    target_link_libraries(pj-ext-run
            "-framework CoreAudio -framework CoreServices -framework AudioUnit -framework AudioToolbox -framework Foundation -framework AppKit -framework AVFoundation -framework CoreGraphics -framework QuartzCore -framework CoreVideo -framework CoreMedia -framework VideoToolbox -framework Security"
            dl
            m
            pthread
            opencore-amrnb
            fvad
            opus
            ssl
            crypto
            pjsip-x86_64-apple-darwin19.3.0
            pjsip-ua-x86_64-apple-darwin19.3.0
            pjsip-simple-x86_64-apple-darwin19.3.0
            pj-x86_64-apple-darwin19.3.0
            pjmedia-x86_64-apple-darwin19.3.0
            pjmedia-codec-x86_64-apple-darwin19.3.0
            pjmedia-audiodev-x86_64-apple-darwin19.3.0
            pjnath-x86_64-apple-darwin19.3.0
            pjlib-util-x86_64-apple-darwin19.3.0
            srtp-x86_64-apple-darwin19.3.0
            resample-x86_64-apple-darwin19.3.0
            gsmcodec-x86_64-apple-darwin19.3.0
            speex-x86_64-apple-darwin19.3.0
            ilbccodec-x86_64-apple-darwin19.3.0
            g7221codec-x86_64-apple-darwin19.3.0
            webrtc-x86_64-apple-darwin19.3.0
            pjsua-x86_64-apple-darwin19.3.0
            pjsua2-x86_64-apple-darwin19.3.0
            )
ELSEIF (UNIX)
    target_link_libraries(pj-ext
            uuid
            asound
            dl
            m
            pthread
            fvad
            opus
            ssl
            crypto
            pjsip
            pjsip-ua
            pjsip-simple
            pj
            pjmedia
            pjmedia-codec
            pjmedia-audiodev
            pjmedia-videodev
            pjnath
            pjlib-util
            srtp
            resample
            gsmcodec
            speex
            ilbccodec
            g7221codec
            webrtc
            yuv
            pjsua
            pjsua2
            )

    target_link_libraries(pj-ext-run
            uuid
            asound
            dl
            m
            pthread
            fvad
            opus
            ssl
            crypto
            pjsip
            pjsip-ua
            pjsip-simple
            pj
            pjmedia
            pjmedia-codec
            pjmedia-audiodev
            pjmedia-videodev
            pjnath
            pjlib-util
            srtp
            resample
            gsmcodec
            speex
            ilbccodec
            g7221codec
            webrtc
            yuv
            pjsua
            pjsua2
            )
    #    target_link_libraries(pidato
    #            uuid
    #            pthread
    #            fvad
    #            opus
    #            ssl
    #            crypto
    #            pjsip-x86_64-unknown-linux-gnu
    #            pjsip-ua-x86_64-unknown-linux-gnu
    #            pjsip-simple-x86_64-unknown-linux-gnu
    #            pj-x86_64-unknown-linux-gnu
    #            pjmedia-x86_64-unknown-linux-gnu
    #            pjmedia-codec-x86_64-unknown-linux-gnu
    #            pjmedia-audiodev-x86_64-unknown-linux-gnu
    #            pjnath-x86_64-unknown-linux-gnu
    #            pjlib-util-x86_64-unknown-linux-gnu
    #            srtp-x86_64-unknown-linux-gnu
    #            resample-x86_64-unknown-linux-gnu
    #            gsmcodec-x86_64-unknown-linux-gnu
    #            speex-x86_64-unknown-linux-gnu
    #            ilbccodec-x86_64-unknown-linux-gnu
    #            g7221codec-x86_64-unknown-linux-gnu
    #            webrtc-x86_64-unknown-linux-gnu
    #            pjsua-x86_64-unknown-linux-gnu
    #            pjsua2-x86_64-unknown-linux-gnu
    #            )
    #
    #    target_link_libraries(pidato-run
    #            stdc++
    #            uuid
    #            pthread
    #            fvad
    #            opus
    #            ssl
    #            crypto
    #            pjsip-x86_64-unknown-linux-gnu
    #            pjsip-ua-x86_64-unknown-linux-gnu
    #            pjsip-simple-x86_64-unknown-linux-gnu
    #            pj-x86_64-unknown-linux-gnu
    #            pjmedia-x86_64-unknown-linux-gnu
    #            pjmedia-codec-x86_64-unknown-linux-gnu
    #            pjmedia-audiodev-x86_64-unknown-linux-gnu
    #            pjnath-x86_64-unknown-linux-gnu
    #            pjlib-util-x86_64-unknown-linux-gnu
    #            srtp-x86_64-unknown-linux-gnu
    #            resample-x86_64-unknown-linux-gnu
    #            gsmcodec-x86_64-unknown-linux-gnu
    #            speex-x86_64-unknown-linux-gnu
    #            ilbccodec-x86_64-unknown-linux-gnu
    #            g7221codec-x86_64-unknown-linux-gnu
    #            webrtc-x86_64-unknown-linux-gnu
    #            pjsua-x86_64-unknown-linux-gnu
    #            pjsua2-x86_64-unknown-linux-gnu
    #            )
ENDIF (APPLE)
