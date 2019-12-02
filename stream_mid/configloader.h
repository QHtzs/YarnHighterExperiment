#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include "eboptiondict.h"
#include <vector>
#include <string>


class ConfigLoader
{
public:
    ConfigLoader();

    static std::vector<std::pair<std::string, uint16_t>> ReadIpcConfigFile(std::string path);

    static bool LoadHostAndPort(std::string path, std::string& host, uint16_t &port);

    static std::vector<EbOptionDict> LoadAvDict(std::string path);

    static int charToInt(const std::string& s);

    static std::vector<std::string> split(std::string s);

private:
    static void trim(std::string &s);
};

#endif // CONFIGLOADER_H
