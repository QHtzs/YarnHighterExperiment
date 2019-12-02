#ifndef EBOPTIONDICT_H
#define EBOPTIONDICT_H

#include <string>

class EbOptionDict
{
public:
    explicit EbOptionDict(const std::string &k, const std::string &v, int flag);
    std::string key;
    std::string value;
    int flag;
};

#endif // EBOPTIONDICT_H
