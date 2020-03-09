#!/usr/bin/env bash

rm
[ -d libs ] || mkdir libs
mkdir libs/fvad
mkdir libs/opus
mkdir libs/ssl
mkdir libs/crypto
mkdir libs/pj
mkdir libs/pjsip
mkdir libs/pjsip-ua
mkdir libs/pjsip-simple
mkdir libs/pjmedia
mkdir libs/pjmedia-codec
mkdir libs/pjmedia-audiodev
mkdir libs/pjnath
mkdir libs/pjlib-util
mkdir libs/srtp
mkdir libs/resample
mkdir libs/gsm
mkdir libs/speex
mkdir libs/libilbccodec
mkdir libs/g7221
mkdir libs/webrtc
mkdir libs/pjsua
mkdir libs/pjsua2
cp ./opus/.libs/libopus.a ./libs/opus/libopus.a
cp ./fvad/src/.libs/libfvad.a ./libs/fvad/libfvad.a
cp ./pjproject/pjlib/lib/libfvad.a ./libs/fvad/libfvad.a
cp ./pjproject/pjsip/lib/libpjsip-x86_64-unknown-linux-gnu.a ./libs/pjsip/libpjsip.a
cp ./pjproject/pjsip/lib/libpjsip-ua-x86_64-unknown-linux-gnu.a ./libs/pjsip-ua/libpjsip-ua.a
cp ./pjproject/pjsip/lib/libpjsip-simple-x86_64-unknown-linux-gnu.a ./libs/pjsip-simple/libpjsip-simple.a
cp ./pjproject/pjlib/lib/libpj-x86_64-unknown-linux-gnu.a ./libs/pj/libpj.a
cp ./pjproject/pjmedia/lib/libpjmedia-x86_64-unknown-linux-gnu.a ./libs/pjmedia/libpjmedia.a
cp ./pjproject/pjmedia/lib/libpjmedia-codec-x86_64-unknown-linux-gnu.a ./libs/pjmedia-codec/libpjmedia-codec.a
cp ./pjproject/pjmedia/lib/libpjmedia-audiodev-x86_64-unknown-linux-gnu.a ./libs/pjmedia-audiodev/libpjmedia-audiodev.a
cp ./pjproject/pjnath/lib/libpjnath-x86_64-unknown-linux-gnu.a ./libs/pjnath/libpjnath.a
cp ./pjproject/pjlib-util/lib/libpjlib-util-x86_64-unknown-linux-gnu.a ./libs/pjlib-util/libpjlib-util.a
cp ./pjproject/third_party/lib/libsrtp-x86_64-unknown-linux-gnu.a ./libs/srtp/libsrtp.a
cp ./pjproject/third_party/lib/libresample-x86_64-unknown-linux-gnu.a ./libs/resample/libresample.a
cp ./pjproject/third_party/lib/libgsmcodec-x86_64-unknown-linux-gnu.a ./libs/gsm/libgsm.a
cp ./pjproject/third_party/lib/libspeex-x86_64-unknown-linux-gnu.a ./libs/speex/libspeex.a
cp ./pjproject/third_party/lib/libilbccodec-x86_64-unknown-linux-gnu.a ./libs/libilbccodec/libilbccodec.a
cp ./pjproject/third_party/lib/libg7221codec-x86_64-unknown-linux-gnu.a ./libs/g7221/libg7221.a
cp ./pjproject/third_party/lib/libwebrtc-x86_64-unknown-linux-gnu.a ./libs/webrtc/libwebrtc.a
cp ./pjproject/pjsip/lib/libpjsua-x86_64-unknown-linux-gnu.a ./libs/pjsua/libpjsua.a
cp ./pjproject/pjsip/lib/libpjsua2-x86_64-unknown-linux-gnu.a ./libs/pjsua2/libpjsua2.a
cd libs/fvad
ar -x libfvad.a
cd ../opus
ar -x libopus.a
cd ../opus
ar -x libopus.a
cd ../ssl
ar -x libssl.a
cd ../crypto
ar -x libcrypto.a
cd ../pj
ar -x libpj.a
cd ../pjsip
ar -x libpjsip.a
cd ../pjsip-ua
ar -x libpjsip-ua.a
cd ../pjsip-simple
ar -x libpjsip-simple.a
cd ../pjmedia
ar -x libpjmedia.a
cd ../pjmedia-codec
ar -x libpjmedia-codec.a
cd ../pjmedia-audiodev
ar -x libpjmedia-audiodev.a
cd ../pjnath
ar -x libpjnath.a
cd ../pjlib-util
ar -x libpjlib-util.a
cd ../srtp
ar -x libsrtp.a
cd ../resample
ar -x libresample.a
cd ../gsm
ar -x libgsm.a
cd ../speex
ar -x libspeex.a
cd ../libilbccodec
ar -x libilbccodec.a
cd ../g7221
ar -x libg7221.a
cd ../webrtc
ar -x libwebrtc.a
cd ../pjsua
ar -x libpjsua.a
cd ../pjsua2
ar -x libpjsua2.a
cd ../
ar -qc libpidato-lib.a fvad/*.o opus/*.o crypto/*.o ssl/*.o pj/*.o pjsip/*.o pjsip-ua/*.o pjsip-simple/*.o pjmedia/*.o pjmedia-audiodev/*.o pjmedia-codec/*.o pjnath/*.o pjlib-util/*.o srtp/*.o resample/*.o gsm/*.o speex/*.o libilbccodec/*.o g7221/*.o webrtc/*.o pjsua/*.o pjsua2/*.o
