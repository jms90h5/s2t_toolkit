# Model Download Instructions

Due to GitHub file size limitations, large model files are not included in the repository. Download them separately:

## Required Models

### WeNet Model (Complete - INCLUDED)
- Location: `models/wenet_model/`
- Status: ✅ **INCLUDED** - Complete with real CMVN statistics
- Files: `global_cmvn`, `final.pt/`, `units.txt`, `words.txt`, `train.yaml`

### ONNX Models (Large files - DOWNLOAD REQUIRED)
- Location: `models/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/`
- Status: ⚠️ **DOWNLOAD REQUIRED** for some large files

#### Download Commands:
```bash
cd models/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/

# Download large encoder models (>100MB)
wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20.tar.bz2
tar -xf sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20.tar.bz2
mv sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/encoder-epoch-99-avg-1*.onnx .
```

#### Included Files (Small):
- ✅ `decoder-epoch-99-avg-1.onnx` (12.2MB)
- ✅ `joiner-epoch-99-avg-1.onnx` (12.2MB) 
- ✅ `test_wavs/` - Real audio test files
- ✅ `tokens.txt` - Vocabulary
- ✅ `bpe.model` and `bpe.vocab`

#### Download Required (Large):
- ⬇️ `encoder-epoch-99-avg-1.onnx` (314MB)
- ⬇️ `encoder-epoch-99-avg-1.int8.onnx` (173MB)

## Alternative: Use WeNet Model Only

The toolkit is fully functional with just the WeNet model (which is included):

```spl
// Use WenetSTT operator with included model
stream<rstring text, boolean isFinal, float64 confidence> Transcription = 
    WenetSTT(AudioStream) {
        param
            modelPath: "../../models/wenet_model";  // ✅ Included
            sampleRate: 16000;
    }
```

## Model Download Status

| Model Type | Size | Status | Notes |
|------------|------|--------|-------|
| WeNet Complete | ~230MB | ✅ Included | Real CMVN, ready to use |
| ONNX Small Files | <50MB | ✅ Included | Decoder, joiner, vocab |
| ONNX Large Files | >300MB | ⬇️ Download | Encoder models |
| Test Audio | 1.5MB | ✅ Included | Real WAV files |

**The toolkit works immediately with included WeNet model - ONNX download is optional for that implementation.**