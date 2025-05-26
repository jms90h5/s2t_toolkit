// TqYu_18l7xeo3HjnrhCtzKBNYDyKj8ThOf7nIjM5IlGr9GMJ7IGor2H3pyyG1Y46c_1HoLmKavdg_1ZP3DeVg0jBG


#include "./ResultDisplay.h"
using namespace SPL::_Operator;

#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>


#define MY_OPERATOR_SCOPE SPL::_Operator
#define MY_BASE_OPERATOR ResultDisplay_Base
#define MY_OPERATOR ResultDisplay$OP




static SPL::Operator * initer() { return new MY_OPERATOR_SCOPE::MY_OPERATOR(); }
bool MY_BASE_OPERATOR::globalInit_ = MY_BASE_OPERATOR::globalIniter();
bool MY_BASE_OPERATOR::globalIniter() {
    instantiators_.insert(std::make_pair("ResultDisplay",&initer));
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
    initRTC(*this, lit$6, "lit$6");
    initRTC(*this, lit$7, "lit$7");
    initRTC(*this, lit$8, "lit$8");
    initRTC(*this, lit$9, "lit$9");
    initRTC(*this, lit$10, "lit$10");
    initRTC(*this, lit$11, "lit$11");
    initRTC(*this, lit$12, "lit$12");
    initRTC(*this, lit$13, "lit$13");
    initRTC(*this, lit$14, "lit$14");
    initRTC(*this, lit$15, "lit$15");
    initRTC(*this, lit$16, "lit$16");
    initRTC(*this, lit$17, "lit$17");
    initRTC(*this, lit$18, "lit$18");
    initRTC(*this, lit$19, "lit$19");
    initRTC(*this, lit$20, "lit$20");
    initRTC(*this, lit$21, "lit$21");
    state$resultCount = lit$18;
    SPLAPPTRC(L_DEBUG, "Variable: state$resultCount Value: " << state$resultCount,SPL_OPER_DBG);
    state$totalLatency = lit$19;
    SPLAPPTRC(L_DEBUG, "Variable: state$totalLatency Value: " << state$totalLatency,SPL_OPER_DBG);
    state$minLatency = lit$20;
    SPLAPPTRC(L_DEBUG, "Variable: state$minLatency Value: " << state$minLatency,SPL_OPER_DBG);
    state$maxLatency = lit$21;
    SPLAPPTRC(L_DEBUG, "Variable: state$maxLatency Value: " << state$maxLatency,SPL_OPER_DBG);
    (void) getParameters(); // ensure thread safety by initializing here
    OperatorMetrics& om = getContext().getMetrics();
    metrics_[0] = &(om.createCustomMetric("nExceptionsCaughtPort0", "Number of exceptions caught on port 0", Metric::Counter));
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

void MY_BASE_OPERATOR::tupleLogic(Tuple const & tuple, uint32_t port) {
    IPort0Type const & iport$0 = static_cast<IPort0Type const  &>(tuple);
    AutoPortMutex $apm($svMutex, *this);
    
{
    ::SPL::Functions::Utility::printStringLn((((((((lit$6 + ::SPL::spl_cast<SPL::rstring, SPL::uint64 >::cast(iport$0.get_latencyMs())) + lit$5) + iport$0.get_text()) + (iport$0.get_isFinal() ? lit$4 : lit$3)) + lit$2) + ::SPL::spl_cast<SPL::rstring, SPL::float64 >::cast((iport$0.get_confidence() * lit$1))) + lit$0));
    state$resultCount++;
    state$totalLatency += iport$0.get_latencyMs();
    if ((iport$0.get_latencyMs() < state$minLatency)) 
        state$minLatency = iport$0.get_latencyMs();
    if ((iport$0.get_latencyMs() > state$maxLatency)) 
        state$maxLatency = iport$0.get_latencyMs();
    if ((SPL::uint64(state$resultCount % lit$8) == lit$7)) 
        {
            ::SPL::Functions::Utility::printStringLn(lit$9);
            ::SPL::Functions::Utility::printStringLn((lit$10 + ::SPL::spl_cast<SPL::rstring, SPL::uint64 >::cast(state$resultCount)));
            ::SPL::Functions::Utility::printStringLn(((lit$12 + ::SPL::spl_cast<SPL::rstring, SPL::uint64 >::cast(SPL::uint64(state$totalLatency / state$resultCount))) + lit$11));
            ::SPL::Functions::Utility::printStringLn(((lit$14 + ::SPL::spl_cast<SPL::rstring, SPL::uint64 >::cast(state$minLatency)) + lit$13));
            ::SPL::Functions::Utility::printStringLn(((lit$16 + ::SPL::spl_cast<SPL::rstring, SPL::uint64 >::cast(state$maxLatency)) + lit$15));
            ::SPL::Functions::Utility::printStringLn(lit$17);
        }
}

}


void MY_BASE_OPERATOR::processRaw(Tuple const & tuple, uint32_t port) {
    try {
            tupleLogic (tuple, port);
            static_cast<MY_OPERATOR_SCOPE::MY_OPERATOR*>(this)->MY_OPERATOR::process(tuple, port);
    } catch (const SPL::SPLRuntimeException& e) {
        metrics_[port]->incrementValue();
        unrecoverableExceptionShutdown(e.getExplanation());
    } catch (const std::exception& e) {
        metrics_[port]->incrementValue();
        unrecoverableExceptionShutdown(e.what());
    } catch (...) {
        metrics_[port]->incrementValue();
        unrecoverableExceptionShutdown("Unknown exception");
    }
}


void MY_BASE_OPERATOR::punctLogic(Punctuation const & punct, uint32_t port) {
}

void MY_BASE_OPERATOR::punctPermitProcessRaw(Punctuation const & punct, uint32_t port) {
    {
        punctNoPermitProcessRaw(punct, port);
    }
}

void MY_BASE_OPERATOR::punctNoPermitProcessRaw(Punctuation const & punct, uint32_t port) {
    switch(punct) {
    case Punctuation::WindowMarker:
        try {
            punctLogic(punct, port);
            process(punct, port);
        } catch (const SPL::SPLRuntimeException& e) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown(e.getExplanation());
        } catch (const std::exception& e) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown(e.what());
        } catch (...) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown("Unknown exception");
        }
        break;
    case Punctuation::FinalMarker:
        try {
            punctLogic(punct, port);
            process(punct, port);
        } catch (const SPL::SPLRuntimeException& e) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown(e.getExplanation());
        } catch (const std::exception& e) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown(e.what());
        } catch (...) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown("Unknown exception");
        }
        break;
    case Punctuation::DrainMarker:
    case Punctuation::ResetMarker:
    case Punctuation::ResumeMarker:
        break;
    case Punctuation::SwitchMarker:
        break;
    case Punctuation::WatermarkMarker:
        break;
    default:
        break;
    }
}

void MY_BASE_OPERATOR::processRaw(Punctuation const & punct, uint32_t port) {
    switch(port) {
    case 0:
        punctNoPermitProcessRaw(punct, port);
        break;
    }
}



void MY_BASE_OPERATOR::checkpointStateVariables(NetworkByteBuffer & opstate) const {
    opstate << state$resultCount;
    opstate << state$totalLatency;
    opstate << state$minLatency;
    opstate << state$maxLatency;
}

void MY_BASE_OPERATOR::restoreStateVariables(NetworkByteBuffer & opstate) {
    opstate >> state$resultCount;
    opstate >> state$totalLatency;
    opstate >> state$minLatency;
    opstate >> state$maxLatency;
}

void MY_BASE_OPERATOR::checkpointStateVariables(Checkpoint & ckpt) {
    ckpt << state$resultCount;
    ckpt << state$totalLatency;
    ckpt << state$minLatency;
    ckpt << state$maxLatency;
}

void MY_BASE_OPERATOR::resetStateVariables(Checkpoint & ckpt) {
    ckpt >> state$resultCount;
    ckpt >> state$totalLatency;
    ckpt >> state$minLatency;
    ckpt >> state$maxLatency;
}

void MY_BASE_OPERATOR::resetStateVariablesToInitialState() {
    state$resultCount = lit$18;
    SPLAPPTRC(L_DEBUG, "Variable: state$resultCount Value: " << state$resultCount,SPL_OPER_DBG);
    state$totalLatency = lit$19;
    SPLAPPTRC(L_DEBUG, "Variable: state$totalLatency Value: " << state$totalLatency,SPL_OPER_DBG);
    state$minLatency = lit$20;
    SPLAPPTRC(L_DEBUG, "Variable: state$minLatency Value: " << state$minLatency,SPL_OPER_DBG);
    state$maxLatency = lit$21;
    SPLAPPTRC(L_DEBUG, "Variable: state$maxLatency Value: " << state$maxLatency,SPL_OPER_DBG);
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



