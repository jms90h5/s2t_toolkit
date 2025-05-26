# NOTE: This makefile is designed to be run from one level up in the directory
# hierarchy compared to where it resides. For instance, if this makefile is
# located at 'output/build/Makefile', then it can be launched as:
# 'make -f build/Makefile' from the 'output' directory.

SHELL=/bin/bash
ifeq ($(CXX),)
  CXX=g++
endif
ifeq ($(CXX_LN),)
  CXX_LN=g++
endif
PKGCONFIG := /homes/jsharpe/teracloud/streams/7.2.0.0/bin/dst-pe-pkg-config-alternative.sh
SPL_BUNDLE_BUILDER := /homes/jsharpe/teracloud/streams/7.2.0.0/system/impl/bin/spl-bundle-builder
SO_INCLUDE := $(shell $(PKGCONFIG) --cflags dst-spl-pe-install)
SO_LIB := $(shell $(PKGCONFIG) --libs dst-spl-pe-install)
SO_STAND_LIB := $(shell $(PKGCONFIG) --libs dst-spl-pe-install-standalone)
SPL_CXXFLAGS = -Wall -Wno-unused -Wno-sign-compare
SPL_CXXFLAGS += -D_REENTRANT -fPIC -finline-functions -fmessage-length=0
SPL_CXXFLAGS += $(SO_INCLUDE) -O3 -DNDEBUG  -std=gnu++17 
SPL_LDFLAGS = $(SO_LIB)

SPL_TYPES = build/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.o \
            build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o \
            build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o 
SPL_FUNCTIONS = 
SPL_OPERATORS = build/operator/Display.o \
                build/operator/AudioStream/RawAudioBlocks.o \
                build/operator/AudioStream/AudioStream.o 
SPL_SO_BINS = bin/Display.so \
              bin/AudioStream.RawAudioBlocks.so \
              bin/AudioStream.AudioStream.so 

SPL_ADL_FILE = TestFileAudioSource.adl

SPL_OUTPUT_DIR_ROOT = $(shell printf '%q' '/homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t/samples/BasicWorkingExample')
SPL_OUTPUT_DIR = $(shell printf '%q' 'test_output')

.PHONY: distclean clean all types functions operators sos bundle toolkits standalone sablink

all: bin types functions operators sos standalone bundle sablink

distclean clean:
	@rm -fr bin/* build/type build/function build/operator build/bundle build/standalone TestFileAudioSource.sab

bin:
	@mkdir -m 755 bin

types: $(SPL_TYPES)

build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o: \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.h \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] enum{csv,txt,bin,block,line}'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.cpp

build/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.o: \
	    src/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.h \
	    src/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] tuple<blob data>'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.cpp

build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o: \
	    src/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.h \
	    src/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] tuple<blob audioChunk,uint64 audioTimestamp>'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.cpp

build/type:
	-@mkdir -m 755 build/type

operators: $(SPL_OPERATORS)

-include build/operator/Display.d
build/operator/Display.o: \
	    src/operator/Display.h \
	    src/operator/Display.cpp\
	    build/dep/operator/Display.dep  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] Display'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"TestFileAudioSource.adl\" -c $(SPL_CXXFLAGS) src/operator/Display.cpp

-include build/operator/AudioStream/RawAudioBlocks.d
build/operator/AudioStream/RawAudioBlocks.o: \
	    src/operator/AudioStream/RawAudioBlocks.h \
	    src/operator/AudioStream/RawAudioBlocks.cpp\
	    build/dep/operator/AudioStream/RawAudioBlocks.dep \
	    src/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.h \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.h  \
	    build/cppOptions \
	    | build/operator/AudioStream
	@echo ' [CXX-operator] AudioStream.RawAudioBlocks'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"TestFileAudioSource.adl\" -c $(SPL_CXXFLAGS) src/operator/AudioStream/RawAudioBlocks.cpp

-include build/operator/AudioStream/AudioStream.d
build/operator/AudioStream/AudioStream.o: \
	    src/operator/AudioStream/AudioStream.h \
	    src/operator/AudioStream/AudioStream.cpp\
	    build/dep/operator/AudioStream/AudioStream.dep \
	    src/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.h  \
	    build/cppOptions \
	    | build/operator/AudioStream
	@echo ' [CXX-operator] AudioStream.AudioStream'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"TestFileAudioSource.adl\" -c $(SPL_CXXFLAGS) src/operator/AudioStream/AudioStream.cpp

build/operator/AudioStream:
	-@mkdir -m 755 -p build/operator/AudioStream

build/operator:
	-@mkdir -m 755 -p build/operator

functions: $(SPL_FUNCTIONS)

sos: $(SPL_SO_BINS)

SODisplay = \
	    build/operator/Display.o \
	    build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o 

bin/Display.so: \
	$(SODisplay) \
	build/dep/Display.so \
	build/ldOptions
	@echo ' [LD-so] so Display'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/Display.so -fPIC -shared $(SODisplay) $(SPL_LDFLAGS)

SOAudioStream.RawAudioBlocks = \
	    build/operator/AudioStream/RawAudioBlocks.o \
	    build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o \
	    build/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.o 

bin/AudioStream.RawAudioBlocks.so: \
	$(SOAudioStream.RawAudioBlocks) \
	build/dep/AudioStream.RawAudioBlocks.so \
	build/ldOptions
	@echo ' [LD-so] so AudioStream.RawAudioBlocks'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/AudioStream.RawAudioBlocks.so -fPIC -shared $(SOAudioStream.RawAudioBlocks) -lstreams-stdtk-runtime $(SPL_LDFLAGS)

SOAudioStream.AudioStream = \
	    build/operator/AudioStream/AudioStream.o \
	    build/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.o \
	    build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o 

bin/AudioStream.AudioStream.so: \
	$(SOAudioStream.AudioStream) \
	build/dep/AudioStream.AudioStream.so \
	build/ldOptions
	@echo ' [LD-so] so AudioStream.AudioStream'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/AudioStream.AudioStream.so -fPIC -shared $(SOAudioStream.AudioStream) $(SPL_LDFLAGS)

standalone: bin/standalone.exe

build/standalone/standalone.o: \
	    src/standalone/standalone.h \
	    src/standalone/standalone.cpp  \
	    build/cppOptions \
	    | build/standalone
	@echo ' [CXX-standalone] standalone'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/standalone/standalone.cpp

bin/standalone.exe: build/standalone/standalone.o build/ldOptions
	@echo ' [LD-standalone] standalone'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/standalone.exe build/standalone/standalone.o $(SO_STAND_LIB)
	@echo ' [LN-standalone] standalone '
	@ln -sf standalone bin/TestFileAudioSource
build/standalone:
	-@mkdir -m 755 build/standalone

sablink:

	@ln -sf ../TestFileAudioSource.sab bin/sab

bundle: TestFileAudioSource.sab

-include build/dep/TestFileAudioSource.sab.d
TestFileAudioSource.sab: $(SPL_SO_BINS) $(SPL_ADL_FILE) build/dep/TestFileAudioSource.sab.d bin/standalone.exe
	@echo ' [Bundle] TestFileAudioSource.sab'
	@$(SPL_BUNDLE_BUILDER) $(SPL_OUTPUT_DIR_ROOT) $(SPL_OUTPUT_DIR) $(SPL_ADL_FILE) TestFileAudioSource.sab

