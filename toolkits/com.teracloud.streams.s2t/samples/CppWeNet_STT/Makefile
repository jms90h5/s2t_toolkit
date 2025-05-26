# Makefile for WeNet Speech-to-Text sample application

.PHONY: all distributed clean 

# Application parameters
SPL_MAIN_COMPOSITE = com.teracloud.streams.s2t.sample::RealtimeTranscriber
SPLC = $(STREAMS_INSTALL)/bin/sc
SPLC_FLAGS = -a -t ../../../
OUTPUT_DIR = output

all: distributed

distributed:
	$(SPLC) $(SPLC_FLAGS) -M $(SPL_MAIN_COMPOSITE) --output-directory $(OUTPUT_DIR)

clean:
	$(SPLC) $(SPLC_FLAGS) -C -M $(SPL_MAIN_COMPOSITE)
	rm -rf $(OUTPUT_DIR)

# Submit to Streams instance
submit: distributed
	$(STREAMS_INSTALL)/bin/streamtool submitjob $(OUTPUT_DIR)/com.teracloud.streams.s2t.sample.RealtimeTranscriber.sab

# Cancel running jobs
cancel:
	$(STREAMS_INSTALL)/bin/streamtool canceljob --jobname RealtimeTranscriber