#include "configloader.h"
#include <iostream>
#include <fstream>


ConfigLoader::ConfigLoader()
{

}

std::vector<std::pair<std::string, uint16_t>> ConfigLoader::ReadIpcConfigFile(std::string path){
        std::vector<std::pair<std::string, uint16_t>> ret;
        std::ifstream file;
        file.open(path.c_str(), std::ios::in);
        if(!file.is_open()){
            std::cerr << "failed to read config file";
            return ret;
        }
        std::string line;
        while(std::getline(file, line)){
            trim(line);
            if(line.empty()) continue;
            auto x = split(line);
            if(x.size() == 2) ret.push_back(std::make_pair(x[0], static_cast<uint16_t>(charToInt(x[1]))));
            std::cout<< "stream url & group:" << line << std::endl;
        }
        return ret;
}

std::vector<std::string> ConfigLoader::split(std::string s)
{
    std::vector<std::string> ret;
    trim(s);
    size_t index;
    while(!s.empty()){
        index = s.find_first_of(32, 0);
        if(index==std::string::npos){
            ret.push_back(s);
            s="";
        }else{
            ret.push_back(s.substr(0, index));
            s = s.substr(index);
        }
        trim(s);
    }
    return ret;
}

std::vector<EbOptionDict> ConfigLoader::LoadAvDict(std::string path){
    std::vector<EbOptionDict> ret;
    std::ifstream file;
    file.open(path.c_str(), std::ios::in);
    if(!file.is_open()) return ret;
    std::string line;
    while(std::getline(file, line)){
        trim(line);
        if(line.empty() || line.at(0)=='#') continue;
        auto tmp = split(line);
        if(tmp.size()==3){
            ret.push_back(EbOptionDict(tmp[0], tmp[1], charToInt(tmp[2])));
            std::cout << "avdict:" << tmp[0] << ";" << tmp[1] << ";" << tmp[2] << std::endl;
        }
    }
    return ret;
}

bool ConfigLoader::LoadHostAndPort(std::string path, std::string& host, uint16_t &port)
{
        std::ifstream file;
        file.open(path.c_str(), std::ios::in);
        if(!file.is_open()){
            std::cout <<"host port is null"<< std::endl;
            return false;
        }
        std::string line;
        while(std::getline(file, line)){
            trim(line);
            if(line.empty()) continue;
            else{
                //first no blank line
                size_t ind = 0;
                for(size_t i=0; i<line.length(); i++){
                    if(line[i]==':') ind = i;
                }
                if(ind < 6) return false;
                host = line.substr(0, ind);
                port = 0;
                ind++;
                while(ind < line.length()){
                    port = 10*port + static_cast<uint16_t>(line[ind] - '0');
                    ind++;
                }
                break;
            }
        }
        std::cout << host << ":" << port << std::endl;
        return true;
}

int ConfigLoader::charToInt(const std::string& s){
    int v=0;
    size_t ind = 0;
    while(ind < s.length()){
       v = 10*v + static_cast<int>(s[ind] - '0');
       ind++;
    }
    return v;
}

void ConfigLoader::trim(std::string &s){
    if(s.empty()) return;
        size_t i=s.length(), j=s.length();
        for(size_t p=0; p<s.length(); p++){
            if(s[p] !=32){
                i = p;
                break;
            }
        }

        for(size_t p=s.length()-1; p>0; p--){
            if(s[p] !=32){j = p; break;}
        }
        if(i==s.length() || j==s.length()) s="";
        else if(i < s.length() && j < s.length()) s = s.substr(i, j-i+1);
        else if(i<s.length()) s = s.substr(i);
        else if(j < s.length()) s = s.substr(0, j+1);
}
