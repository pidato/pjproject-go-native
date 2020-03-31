cmake_minimum_required(VERSION 3.10)
project(pj-ext)
# IF (APPLE)
#     message("macOS:")
# ELSEIF (UNIX)
#     message("Unix:")
# ENDIF (APPLE)
# message("\tprefix: ${CMAKE_FIND_LIBRARY_PREFIXES}")
# message("\tsuffix: ${CMAKE_FIND_LIBRARY_SUFFIXES}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
message("Parent Dir: ${PARENT_DIR}")

include_directories(${PARENT_DIR}/include)

link_directories(${PARENT_DIR}/libs/fvad)
link_directories(${PARENT_DIR}/libs/opus)
link_directories(${PARENT_DIR}/libs/ssl)
link_directories(${PARENT_DIR}/libs/crypto)
#link_directories(${PARENT_DIR}/libs/amrnb)
link_directories(${PARENT_DIR}/libs/g7221)
link_directories(${PARENT_DIR}/libs/gsm)
link_directories(${PARENT_DIR}/libs/libilbccodec)
link_directories(${PARENT_DIR}/libs/pj)
link_directories(${PARENT_DIR}/libs/pjlib-util)
link_directories(${PARENT_DIR}/libs/pjmedia)
link_directories(${PARENT_DIR}/libs/pjmedia-audiodev)
link_directories(${PARENT_DIR}/libs/pjmedia-codec)
link_directories(${PARENT_DIR}/libs/pjmedia-videodev)
link_directories(${PARENT_DIR}/libs/pjnath)
link_directories(${PARENT_DIR}/libs/pjsip)
link_directories(${PARENT_DIR}/libs/pjsip-simple)
link_directories(${PARENT_DIR}/libs/pjsip-ua)
link_directories(${PARENT_DIR}/libs/pjsua)
link_directories(${PARENT_DIR}/libs/pjsua2)
link_directories(${PARENT_DIR}/libs/resample)
link_directories(${PARENT_DIR}/libs/speex)
link_directories(${PARENT_DIR}/libs/srtp)
link_directories(${PARENT_DIR}/libs/webrtc)
link_directories(${PARENT_DIR}/libs/yuv)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
set(CMAKE_C_FLAGS "-Wall -fPIC")
set(CMAKE_CXX_FLAGS "-Wall -fPIC")

set(CMAKE_CXX_STANDARD 17)

add_library(pj-ext pj-ext.cpp pj-ext.h)
if (APPLE)
    add_executable(example pj-ext.h pj-ext.cpp main.cpp)
endif (APPLE)
set_property(TARGET pj-ext PROPERTY POSITION_INDEPENDENT_CODE ON)

if (APPLE)
    target_link_libraries(pj-ext
        #     "-framework CoreAudio -framework CoreServices -framework AudioUnit -framework AudioToolbox -framework Foundation -framework AppKit -framework AVFoundation -framework CoreGraphics -framework QuartzCore -framework CoreVideo -framework CoreMedia -framework VideoToolbox -framework Security"
            "-framework CoreAudio -framework CoreServices -framework AudioUnit -framework AudioToolbox -framework Foundation -framework AppKit"
            dl
            m
            pthread
            )

    target_link_libraries(example
        #     "-framework CoreAudio -framework CoreServices -framework AudioUnit -framework AudioToolbox -framework Foundation -framework AppKit -framework AVFoundation -framework CoreGraphics -framework QuartzCore -framework CoreVideo -framework CoreMedia -framework VideoToolbox -framework Security"
            "-framework CoreAudio -framework CoreServices -framework AudioUnit -framework AudioToolbox -framework Foundation -framework AppKit"
            dl
            m
            pthread

#            opencore-amrnb
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
#            pjmedia-videodev
            pjnath
            pjlib-util
            srtp
            resample
            gsm
            speex
            ilbccodec
            g7221
            webrtc
#            yuv
            pjsua
            pjsua2
            )
ELSEIF (UNIX)
    target_link_libraries(pj-ext
            uuid
#            asound
            dl
            m
            pthread
            )

#    target_link_libraries(example
#            uuid
#            asound
#            dl
#            m
#            pthread
#
#            opencore-amrnb
#            fvad
#            opus
#            ssl
#            crypto
#
#            pjsip
#            pjsip-ua
#            pjsip-simple
#            pj
#            pjmedia
#            pjmedia-codec
#            pjmedia-audiodev
#            pjmedia-videodev
#            pjnath
#            pjlib-util
#            srtp
#            resample
#            gsm
#            speex
#            ilbccodec
#            g7221
#            webrtc
#            yuv
#            pjsua
#            pjsua2
#            )
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
