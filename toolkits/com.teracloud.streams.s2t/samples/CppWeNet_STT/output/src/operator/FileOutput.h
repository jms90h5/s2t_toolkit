// OG6RjIrzSH2t6I8HHfBCp0gf0EhszAF7XU92fZrHkJ7lDKEO0GH_1Fssx0OvL_0LBL3DffTiewI807n2DCX9t1BC





#ifndef SPL_OPER_INSTANCE_FILEOUTPUT_H_
#define SPL_OPER_INSTANCE_FILEOUTPUT_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <SPL/Runtime/Common/Metric.h>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/ParameterValue.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Operator/OperatorMetrics.h>
#include <SPL/Runtime/Operator/Port/AutoPortMutex.h>
#include <SPL/Runtime/Operator/State/StateHandler.h>
#include <SPL/Runtime/ProcessingElement/PE.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/Runtime/Utility/CV.h>
using namespace UTILS_NAMESPACE;

#include "../type/BEgoJ0po0GtM2yrcYCdUueq_0x37P_0TgGZrLtqFnIVksAYOU0HiLmCCg9YDl_1Ifrq1sXNnn0FPmSzdGzgMuSZwD_1.h"
#include "../type/BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg.h"
#include "../type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.h"


#define MY_OPERATOR FileOutput$OP
#define MY_BASE_OPERATOR FileOutput_Base
#define MY_OPERATOR_SCOPE SPL::_Operator

namespace SPL {
namespace _Operator {

class MY_BASE_OPERATOR : public Operator
{
public:
    
    typedef SPL::BTxPHLH1IescpAMA6xZAxJpwE2p0IwWuwyxdeIjIAwrgLhR3ZkaPkKt67FUGqkFYaAxM3oO1cdawwoOqmPtVDDg IPort0Type;
    
    MY_BASE_OPERATOR();
    
    ~MY_BASE_OPERATOR();
    
    inline void tupleLogic(Tuple const & tuple, uint32_t port);
    void processRaw(Tuple const & tuple, uint32_t port);
    
    inline void punctLogic(Punctuation const & punct, uint32_t port);
    void processRaw(Punctuation const & punct, uint32_t port);
    void punctPermitProcessRaw(Punctuation const & punct, uint32_t port);
    void punctNoPermitProcessRaw(Punctuation const & punct, uint32_t port);
    
    inline void submit(Tuple & tuple, uint32_t port)
    {
        Operator::submit(tuple, port);
    }
    inline void submit(Tuple const & tuple, uint32_t port)
    {
        Operator::submit(tuple, port);
    }
    inline void submit(Punctuation const & punct, uint32_t port)
    {
        Operator::submit(punct, port);
    }
    
    
    
    SPL::rstring lit$0;
    SPL::uint32 lit$1;
    
    
protected:
    Mutex $svMutex;
    SPL::rstring param$format$0;
    SPL::boolean param$writePunctuations$0;
    SPL::rstring param$closeMode$0;
    void checkpointStateVariables(NetworkByteBuffer & opstate) const;
    void restoreStateVariables(NetworkByteBuffer & opstate);
    void checkpointStateVariables(Checkpoint & ckpt);
    void resetStateVariables(Checkpoint & ckpt);
    void resetStateVariablesToInitialState();
    bool hasStateVariables() const;
    void resetToInitialStateRaw();
    void checkpointRaw(Checkpoint & ckpt);
    void resetRaw(Checkpoint & ckpt);

private:
    static bool globalInit_;
    static bool globalIniter();
    ParameterMapType paramValues_;
    Metric* metrics_[1];
    ParameterMapType& getParameters() { return paramValues_;}
    void addParameterValue(std::string const & param, ConstValueHandle const& value)
    {
        ParameterMapType::iterator it = paramValues_.find(param);
        if (it == paramValues_.end())
            it = paramValues_.insert (std::make_pair (param, ParameterValueListType())).first;
        it->second.push_back(&ParameterValue::create(value));
    }
    void addParameterValue(std::string const & param)
    {
        ParameterMapType::iterator it = paramValues_.find(param);
        if (it == paramValues_.end())
            it = paramValues_.insert (std::make_pair (param, ParameterValueListType())).first;
        it->second.push_back(&ParameterValue::create());
    }
};


class MY_OPERATOR : public MY_BASE_OPERATOR 
{
  public:
    MY_OPERATOR();

    virtual void prepareToShutdown();

    void process(Tuple const & tuple, uint32_t port);
    
    
        void process(Punctuation const & punct, uint32_t port);
    
    
  private:
    class Helper {
        public:
            Helper (const std::string& fName, bool restoring
                    
                    );
            
                std::ostream& fs() { return _fs; }
                std::ostream& writeTo() { return _fs; }
            
            void flush() { _fs.flush(); }
            int fd() { return _fd; }
            boost::iostreams::filtering_streambuf<boost::iostreams::output>& filt_str()
                { return _filt_str; }
        private:
            std::unique_ptr<boost::iostreams::file_descriptor_sink> _fd_sink;
            std::ostream _fs;
            boost::iostreams::filtering_streambuf<boost::iostreams::output> _filt_str;
            
            
            int _fd;
    };


    
    

    std::string genFilename();
    std::string makeAbsolute(const std::string & path);
    void openFile(bool restoring);
    void closeFile();
    Mutex _mutex;
    volatile bool _shutdown;
    std::string _pathName;
    std::string _pattern;
    Metric& _numFilesOpenedMetric;
    Metric& _numTupleWriteErrorsMetric;
    std::unique_ptr<Helper> _f;
    uint32_t _fileGeneration;
    
    
    uint64_t _tuplesUntilFlush, _flushCount;
    
    
    
    
    

    

    // Position of the file on a checkpoint/restore
    uint64_t _ckptOffset;
    bool _resetting;
}; 

} // namespace _Operator
} // namespace SPL

#undef MY_OPERATOR_SCOPE
#undef MY_BASE_OPERATOR
#undef MY_OPERATOR
#endif // SPL_OPER_INSTANCE_FILEOUTPUT_H_

