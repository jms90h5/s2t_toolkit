// t5s4textb7isFinalF10confidenceU14audioTimestampU9latencyMs


#ifndef BFNW1L8VHTXQVFSKQGWLM0FQWKSYJGCR1GOWQXQI29YCLLU3JP63B_1NCBCBOZPDW0UPAMIFMQ2YZ9B9B8KQEGAM_H_
#define BFNW1L8VHTXQVFSKQGWLM0FQWKSYJGCR1GOWQXQI29YCLLU3JP63B_1NCBCBOZPDW0UPAMIFMQ2YZ9B9B8KQEGAM_H_

#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Type/PrimitiveType.h>
#include <SPL/Runtime/Type/CollectionType.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/VirtualByteBuffer.h>
#include <SPL/Runtime/Type/Optional.h>



#define SELF BFnw1L8VHTxQVFSKqgwlm0fQwKSYjgcr1gowQXqI29YclLU3jp63B_1ncbCBOZPDW0UpaMIFMq2yz9b9B8KqeGAm

namespace SPL {

class SELF : public Tuple
{
public:
    static const bool facade = false;

    typedef SELF Self;
    
    typedef SPL::rstring text_type;
    typedef SPL::boolean isFinal_type;
    typedef SPL::float64 confidence_type;
    typedef SPL::uint64 audioTimestamp_type;
    typedef SPL::uint64 latencyMs_type;

    enum { num_attributes = 5 } ;
    
    SELF() : Tuple(), text_(), isFinal_(), confidence_(), audioTimestamp_(), latencyMs_() {}
    SELF(const Self & ot) : Tuple(), text_(ot.text_), isFinal_(ot.isFinal_), confidence_(ot.confidence_), audioTimestamp_(ot.audioTimestamp_), latencyMs_(ot.latencyMs_) 
      { constructPayload(ot); }
    SELF(const text_type & _text, const isFinal_type & _isFinal, const confidence_type & _confidence, const audioTimestamp_type & _audioTimestamp, const latencyMs_type & _latencyMs) : Tuple(), text_(_text), isFinal_(_isFinal), confidence_(_confidence), audioTimestamp_(_audioTimestamp), latencyMs_(_latencyMs) { }
    
    SELF(const Tuple & ot, bool typesafe = true) : Tuple() { assignFrom(ot, typesafe); }
    SELF(const ConstValueHandle & ot) : Tuple() { const Tuple & o = ot; assignFrom(o); }

    virtual ~SELF() {}
    
    text_type & get_text() { return text_; }
    const text_type & get_text() const { return text_; }
    void set_text(const text_type & _text) { text_ = _text; }
    isFinal_type & get_isFinal() { return isFinal_; }
    const isFinal_type & get_isFinal() const { return isFinal_; }
    void set_isFinal(const isFinal_type & _isFinal) { isFinal_ = _isFinal; }
    confidence_type & get_confidence() { return confidence_; }
    const confidence_type & get_confidence() const { return confidence_; }
    void set_confidence(const confidence_type & _confidence) { confidence_ = _confidence; }
    audioTimestamp_type & get_audioTimestamp() { return audioTimestamp_; }
    const audioTimestamp_type & get_audioTimestamp() const { return audioTimestamp_; }
    void set_audioTimestamp(const audioTimestamp_type & _audioTimestamp) { audioTimestamp_ = _audioTimestamp; }
    latencyMs_type & get_latencyMs() { return latencyMs_; }
    const latencyMs_type & get_latencyMs() const { return latencyMs_; }
    void set_latencyMs(const latencyMs_type & _latencyMs) { latencyMs_ = _latencyMs; }
    virtual bool equals(const Tuple & ot) const
    {

        if (typeid(*this) != typeid(ot)) { return false; }
        const SELF & o = static_cast<const SELF &>(ot);
        return (*this == o);

    }

    virtual SELF& clear();

    Tuple* clone() const { return new Self(*this); }
    
    void serialize(VirtualByteBuffer & buf) const
    {
        buf << text_ << isFinal_ << confidence_ << audioTimestamp_ << latencyMs_;
    }

    template <class BufferType>
    void serialize(ByteBuffer<BufferType> & buf) const
    {        
        buf << text_ << isFinal_ << confidence_ << audioTimestamp_ << latencyMs_;
    } 
    
    void serialize(NativeByteBuffer & buf) const
    {
        this->serialize<NativeByteBuffer>(buf);
    }

    void serialize(NetworkByteBuffer & buf) const
    {
        this->serialize<NetworkByteBuffer>(buf);
    }
    
    void deserialize(VirtualByteBuffer & buf)
    {
        buf >> text_ >> isFinal_ >> confidence_ >> audioTimestamp_ >> latencyMs_;
    }

    template <class BufferType>
    void deserialize(ByteBuffer<BufferType> & buf)
    {        
        buf >> text_ >> isFinal_ >> confidence_ >> audioTimestamp_ >> latencyMs_;
    } 

    void deserialize(NativeByteBuffer & buf)
    {
        this->deserialize<NativeByteBuffer>(buf);
    }    

    void deserialize(NetworkByteBuffer & buf)
    {
        this->deserialize<NetworkByteBuffer>(buf);
    }    

    void serialize(std::ostream & ostr) const;

    void serializeWithPrecision(std::ostream & ostr) const;

    void deserialize(std::istream & istr, bool withSuffix = false);
    
    void deserializeWithNanAndInfs(std::istream & istr, bool withSuffix = false);
    
    size_t hashCode() const
    {
        size_t s = 17;
        s = 37 * s + std::hash<text_type >()(text_);
        s = 37 * s + std::hash<isFinal_type >()(isFinal_);
        s = 37 * s + std::hash<confidence_type >()(confidence_);
        s = 37 * s + std::hash<audioTimestamp_type >()(audioTimestamp_);
        s = 37 * s + std::hash<latencyMs_type >()(latencyMs_);
        return s;
    }
    
    size_t getSerializedSize() const
    {
        size_t size = sizeof(SPL::boolean)+sizeof(SPL::float64)+sizeof(SPL::uint64)+sizeof(SPL::uint64);
           size += text_.getSerializedSize();

        return size;

    }
    
    uint32_t getNumberOfAttributes() const 
        { return num_attributes; }

    TupleIterator getBeginIterator() 
        { return TupleIterator(*this, 0); }
    
    ConstTupleIterator getBeginIterator() const 
        { return ConstTupleIterator(*this, 0); }

    TupleIterator getEndIterator() 
        { return TupleIterator(*this, num_attributes); }

    ConstTupleIterator getEndIterator() const 
        { return ConstTupleIterator(*this, num_attributes); }
    
    TupleIterator findAttribute(const std::string & attrb)
    {
        std::unordered_map<std::string, uint32_t>::const_iterator it = mappings_->nameToIndex_.find(attrb);
        if ( it == mappings_->nameToIndex_.end() ) {
            return this->getEndIterator();
        }
        return TupleIterator(*this, it->second);
    }
    
    ConstTupleIterator findAttribute(const std::string & attrb) const
        { return const_cast<Self*>(this)->findAttribute(attrb); }
    
    TupleAttribute getAttribute(uint32_t index)
    {
        if (index >= num_attributes)
            invalidIndex (index, num_attributes);
        return TupleAttribute(mappings_->indexToName_[index], index, *this);
    }
    
    ConstTupleAttribute getAttribute(uint32_t index) const
        { return const_cast<Self*>(this)->getAttribute(index); }

    ValueHandle getAttributeValue(const std::string & attrb)
        { return getAttributeValueRaw(lookupAttributeName(*mappings_, attrb)->second); }


    ConstValueHandle getAttributeValue(const std::string & attrb) const
        { return const_cast<Self*>(this)->getAttributeValue(attrb); }

    ValueHandle getAttributeValue(uint32_t index) 
        { return getAttributeValueRaw(index); }

    ConstValueHandle getAttributeValue(uint32_t index) const
        { return const_cast<Self*>(this)->getAttributeValue(index); }

    Self & operator=(const Self & ot) 
    { 
        text_ = ot.text_;
        isFinal_ = ot.isFinal_;
        confidence_ = ot.confidence_;
        audioTimestamp_ = ot.audioTimestamp_;
        latencyMs_ = ot.latencyMs_; 
        assignPayload(ot);
        return *this; 
    }

    Self & operator=(const Tuple & ot) 
    { 
        assignFrom(ot); 
        return *this; 
    }

    void assign(Tuple const & tuple)
    {
        *this = static_cast<SELF const &>(tuple);
    }


    bool operator==(const Self & ot) const 
    {  
       return ( 
                text_ == ot.text_ && 
                isFinal_ == ot.isFinal_ && 
                confidence_ == ot.confidence_ && 
                audioTimestamp_ == ot.audioTimestamp_ && 
                latencyMs_ == ot.latencyMs_  
              ); 
    }
    bool operator==(const Tuple & ot) const { return equals(ot); }

    bool operator!=(const Self & ot) const { return !(*this == ot); }
    bool operator!=(const Tuple & ot) const { return !(*this == ot); }


    void swap(SELF & ot) 
    { 
        std::swap(text_, ot.text_);
        std::swap(isFinal_, ot.isFinal_);
        std::swap(confidence_, ot.confidence_);
        std::swap(audioTimestamp_, ot.audioTimestamp_);
        std::swap(latencyMs_, ot.latencyMs_);
      std::swap(payload_, ot.payload_);
    }

    void reset()
    { 
        *this = SELF(); 
    }

    void normalizeBoundedSetsAndMaps(); 

    const std::string & getAttributeName(uint32_t index) const
    {
        if (index >= num_attributes)
            invalidIndex (index, num_attributes);
        return mappings_->indexToName_[index];
    }

    const std::unordered_map<std::string, uint32_t> & getAttributeNames() const 
        { return mappings_->nameToIndex_; }


protected:

    ValueHandle getAttributeValueRaw(const uint32_t index)
    {
        if (index >= num_attributes)
            invalidIndex(index, num_attributes);
        const TypeOffset & t = mappings_->indexToTypeOffset_[index];
        return ValueHandle((char*)this + t.getOffset(), t.getMetaType(), &t.getTypeId());
    }

private:
    
    text_type text_;
    isFinal_type isFinal_;
    confidence_type confidence_;
    audioTimestamp_type audioTimestamp_;
    latencyMs_type latencyMs_;

    static TupleMappings* mappings_;
    static TupleMappings* initMappings();
};

inline VirtualByteBuffer & operator>>(VirtualByteBuffer & sbuf, SELF & tuple) 
    { tuple.deserialize(sbuf); return sbuf; }
inline VirtualByteBuffer & operator<<(VirtualByteBuffer & sbuf, SELF const & tuple) 
    { tuple.serialize(sbuf); return sbuf; }

template <class BufferType>
inline ByteBuffer<BufferType> & operator>>(ByteBuffer<BufferType> & sbuf, SELF & tuple) 
    { tuple.deserialize(sbuf); return sbuf; }
template <class BufferType>
inline ByteBuffer<BufferType> & operator<<(ByteBuffer<BufferType> & sbuf, SELF const & tuple) 
    { tuple.serialize(sbuf); return sbuf; }

inline NetworkByteBuffer & operator>>(NetworkByteBuffer & sbuf, SELF & tuple) 
    { tuple.deserialize(sbuf); return sbuf; }
inline NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, SELF const & tuple) 
    { tuple.serialize(sbuf); return sbuf; }

inline NativeByteBuffer & operator>>(NativeByteBuffer & sbuf, SELF & tuple) 
    { tuple.deserialize(sbuf); return sbuf; }
inline NativeByteBuffer & operator<<(NativeByteBuffer & sbuf, SELF const & tuple) 
    { tuple.serialize(sbuf); return sbuf; }

template<>
inline std::ostream & serializeWithPrecision(std::ostream & ostr, SELF const & tuple) 
    { tuple.serializeWithPrecision(ostr); return ostr; }
inline std::ostream & operator<<(std::ostream & ostr, SELF const & tuple) 
    { tuple.serialize(ostr); return ostr; }
inline std::istream & operator>>(std::istream & istr, SELF & tuple) 
    { tuple.deserialize(istr); return istr; }
template<>
inline void deserializeWithSuffix(std::istream & istr, SELF  & tuple) 
{ tuple.deserialize(istr,true);  }
inline void deserializeWithNanAndInfs(std::istream & istr, SELF  & tuple, bool withSuffix = false) 
{ tuple.deserializeWithNanAndInfs(istr,withSuffix);  }



}  // namespace SPL

namespace std
{
    inline void swap(SPL::SELF & a, SPL::SELF & b)
    {
        a.swap(b);
    }
}

namespace std { 
        template <>
        struct hash<SPL::SELF> {
            inline size_t operator()(const SPL::SELF & self) const 
                { return self.hashCode(); }
        };
}

#undef SELF
#endif // BFNW1L8VHTXQVFSKQGWLM0FQWKSYJGCR1GOWQXQI29YCLLU3JP63B_1NCBCBOZPDW0UPAMIFMQ2YZ9B9B8KQEGAM_H_ 


