// y6_5punct5count4size4time7dynamic5never


#include "BEgoJ0po0GtM2yrcYCdUueq_0x37P_0TgGZrLtqFnIVksAYOU0HiLmCCg9YDl_1Ifrq1sXNnn0FPmSzdGzgMuSZwD_1.h"

#define SELF BEgoJ0po0GtM2yrcYCdUueq_0x37P_0TgGZrLtqFnIVksAYOU0HiLmCCg9YDl_1Ifrq1sXNnn0FPmSzdGzgMuSZwD_1

using namespace SPL;

EnumMappings* SELF::mappings_ = SELF::initMappings();


SELF SELF::punct(0);
SELF SELF::count(1);
SELF SELF::size(2);
SELF SELF::time(3);
SELF SELF::dynamic(4);
SELF SELF::never(5);

SELF::SELF(const std::string & v)
: Enum(*mappings_)
{
    // initialize from a string value
    this->Enum::operator=(v);
}

SELF::SELF(const rstring & v)
: Enum(*mappings_)
{
    // initialize from a string value
    this->Enum::operator=(v);
}


EnumMappings* SELF::initMappings()
{
   EnumMappings* em = new EnumMappings();
   // initialize the mappings 
   {
      std::string s("punct");
      em->nameToIndex_.insert(std::make_pair(s, 0));
      em->indexToName_.push_back(s);
   }
   
   {
      std::string s("count");
      em->nameToIndex_.insert(std::make_pair(s, 1));
      em->indexToName_.push_back(s);
   }
   
   {
      std::string s("size");
      em->nameToIndex_.insert(std::make_pair(s, 2));
      em->indexToName_.push_back(s);
   }
   
   {
      std::string s("time");
      em->nameToIndex_.insert(std::make_pair(s, 3));
      em->indexToName_.push_back(s);
   }
   
   {
      std::string s("dynamic");
      em->nameToIndex_.insert(std::make_pair(s, 4));
      em->indexToName_.push_back(s);
   }
   
   {
      std::string s("never");
      em->nameToIndex_.insert(std::make_pair(s, 5));
      em->indexToName_.push_back(s);
   }
   
   return em;
}
