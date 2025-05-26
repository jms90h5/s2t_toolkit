// y6_5punct5count4size4time7dynamic5never


#ifndef BEGOJ0PO0GTM2YRCYCDUUEQ_0X37P_0TGGZRLTQFNIVKSAYOU0HILMCCG9YDL_1IFRQ1SXNNN0FPMSZDGZGMUSZWD_1_H_
#define BEGOJ0PO0GTM2YRCYCDUUEQ_0X37P_0TGGZRLTQFNIVKSAYOU0HILMCCG9YDL_1IFRQ1SXNNN0FPMSZDGZGMUSZWD_1_H_

#include <SPL/Runtime/Type/Enum.h>

#define SELF BEgoJ0po0GtM2yrcYCdUueq_0x37P_0TgGZrLtqFnIVksAYOU0HiLmCCg9YDl_1Ifrq1sXNnn0FPmSzdGzgMuSZwD_1

namespace SPL {

class SELF : public Enum
{
public:
   typedef SELF Self;

   static SELF punct;
   static SELF count;
   static SELF size;
   static SELF time;
   static SELF dynamic;
   static SELF never;
   

   SELF() : Enum(*mappings_) { }
   SELF(uint32_t v) : Enum(*mappings_, v) { }
   SELF(const Self & ot) : Enum(ot) { }
   SELF(const Enum& ot) : Enum(*mappings_) { assignFrom(ot); }
   SELF(const ConstValueHandle & ot) : Enum(ot) { }
   SELF(const std::string & v);
   SELF(const rstring & v);

   virtual Enum * clone() const { return new Self(*this); }

   Self & operator=(const Self & ot) { index_ = ot.index_; return *this; }

   bool operator==(const Self & ot) const { return index_ == ot.index_; }
   bool operator!=(const Self & ot) const { return index_ != ot.index_; }
   bool operator>(const Self & ot) const { return index_ > ot.index_; }
   bool operator>=(const Self & ot) const { return index_ >= ot.index_; }
   bool operator<(const Self & ot) const { return index_ < ot.index_; }
   bool operator<=(const Self & ot) const { return index_ <= ot.index_; }

   bool operator==(const Enum & ot) const { return index_ == ot.getIndex(); }
   bool operator!=(const Enum & ot) const { return index_ != ot.getIndex(); }
   bool operator>(const Enum & ot) const { return index_ > ot.getIndex(); }
   bool operator>=(const Enum & ot) const { return index_ >= ot.getIndex(); }
   bool operator<(const Enum & ot) const { return index_ < ot.getIndex(); }
   bool operator<=(const Enum & ot) const { return index_ <= ot.getIndex(); }

   SELF& operator= (uint32_t v) { index_ = v; return *this; }

private:
   static EnumMappings* mappings_;
   static EnumMappings* initMappings();
};

// Helper for parsing CSV.
template <class T, unsigned char separator>
class CSVExtractor;

template <unsigned char separator>
class CSVExtractor<SELF, separator>
{
public:
    // Extract one token of type T from stream fs.
    static inline void extract(std::istream & fs, SELF& field) {
        field.deserialize(fs, separator);
    }
};

}  /* namespace SPL */

namespace std { 
        template<>
        struct hash<SPL::SELF> {
            inline size_t operator()(const SPL::SELF & self) const { return self.hashCode(); }
        };
}

#undef SELF
#endif // BEGOJ0PO0GTM2YRCYCDUUEQ_0X37P_0TGGZRLTQFNIVKSAYOU0HILMCCG9YDL_1IFRQ1SXNNN0FPMSZDGZGMUSZWD_1_H_
