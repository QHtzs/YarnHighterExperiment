#include "eboptiondict.h"

EbOptionDict::EbOptionDict(const std::string &k, const std::string &v, int flag)
{
    key = k;
    value = v;
    this->flag = flag;
}
