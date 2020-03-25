FVAD_DIR="$(PWD)/fvad"
OPUS_DIR="$(PWD)/opus"
SSL_DIR="$(PWD)/openssl"
OPENCORE_AMR_DIR="$(PWD)/opencore-amr"
PLATFORM_SUFFIX=
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)
DEFAULT_CXXFLAGS=-fPIC -O3
DEFAULT_CFLAGS=-fPIC -O3
PJPROJECT_CONFIGURE=

ifeq ($(UNAME_S),Linux)
	UNAME_I = $(shell uname -i)
	PLATFORM_SUFFIX = $(UNAME_M)-$(UNAME_I)-linux-gnu
	PJPROJECT_CONFIGURE=\
./configure \
--enable-epoll \
--enable-shared \
--with-opencore-amr=$(OPENCORE_AMR_DIR) \
--with-opus=$(OPUS_DIR) \
--with-ssl=$(SSL_DIR) \
CXXFLAGS="-std=c++17 $(DEFAULT_CXXFLAGS)" \
CFLAGS="$(DEFAULT_CXXFLAGS) -DPJSUA_MAX_CALLS=256 -DPJMEDIA_CODEC_L16_HAS_16KHZ_MONO"
endif
ifeq ($(UNAME_S),Darwin)
	UNAME_R := $(shell uname -r)
	PLATFORM_SUFFIX=$(UNAME_M)-apple-darwin$(UNAME_R)
	PJPROJECT_CONFIGURE=\
./configure \
--enable-shared \
--with-opencore-amr=$(OPENCORE_AMR_DIR) \
--with-opus=$(OPUS_DIR) \
--with-ssl=$(SSL_DIR) \
CXXFLAGS="-std=c++17 $(DEFAULT_CXXFLAGS)" \
CFLAGS="$(DEFAULT_CFLAGS) -DPJSUA_MAX_CALLS=256 -DPJMEDIA_CODEC_L16_HAS_16KHZ_MONO"
endif

all: clean configure dep build-ext assemble
debug: DEFAULT_CFLAGS += -DDEBUG -g
debug: DEFAULT_CXXFLAGS += -DDEBUG -g
debug: clean debug-configure debug-dep build-ext assemble

ext: build-deps build-ext assemble

dep: build-opencore build-fvad build-opus build-ssl build-pjproject move-include move-libs
debug-dep: DEFAULT_CFLAGS += -DDEBUG -g
debug-dep: DEFAULT_CXXFLAGS += -DDEBUG -g
debug-dep: dep

configure: configure-opencore configure-fvad configure-opus configure-ssl configure-pjproject
debug-configure: DEFAULT_CFLAGS += -DDEBUG -g
debug-configure: DEFAULT_CXXFLAGS += -DDEBUG -g
debug-configure: configure

clean: clean-libs clean-include clean-opencore clean-fvad clean-opus clean-ssl clean-pjproject

build-deps: build-opencore build-fvad build-opus build-ssl build-pj move-include move-libs

clean-pjproject:
	- cd pjproject; $(MAKE) clean

configure-pjproject:
	- cd pjproject;\
	$(PJPROJECT_CONFIGURE);\
	$(MAKE) clean

build-pj:
	- cd pjproject; $(MAKE)

build-pjproject:
	- cd pjproject; $(MAKE) dep && $(MAKE)

clean-opencore:
	- cd opencore-amr; $(MAKE) clean
	- cd opencore-amr; rm -r lib

configure-opencore:
	cd opencore-amr;\
	autoreconf -i;\
	./configure --enable-static --enable-shared CXXFLAGS="$(DEFAULT_CXXFLAGS)" CFLAGS="$(DEFAULT_CFLAGS)";\
	$(MAKE) clean

build-opencore:	
	- cd opencore-amr; $(MAKE)
	- cd opencore-amr; mkdir lib
	- cd opencore-amr; cp ./amrnb/.libs/libopencore-amrnb.a ./lib/libopencore-amrnb.a

clean-fvad:
	- cd fvad; $(MAKE) clean

configure-fvad:
	cd fvad;\
	autoreconf -i;\
	./configure --enable-static --enable-shared --disable-rtcd --disable-intrinsics CXXFLAGS="$(DEFAULT_CXXFLAGS)" CFLAGS="$(DEFAULT_CFLAGS)";\
	$(MAKE) clean

build-fvad:
	cd fvad;\
	$(MAKE)

configure-opus:
	cd opus;\
	autoreconf -i;\
	./configure --enable-static --enable-shared --disable-rtcd --disable-intrinsics CXXFLAGS="$(DEFAULT_CXXFLAGS)" CFLAGS="$(DEFAULT_CFLAGS)";\
	$(MAKE) clean

clean-opus:
	- cd opus; $(MAKE) clean; rm -r lib

build-opus:
	- cd opus; $(MAKE)
	- cd opus; mkdir lib
	- cd opus; cp ./.libs/libopus.a ./lib/libopus.a

clean-ssl:
	- cd openssl; $(MAKE) clean
	- cd openssl; rm -r lib

configure-ssl:
	cd openssl; export CFLAGS="-fPIC -Wall -O3"; ./config; $(MAKE) clean

build-ssl:
	- cd openssl;\
	export CFLAGS="$(DEFAULT_CXXFLAGS)";\
	$(MAKE)
	- cd openssl; mkdir lib
	- cd openssl;\
	cp ./libcrypto.a ./lib/libcrypto.a;\
	cp ./libssl.a ./lib/libssl.a

clean-ext:
	- cd pjproject-ext; make clean
	- cd pjproject-ext; rm libpj-ext.a

build-ext:
	- mkdir pjproject-ext/build
	cd pjproject-ext; cmake -S . -B build; cd build; $(MAKE) clean; $(MAKE)

xcode:
	- mkdir pjproject-ext/xcode
	cd pjproject-ext; cmake -S . -B xcode -G Xcode

clean-libs:
	- rm -r libs

clean-include:
	- rm -r include

move-include:
	- rm -r include
	- mkdir include
	cp -r fvad/include ./
	cp -r opus/include ./include/opus
	cp -r pjproject/pjlib/include ./
	cp -r pjproject/pjlib-util/include ./
	cp -r pjproject/pjmedia/include ./
	cp -r pjproject/pjnath/include ./
	cp -r pjproject/pjsip/include ./
	cp -r openssl/include ./include/openssl
	cp pjproject-ext/pj-ext.h include/pjsua2/pidato.hpp
	- rm include/pjsua2.hpp
	cp pjproject-ext/pjsua2.hpp include/pjsua2.hpp

move-libs:
	- rm -r libs
	- mkdir libs
	- mkdir libs/fvad
	- mkdir libs/opus
	- mkdir libs/ssl
	- mkdir libs/crypto
	- mkdir libs/amrnb
	- mkdir libs/pj
	- mkdir libs/pjsip
	- mkdir libs/pjsip-ua
	- mkdir libs/pjsip-simple
	- mkdir libs/pjmedia
	- mkdir libs/pjmedia-codec
	- mkdir libs/pjmedia-audiodev
	- mkdir libs/pjmedia-videodev
	- mkdir libs/pjnath
	- mkdir libs/pjlib-util
	- mkdir libs/srtp
	- mkdir libs/resample
	- mkdir libs/gsm
	- mkdir libs/speex
	- mkdir libs/libilbccodec
	- mkdir libs/g7221
	- mkdir libs/webrtc
	- mkdir libs/yuv
	- mkdir libs/pjsua
	- mkdir libs/pjsua2
	- mkdir libs/pj-ext
	cp ./opus/.libs/libopus.a ./libs/opus/libopus.a
	cp ./opus/.libs/libopus.a ./libs/libopus.a
	cp ./fvad/src/.libs/libfvad.a ./libs/fvad/libfvad.a
	cp ./fvad/src/.libs/libfvad.a ./libs/libfvad.a
	cp ./openssl/libssl.a ./libs/ssl/libssl.a
	cp ./openssl/libcrypto.a ./libs/crypto/libcrypto.a
	cp ./opencore-amr/amrnb/.libs/libopencore-amrnb.a ./libs/amrnb/libopencore-amrnb.a
	cp ./pjproject/pjsip/lib/libpjsip-$(PLATFORM_SUFFIX).a ./libs/pjsip/libpjsip.a
	cp ./pjproject/pjsip/lib/libpjsip-ua-$(PLATFORM_SUFFIX).a ./libs/pjsip-ua/libpjsip-ua.a
	cp ./pjproject/pjsip/lib/libpjsip-simple-$(PLATFORM_SUFFIX).a ./libs/pjsip-simple/libpjsip-simple.a
	cp ./pjproject/pjlib/lib/libpj-$(PLATFORM_SUFFIX).a ./libs/pj/libpj.a
	cp ./pjproject/pjmedia/lib/libpjmedia-$(PLATFORM_SUFFIX).a ./libs/pjmedia/libpjmedia.a
	cp ./pjproject/pjmedia/lib/libpjmedia-codec-$(PLATFORM_SUFFIX).a ./libs/pjmedia-codec/libpjmedia-codec.a
	cp ./pjproject/pjmedia/lib/libpjmedia-audiodev-$(PLATFORM_SUFFIX).a ./libs/pjmedia-audiodev/libpjmedia-audiodev.a
	cp ./pjproject/pjmedia/lib/libpjmedia-videodev-$(PLATFORM_SUFFIX).a ./libs/pjmedia-videodev/libpjmedia-videodev.a
	cp ./pjproject/pjnath/lib/libpjnath-$(PLATFORM_SUFFIX).a ./libs/pjnath/libpjnath.a
	cp ./pjproject/pjlib-util/lib/libpjlib-util-$(PLATFORM_SUFFIX).a ./libs/pjlib-util/libpjlib-util.a
	cp ./pjproject/third_party/lib/libsrtp-$(PLATFORM_SUFFIX).a ./libs/srtp/libsrtp.a
	cp ./pjproject/third_party/lib/libresample-$(PLATFORM_SUFFIX).a ./libs/resample/libresample.a
	cp ./pjproject/third_party/lib/libgsmcodec-$(PLATFORM_SUFFIX).a ./libs/gsm/libgsm.a
	cp ./pjproject/third_party/lib/libspeex-$(PLATFORM_SUFFIX).a ./libs/speex/libspeex.a
	cp ./pjproject/third_party/lib/libilbccodec-$(PLATFORM_SUFFIX).a ./libs/libilbccodec/libilbccodec.a
	cp ./pjproject/third_party/lib/libg7221codec-$(PLATFORM_SUFFIX).a ./libs/g7221/libg7221.a
	cp ./pjproject/third_party/lib/libwebrtc-$(PLATFORM_SUFFIX).a ./libs/webrtc/libwebrtc.a
	cp ./pjproject/third_party/lib/libyuv-$(PLATFORM_SUFFIX).a ./libs/yuv/libyuv.a
	cp ./pjproject/pjsip/lib/libpjsua-$(PLATFORM_SUFFIX).a ./libs/pjsua/libpjsua.a
	cp ./pjproject/pjsip/lib/libpjsua2-$(PLATFORM_SUFFIX).a ./libs/pjsua2/libpjsua2.a

assemble:
	cp pjproject-ext/build/libpj-ext.a ./libs/pj-ext/libpj-ext.a
	cd libs/fvad; ar -x libfvad.a
	cd libs/opus; ar -x libopus.a
	cd libs/ssl; ar -x libssl.a
	cd libs/crypto; ar -x libcrypto.a
	cd libs/pj; ar -x libpj.a
	cd libs/pjsip; ar -x libpjsip.a
	cd libs/pjsip-ua; ar -x libpjsip-ua.a
	cd libs/pjsip-simple; ar -x libpjsip-simple.a
	cd libs/pjmedia; ar -x libpjmedia.a
	cd libs/pjmedia-codec; ar -x libpjmedia-codec.a
	cd libs/pjmedia-audiodev; ar -x libpjmedia-audiodev.a
	cd libs/pjmedia-videodev; ar -x libpjmedia-videodev.a
	cd libs/pjnath; ar -x libpjnath.a
	cd libs/pjlib-util; ar -x libpjlib-util.a
	cd libs/srtp; ar -x libsrtp.a
	cd libs/resample; ar -x libresample.a
	cd libs/gsm; ar -x libgsm.a
	cd libs/speex; ar -x libspeex.a
	cd libs/libilbccodec; ar -x libilbccodec.a
	cd libs/g7221; ar -x libg7221.a
	cd libs/webrtc; ar -x libwebrtc.a
	cd libs/yuv; ar -x libyuv.a
	cd libs/pjsua; ar -x libpjsua.a
	cd libs/pjsua2; ar -x libpjsua2.a
	cd libs/pj-ext; ar -x libpj-ext.a
	cd libs/amrnb; ar -x libopencore-amrnb.a
	- rm libs/srtp/aes_icm.o
	cd libs; ar -q libpjproject-2.10.a fvad/*.o opus/*.o crypto/*.o ssl/*.o pj/*.o pjsip/*.o pjsip-ua/*.o pjsip-simple/*.o pjmedia/*.o pjmedia-audiodev/*.o pjmedia-videodev/*.o pjmedia-codec/*.o pjnath/*.o pjlib-util/*.o srtp/*.o resample/*.o gsm/*.o speex/*.o libilbccodec/*.o g7221/*.o webrtc/*.o yuv/*.o pjsua/*.o pjsua2/*.o amrnb/*.o pj-ext/*.o

print:
	@ echo "PLATFORM SUFFIX = $(PLATFORM_SUFFIX)"
	@ echo "FVAD dir = $(FVAD_DIR)"
	@ echo "CFLAGS = $(DEFAULT_CFLAGS)"
