// GsyN39Ym_0oSOyJ33schDr2HlaSJjqNrao_1EJYjGM4ZNnWguh8BZTlGjLbrr6QS9mHHopYRpIwLNB9KcyzmGYBK


#include "./AudioStream.h"
using namespace SPL::_Operator;

#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>


#define MY_OPERATOR_SCOPE SPL::_Operator
#define MY_BASE_OPERATOR AudioStream_Base
#define MY_OPERATOR AudioStream$OP




   void MY_OPERATOR_SCOPE::MY_OPERATOR::process(uint32_t index)
   {
        {
    while ((state$count < lit$1))
        {
            SPL::blob id$audioBlob = ::SPL::Functions::String::convertToBlob((lit$2 + ::SPL::spl_cast<SPL::rstring, SPL::int32 >::cast(state$count)));
            do { SPL::BWS8pWC_1HYKrTViSTcG4T5GFz_0nGxqvmNlrajI9k_1E5zHNl9YbvsoTe9P3EfS_0f6OzRd8ZZzCJBaSXCG5A8XGA6 temp = SPL::BWS8pWC_1HYKrTViSTcG4T5GFz_0nGxqvmNlrajI9k_1E5zHNl9YbvsoTe9P3EfS_0f6OzRd8ZZzCJBaSXCG5A8XGA6(id$audioBlob, ::SPL::spl_cast<SPL::uint64, SPL::float64 >::cast((::SPL::Functions::Time::getTimestampInSecs() * lit$3))); submit (temp, lit$4); } while(0);
            state$count++;
            ::SPL::Functions::Utility::block(lit$5);
        }
}

   }

   void MY_OPERATOR_SCOPE::MY_OPERATOR::allPortsReady()
   {
        createThreads(1);
   }

static SPL::Operator * initer() { return new MY_OPERATOR_SCOPE::MY_OPERATOR(); }
bool MY_BASE_OPERATOR::globalInit_ = MY_BASE_OPERATOR::globalIniter();
bool MY_BASE_OPERATOR::globalIniter() {
    instantiators_.insert(std::make_pair("AudioStream",&initer));
    return true;
}

template<class T> static void initRTC (SPL::Operator& o, T& v, const char * n) {
    SPL::ValueHandle vh = v;
    o.getContext().getRuntimeConstantValue(vh, n);
}

MY_BASE_OPERATOR::MY_BASE_OPERATOR()
 : Operator()  {
    uint32_t index = getIndex();
    initRTC(*this, lit$0, "lit$0");
    initRTC(*this, lit$1, "lit$1");
    initRTC(*this, lit$2, "lit$2");
    initRTC(*this, lit$3, "lit$3");
    initRTC(*this, lit$4, "lit$4");
    initRTC(*this, lit$5, "lit$5");
    state$count = lit$0;
    SPLAPPTRC(L_DEBUG, "Variable: state$count Value: " << state$count,SPL_OPER_DBG);
    (void) getParameters(); // ensure thread safety by initializing here
    OperatorMetrics& om = getContext().getMetrics();
}
MY_BASE_OPERATOR::~MY_BASE_OPERATOR()
{
    for (ParameterMapType::const_iterator it = paramValues_.begin(); it != paramValues_.end(); it++) {
        const ParameterValueListType& pvl = it->second;
        for (ParameterValueListType::const_iterator it2 = pvl.begin(); it2 != pvl.end(); it2++) {
            delete *it2;
        }
    }
}



void MY_BASE_OPERATOR::checkpointStateVariables(NetworkByteBuffer & opstate) const {
    opstate << state$count;
}

void MY_BASE_OPERATOR::restoreStateVariables(NetworkByteBuffer & opstate) {
    opstate >> state$count;
}

void MY_BASE_OPERATOR::checkpointStateVariables(Checkpoint & ckpt) {
    ckpt << state$count;
}

void MY_BASE_OPERATOR::resetStateVariables(Checkpoint & ckpt) {
    ckpt >> state$count;
}

void MY_BASE_OPERATOR::resetStateVariablesToInitialState() {
    state$count = lit$0;
    SPLAPPTRC(L_DEBUG, "Variable: state$count Value: " << state$count,SPL_OPER_DBG);
}

bool MY_BASE_OPERATOR::hasStateVariables() const {
    return true;
}

void MY_BASE_OPERATOR::resetToInitialStateRaw() {
    AutoMutex $apm($svMutex);
    StateHandler *sh = getContext().getStateHandler();
    if (sh != NULL) {
        sh->resetToInitialState();
    }
    resetStateVariablesToInitialState();
}

void MY_BASE_OPERATOR::checkpointRaw(Checkpoint & ckpt) {
    AutoMutex $apm($svMutex);
    StateHandler *sh = getContext().getStateHandler();
    if (sh != NULL) {
        sh->checkpoint(ckpt);
    }
    checkpointStateVariables(ckpt);
}

void MY_BASE_OPERATOR::resetRaw(Checkpoint & ckpt) {
    AutoMutex $apm($svMutex);
    StateHandler *sh = getContext().getStateHandler();
    if (sh != NULL) {
        sh->reset(ckpt);
    }
    resetStateVariables(ckpt);
}



