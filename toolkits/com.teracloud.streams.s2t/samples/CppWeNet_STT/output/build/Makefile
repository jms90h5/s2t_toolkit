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

SPL_TYPES = build/type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.o \
            build/type/BtGOTOmseHBDBZJkUONJc5FWBP94nOE83XLB_00bk_0KP375_01XppZSWMm1tUBFbC2tuDNWD2HS_1q5gZGoogBKyDl.o \
            build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o \
            build/type/BEgoJ0po0GtM2yrcYCdUueq_0x37P_0TgGZrLtqFnIVksAYOU0HiLmCCg9YDl_1Ifrq1sXNnn0FPmSzdGzgMuSZwD_1.o \
            build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o 
SPL_FUNCTIONS = 
SPL_OPERATORS = build/operator/TranscriptionMonitor.o \
                build/operator/TimestampedTranscription.o \
                build/operator/FileOutput.o \
                build/operator/AudioStream/AudioStream.o \
                build/operator/TranscriptionStream/TranscriptionStream.o \
                build/operator/TranscriptionSink/Sink.o 
SPL_SO_BINS = bin/TranscriptionMonitor.so \
              bin/TimestampedTranscription.so \
              bin/FileOutput.so \
              bin/AudioStream.AudioStream.so \
              bin/TranscriptionStream.TranscriptionStream.so \
              bin/TranscriptionSink.Sink.so 

SPL_ADL_FILE = com.teracloud.streams.s2t.sample.RealtimeTranscriber.adl

SPL_OUTPUT_DIR_ROOT = $(shell printf '%q' '/homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t/samples/CppWeNet_STT')
SPL_OUTPUT_DIR = $(shell printf '%q' 'output')

.PHONY: distclean clean all types functions operators sos bundle toolkits standalone sablink

all: bin types functions operators sos standalone bundle sablink

distclean clean:
	@rm -fr bin/* build/type build/function build/operator build/bundle build/standalone com.teracloud.streams.s2t.sample.RealtimeTranscriber.sab

bin:
	@mkdir -m 755 bin

types: $(SPL_TYPES)

build/type/BEgoJ0po0GtM2yrcYCdUueq_0x37P_0TgGZrLtqFnIVksAYOU0HiLmCCg9YDl_1Ifrq1sXNnn0FPmSzdGzgMuSZwD_1.o: \
	    src/type/BEgoJ0po0GtM2yrcYCdUueq_0x37P_0TgGZrLtqFnIVksAYOU0HiLmCCg9YDl_1Ifrq1sXNnn0FPmSzdGzgMuSZwD_1.h \
	    src/type/BEgoJ0po0GtM2yrcYCdUueq_0x37P_0TgGZrLtqFnIVksAYOU0HiLmCCg9YDl_1Ifrq1sXNnn0FPmSzdGzgMuSZwD_1.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] enum{punct,count,size,time,dynamic,never}'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BEgoJ0po0GtM2yrcYCdUueq_0x37P_0TgGZrLtqFnIVksAYOU0HiLmCCg9YDl_1Ifrq1sXNnn0FPmSzdGzgMuSZwD_1.cpp

build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o: \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.h \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] enum{csv,txt,bin,block,line}'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.cpp

build/type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.o: \
	    src/type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.h \
	    src/type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] tuple<rstring transcription,float64 confidence,uint64 audioTimestamp>'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.cpp

build/type/BtGOTOmseHBDBZJkUONJc5FWBP94nOE83XLB_00bk_0KP375_01XppZSWMm1tUBFbC2tuDNWD2HS_1q5gZGoogBKyDl.o: \
	    src/type/BtGOTOmseHBDBZJkUONJc5FWBP94nOE83XLB_00bk_0KP375_01XppZSWMm1tUBFbC2tuDNWD2HS_1q5gZGoogBKyDl.h \
	    src/type/BtGOTOmseHBDBZJkUONJc5FWBP94nOE83XLB_00bk_0KP375_01XppZSWMm1tUBFbC2tuDNWD2HS_1q5gZGoogBKyDl.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] tuple<rstring text,boolean isFinal,float64 confidence>'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BtGOTOmseHBDBZJkUONJc5FWBP94nOE83XLB_00bk_0KP375_01XppZSWMm1tUBFbC2tuDNWD2HS_1q5gZGoogBKyDl.cpp

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

-include build/operator/TranscriptionMonitor.d
build/operator/TranscriptionMonitor.o: \
	    src/operator/TranscriptionMonitor.h \
	    src/operator/TranscriptionMonitor.cpp\
	    build/dep/operator/TranscriptionMonitor.dep  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] TranscriptionMonitor'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"com.teracloud.streams.s2t.sample.RealtimeTranscriber.adl\" -c $(SPL_CXXFLAGS) src/operator/TranscriptionMonitor.cpp

-include build/operator/TimestampedTranscription.d
build/operator/TimestampedTranscription.o: \
	    src/operator/TimestampedTranscription.h \
	    src/operator/TimestampedTranscription.cpp\
	    build/dep/operator/TimestampedTranscription.dep \
	    src/type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.h  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] TimestampedTranscription'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"com.teracloud.streams.s2t.sample.RealtimeTranscriber.adl\" -c $(SPL_CXXFLAGS) src/operator/TimestampedTranscription.cpp

-include build/operator/FileOutput.d
build/operator/FileOutput.o: \
	    src/operator/FileOutput.h \
	    src/operator/FileOutput.cpp\
	    build/dep/operator/FileOutput.dep \
	    src/type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.h \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.h \
	    src/type/BEgoJ0po0GtM2yrcYCdUueq_0x37P_0TgGZrLtqFnIVksAYOU0HiLmCCg9YDl_1Ifrq1sXNnn0FPmSzdGzgMuSZwD_1.h  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] FileOutput'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"com.teracloud.streams.s2t.sample.RealtimeTranscriber.adl\" -c $(SPL_CXXFLAGS) src/operator/FileOutput.cpp

-include build/operator/AudioStream/AudioStream.d
build/operator/AudioStream/AudioStream.o: \
	    src/operator/AudioStream/AudioStream.h \
	    src/operator/AudioStream/AudioStream.cpp\
	    build/dep/operator/AudioStream/AudioStream.dep  \
	    build/cppOptions \
	    | build/operator/AudioStream
	@echo ' [CXX-operator] AudioStream.AudioStream'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"com.teracloud.streams.s2t.sample.RealtimeTranscriber.adl\" -c $(SPL_CXXFLAGS) src/operator/AudioStream/AudioStream.cpp

-include build/operator/TranscriptionStream/TranscriptionStream.d
build/operator/TranscriptionStream/TranscriptionStream.o: \
	    src/operator/TranscriptionStream/TranscriptionStream.h \
	    src/operator/TranscriptionStream/TranscriptionStream.cpp\
	    build/dep/operator/TranscriptionStream/TranscriptionStream.dep \
	    src/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.h  \
	    build/cppOptions \
	    | build/operator/TranscriptionStream
	@echo ' [CXX-operator] TranscriptionStream.TranscriptionStream'
	@$(CXX) -o $@ -MD -I ../../../../com.teracloud.streams.s2t/impl/nl/include -I /homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t/impl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"com.teracloud.streams.s2t.sample.RealtimeTranscriber.adl\" -c $(SPL_CXXFLAGS) src/operator/TranscriptionStream/TranscriptionStream.cpp

-include build/operator/TranscriptionSink/Sink.d
build/operator/TranscriptionSink/Sink.o: \
	    src/operator/TranscriptionSink/Sink.h \
	    src/operator/TranscriptionSink/Sink.cpp\
	    build/dep/operator/TranscriptionSink/Sink.dep \
	    src/type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.h \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.h  \
	    build/cppOptions \
	    | build/operator/TranscriptionSink
	@echo ' [CXX-operator] TranscriptionSink.Sink'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"com.teracloud.streams.s2t.sample.RealtimeTranscriber.adl\" -c $(SPL_CXXFLAGS) src/operator/TranscriptionSink/Sink.cpp

build/operator/TranscriptionSink:
	-@mkdir -m 755 -p build/operator/TranscriptionSink

build/operator/TranscriptionStream:
	-@mkdir -m 755 -p build/operator/TranscriptionStream

build/operator/AudioStream:
	-@mkdir -m 755 -p build/operator/AudioStream

build/operator:
	-@mkdir -m 755 -p build/operator

functions: $(SPL_FUNCTIONS)

sos: $(SPL_SO_BINS)

SOTranscriptionMonitor = \
	    build/operator/TranscriptionMonitor.o \
	    build/type/BtGOTOmseHBDBZJkUONJc5FWBP94nOE83XLB_00bk_0KP375_01XppZSWMm1tUBFbC2tuDNWD2HS_1q5gZGoogBKyDl.o 

bin/TranscriptionMonitor.so: \
	$(SOTranscriptionMonitor) \
	build/dep/TranscriptionMonitor.so \
	build/ldOptions
	@echo ' [LD-so] so TranscriptionMonitor'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/TranscriptionMonitor.so -fPIC -shared $(SOTranscriptionMonitor) $(SPL_LDFLAGS)

SOTimestampedTranscription = \
	    build/operator/TimestampedTranscription.o \
	    build/type/BtGOTOmseHBDBZJkUONJc5FWBP94nOE83XLB_00bk_0KP375_01XppZSWMm1tUBFbC2tuDNWD2HS_1q5gZGoogBKyDl.o \
	    build/type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.o 

bin/TimestampedTranscription.so: \
	$(SOTimestampedTranscription) \
	build/dep/TimestampedTranscription.so \
	build/ldOptions
	@echo ' [LD-so] so TimestampedTranscription'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/TimestampedTranscription.so -fPIC -shared $(SOTimestampedTranscription) $(SPL_LDFLAGS)

SOFileOutput = \
	    build/operator/FileOutput.o \
	    build/type/BEgoJ0po0GtM2yrcYCdUueq_0x37P_0TgGZrLtqFnIVksAYOU0HiLmCCg9YDl_1Ifrq1sXNnn0FPmSzdGzgMuSZwD_1.o \
	    build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o \
	    build/type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.o 

bin/FileOutput.so: \
	$(SOFileOutput) \
	build/dep/FileOutput.so \
	build/ldOptions
	@echo ' [LD-so] so FileOutput'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/FileOutput.so -fPIC -shared $(SOFileOutput) -lstreams-stdtk-runtime $(SPL_LDFLAGS)

SOAudioStream.AudioStream = \
	    build/operator/AudioStream/AudioStream.o \
	    build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o 

bin/AudioStream.AudioStream.so: \
	$(SOAudioStream.AudioStream) \
	build/dep/AudioStream.AudioStream.so \
	build/ldOptions
	@echo ' [LD-so] so AudioStream.AudioStream'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/AudioStream.AudioStream.so -fPIC -shared $(SOAudioStream.AudioStream) $(SPL_LDFLAGS)

SOTranscriptionStream.TranscriptionStream = \
	    build/operator/TranscriptionStream/TranscriptionStream.o \
	    build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o \
	    build/type/BtGOTOmseHBDBZJkUONJc5FWBP94nOE83XLB_00bk_0KP375_01XppZSWMm1tUBFbC2tuDNWD2HS_1q5gZGoogBKyDl.o 

bin/TranscriptionStream.TranscriptionStream.so: \
	$(SOTranscriptionStream.TranscriptionStream) \
	build/dep/TranscriptionStream.TranscriptionStream.so \
	build/ldOptions
	@echo ' [LD-so] so TranscriptionStream.TranscriptionStream'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/TranscriptionStream.TranscriptionStream.so -fPIC -shared $(SOTranscriptionStream.TranscriptionStream) -L /homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t/impl -Wl,-rpath -Wl,\$$ORIGIN/../toolkits/com.teracloud.streams.s2t/impl -ls2t_impl $(SPL_LDFLAGS)

SOTranscriptionSink.Sink = \
	    build/operator/TranscriptionSink/Sink.o \
	    build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o \
	    build/type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.o 

bin/TranscriptionSink.Sink.so: \
	$(SOTranscriptionSink.Sink) \
	build/dep/TranscriptionSink.Sink.so \
	build/ldOptions
	@echo ' [LD-so] so TranscriptionSink.Sink'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/TranscriptionSink.Sink.so -fPIC -shared $(SOTranscriptionSink.Sink) -lstreams-stdtk-runtime $(SPL_LDFLAGS)

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
	@ln -sf standalone bin/com.teracloud.streams.s2t.sample.RealtimeTranscriber
build/standalone:
	-@mkdir -m 755 build/standalone

sablink:

	@ln -sf ../com.teracloud.streams.s2t.sample.RealtimeTranscriber.sab bin/sab

bundle: com.teracloud.streams.s2t.sample.RealtimeTranscriber.sab

-include build/dep/com.teracloud.streams.s2t.sample.RealtimeTranscriber.sab.d
com.teracloud.streams.s2t.sample.RealtimeTranscriber.sab: $(SPL_SO_BINS) $(SPL_ADL_FILE) build/dep/com.teracloud.streams.s2t.sample.RealtimeTranscriber.sab.d bin/standalone.exe
	@echo ' [Bundle] com.teracloud.streams.s2t.sample.RealtimeTranscriber.sab'
	@$(SPL_BUNDLE_BUILDER) $(SPL_OUTPUT_DIR_ROOT) $(SPL_OUTPUT_DIR) $(SPL_ADL_FILE) com.teracloud.streams.s2t.sample.RealtimeTranscriber.sab

