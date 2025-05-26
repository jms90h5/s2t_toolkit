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

SPL_TYPES = build/type/BpSUw8mfEcWf02DHxFXcAEfeiSinvo2iyOrm4KEfBRvnHy4jMGnM6au_0nCr0kf7SJp_1QitTcsqIopJSCi7aPgAg.o \
            build/type/BWS8pWC_1HYKrTViSTcG4T5GFz_0nGxqvmNlrajI9k_1E5zHNl9YbvsoTe9P3EfS_0f6OzRd8ZZzCJBaSXCG5A8XGA6.o \
            build/type/Boa4dOIvUbIAmH24J4ofOOwbtyW_1L9R53Tr_1DArFVbZvrKaQaIPwvqDQjHc7MFE_0QstIWFaRo2B5PaWfPHLxQDb.o \
            build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o 
SPL_FUNCTIONS = 
SPL_OPERATORS = build/operator/AudioStream.o \
                build/operator/ConfigStream.o \
                build/operator/MockTranscription.o \
                build/operator/ResultDisplay.o \
                build/operator/FileWriter.o \
                build/operator/CompletionMonitor.o 
SPL_SO_BINS = bin/AudioStream.so \
              bin/ConfigStream.so \
              bin/MockTranscription.so \
              bin/ResultDisplay.so \
              bin/FileWriter.so \
              bin/CompletionMonitor.so 

SPL_ADL_FILE = BasicDemo.adl

SPL_OUTPUT_DIR_ROOT = $(shell printf '%q' '/homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t/samples/BasicWorkingExample')
SPL_OUTPUT_DIR = $(shell printf '%q' 'output')

.PHONY: distclean clean all types functions operators sos bundle toolkits standalone sablink

all: bin types functions operators sos standalone bundle sablink

distclean clean:
	@rm -fr bin/* build/type build/function build/operator build/bundle build/standalone BasicDemo.sab

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

build/type/BpSUw8mfEcWf02DHxFXcAEfeiSinvo2iyOrm4KEfBRvnHy4jMGnM6au_0nCr0kf7SJp_1QitTcsqIopJSCi7aPgAg.o: \
	    src/type/BpSUw8mfEcWf02DHxFXcAEfeiSinvo2iyOrm4KEfBRvnHy4jMGnM6au_0nCr0kf7SJp_1QitTcsqIopJSCi7aPgAg.h \
	    src/type/BpSUw8mfEcWf02DHxFXcAEfeiSinvo2iyOrm4KEfBRvnHy4jMGnM6au_0nCr0kf7SJp_1QitTcsqIopJSCi7aPgAg.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] tuple<rstring text,boolean isFinal,float64 confidence,uint64 audioTimestamp>'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BpSUw8mfEcWf02DHxFXcAEfeiSinvo2iyOrm4KEfBRvnHy4jMGnM6au_0nCr0kf7SJp_1QitTcsqIopJSCi7aPgAg.cpp

build/type/BWS8pWC_1HYKrTViSTcG4T5GFz_0nGxqvmNlrajI9k_1E5zHNl9YbvsoTe9P3EfS_0f6OzRd8ZZzCJBaSXCG5A8XGA6.o: \
	    src/type/BWS8pWC_1HYKrTViSTcG4T5GFz_0nGxqvmNlrajI9k_1E5zHNl9YbvsoTe9P3EfS_0f6OzRd8ZZzCJBaSXCG5A8XGA6.h \
	    src/type/BWS8pWC_1HYKrTViSTcG4T5GFz_0nGxqvmNlrajI9k_1E5zHNl9YbvsoTe9P3EfS_0f6OzRd8ZZzCJBaSXCG5A8XGA6.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] tuple<blob audio,uint64 audioTimestamp>'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BWS8pWC_1HYKrTViSTcG4T5GFz_0nGxqvmNlrajI9k_1E5zHNl9YbvsoTe9P3EfS_0f6OzRd8ZZzCJBaSXCG5A8XGA6.cpp

build/type/Boa4dOIvUbIAmH24J4ofOOwbtyW_1L9R53Tr_1DArFVbZvrKaQaIPwvqDQjHc7MFE_0QstIWFaRo2B5PaWfPHLxQDb.o: \
	    src/type/Boa4dOIvUbIAmH24J4ofOOwbtyW_1L9R53Tr_1DArFVbZvrKaQaIPwvqDQjHc7MFE_0QstIWFaRo2B5PaWfPHLxQDb.h \
	    src/type/Boa4dOIvUbIAmH24J4ofOOwbtyW_1L9R53Tr_1DArFVbZvrKaQaIPwvqDQjHc7MFE_0QstIWFaRo2B5PaWfPHLxQDb.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] tuple<rstring websocketUrl>'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/Boa4dOIvUbIAmH24J4ofOOwbtyW_1L9R53Tr_1DArFVbZvrKaQaIPwvqDQjHc7MFE_0QstIWFaRo2B5PaWfPHLxQDb.cpp

build/type:
	-@mkdir -m 755 build/type

operators: $(SPL_OPERATORS)

-include build/operator/AudioStream.d
build/operator/AudioStream.o: \
	    src/operator/AudioStream.h \
	    src/operator/AudioStream.cpp\
	    build/dep/operator/AudioStream.dep  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] AudioStream'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"BasicDemo.adl\" -c $(SPL_CXXFLAGS) src/operator/AudioStream.cpp

-include build/operator/ConfigStream.d
build/operator/ConfigStream.o: \
	    src/operator/ConfigStream.h \
	    src/operator/ConfigStream.cpp\
	    build/dep/operator/ConfigStream.dep  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] ConfigStream'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"BasicDemo.adl\" -c $(SPL_CXXFLAGS) src/operator/ConfigStream.cpp

-include build/operator/MockTranscription.d
build/operator/MockTranscription.o: \
	    src/operator/MockTranscription.h \
	    src/operator/MockTranscription.cpp\
	    build/dep/operator/MockTranscription.dep \
	    src/type/BpSUw8mfEcWf02DHxFXcAEfeiSinvo2iyOrm4KEfBRvnHy4jMGnM6au_0nCr0kf7SJp_1QitTcsqIopJSCi7aPgAg.h  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] MockTranscription'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"BasicDemo.adl\" -c $(SPL_CXXFLAGS) src/operator/MockTranscription.cpp

-include build/operator/ResultDisplay.d
build/operator/ResultDisplay.o: \
	    src/operator/ResultDisplay.h \
	    src/operator/ResultDisplay.cpp\
	    build/dep/operator/ResultDisplay.dep  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] ResultDisplay'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"BasicDemo.adl\" -c $(SPL_CXXFLAGS) src/operator/ResultDisplay.cpp

-include build/operator/FileWriter.d
build/operator/FileWriter.o: \
	    src/operator/FileWriter.h \
	    src/operator/FileWriter.cpp\
	    build/dep/operator/FileWriter.dep \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.h  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] FileWriter'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"BasicDemo.adl\" -c $(SPL_CXXFLAGS) src/operator/FileWriter.cpp

-include build/operator/CompletionMonitor.d
build/operator/CompletionMonitor.o: \
	    src/operator/CompletionMonitor.h \
	    src/operator/CompletionMonitor.cpp\
	    build/dep/operator/CompletionMonitor.dep  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] CompletionMonitor'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"BasicDemo.adl\" -c $(SPL_CXXFLAGS) src/operator/CompletionMonitor.cpp

build/operator:
	-@mkdir -m 755 -p build/operator

functions: $(SPL_FUNCTIONS)

sos: $(SPL_SO_BINS)

SOAudioStream = \
	    build/operator/AudioStream.o \
	    build/type/BWS8pWC_1HYKrTViSTcG4T5GFz_0nGxqvmNlrajI9k_1E5zHNl9YbvsoTe9P3EfS_0f6OzRd8ZZzCJBaSXCG5A8XGA6.o 

bin/AudioStream.so: \
	$(SOAudioStream) \
	build/dep/AudioStream.so \
	build/ldOptions
	@echo ' [LD-so] so AudioStream'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/AudioStream.so -fPIC -shared $(SOAudioStream) $(SPL_LDFLAGS)

SOConfigStream = \
	    build/operator/ConfigStream.o \
	    build/type/Boa4dOIvUbIAmH24J4ofOOwbtyW_1L9R53Tr_1DArFVbZvrKaQaIPwvqDQjHc7MFE_0QstIWFaRo2B5PaWfPHLxQDb.o 

bin/ConfigStream.so: \
	$(SOConfigStream) \
	build/dep/ConfigStream.so \
	build/ldOptions
	@echo ' [LD-so] so ConfigStream'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/ConfigStream.so -fPIC -shared $(SOConfigStream) $(SPL_LDFLAGS)

SOMockTranscription = \
	    build/operator/MockTranscription.o \
	    build/type/BWS8pWC_1HYKrTViSTcG4T5GFz_0nGxqvmNlrajI9k_1E5zHNl9YbvsoTe9P3EfS_0f6OzRd8ZZzCJBaSXCG5A8XGA6.o \
	    build/type/BpSUw8mfEcWf02DHxFXcAEfeiSinvo2iyOrm4KEfBRvnHy4jMGnM6au_0nCr0kf7SJp_1QitTcsqIopJSCi7aPgAg.o 

bin/MockTranscription.so: \
	$(SOMockTranscription) \
	build/dep/MockTranscription.so \
	build/ldOptions
	@echo ' [LD-so] so MockTranscription'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/MockTranscription.so -fPIC -shared $(SOMockTranscription) $(SPL_LDFLAGS)

SOResultDisplay = \
	    build/operator/ResultDisplay.o \
	    build/type/BpSUw8mfEcWf02DHxFXcAEfeiSinvo2iyOrm4KEfBRvnHy4jMGnM6au_0nCr0kf7SJp_1QitTcsqIopJSCi7aPgAg.o 

bin/ResultDisplay.so: \
	$(SOResultDisplay) \
	build/dep/ResultDisplay.so \
	build/ldOptions
	@echo ' [LD-so] so ResultDisplay'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/ResultDisplay.so -fPIC -shared $(SOResultDisplay) $(SPL_LDFLAGS)

SOFileWriter = \
	    build/operator/FileWriter.o \
	    build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o \
	    build/type/BpSUw8mfEcWf02DHxFXcAEfeiSinvo2iyOrm4KEfBRvnHy4jMGnM6au_0nCr0kf7SJp_1QitTcsqIopJSCi7aPgAg.o 

bin/FileWriter.so: \
	$(SOFileWriter) \
	build/dep/FileWriter.so \
	build/ldOptions
	@echo ' [LD-so] so FileWriter'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/FileWriter.so -fPIC -shared $(SOFileWriter) -lstreams-stdtk-runtime $(SPL_LDFLAGS)

SOCompletionMonitor = \
	    build/operator/CompletionMonitor.o \
	    build/type/BpSUw8mfEcWf02DHxFXcAEfeiSinvo2iyOrm4KEfBRvnHy4jMGnM6au_0nCr0kf7SJp_1QitTcsqIopJSCi7aPgAg.o 

bin/CompletionMonitor.so: \
	$(SOCompletionMonitor) \
	build/dep/CompletionMonitor.so \
	build/ldOptions
	@echo ' [LD-so] so CompletionMonitor'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/CompletionMonitor.so -fPIC -shared $(SOCompletionMonitor) $(SPL_LDFLAGS)

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
	@ln -sf standalone bin/BasicDemo
build/standalone:
	-@mkdir -m 755 build/standalone

sablink:

	@ln -sf ../BasicDemo.sab bin/sab

bundle: BasicDemo.sab

-include build/dep/BasicDemo.sab.d
BasicDemo.sab: $(SPL_SO_BINS) $(SPL_ADL_FILE) build/dep/BasicDemo.sab.d bin/standalone.exe
	@echo ' [Bundle] BasicDemo.sab'
	@$(SPL_BUNDLE_BUILDER) $(SPL_OUTPUT_DIR_ROOT) $(SPL_OUTPUT_DIR) $(SPL_ADL_FILE) BasicDemo.sab

