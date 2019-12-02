#include "udppostsample.h"
#include "configloader.h"
#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{

    std::string ipc_cfg;
    std::string host_cfg;
    std::string avdict_cfg;

    std::vector<std::string> decode_names;
    if(argc >= 4){
        ipc_cfg = argv[1];
        host_cfg = argv[2];
        avdict_cfg = argv[3];
        for(int i=4; i<argc; i++) decode_names.push_back(argv[i]);
    }else{
//        ipc_cfg = "ipc_ips.cfg";
//        host_cfg = "server_domain.cfg";
//        avdict_cfg = "avdict.cfg";
        std::cerr << "option_args number < 4" << std::endl;
        return -1;
    }
    std::cout << ipc_cfg << "|" << host_cfg << "|" << avdict_cfg;

    uint16_t port;
    std::string host;
    if(!ConfigLoader::LoadHostAndPort(host_cfg, host, port)){
        std::cerr << "no host config file" << std::endl;
        return -1;
    }

    auto ipcs =  ConfigLoader::ReadIpcConfigFile(ipc_cfg);
    if(ipcs.empty()){
        std::cerr << "no ipcs config file" << std::endl;
        return -1;
    }

    auto avdict = ConfigLoader::LoadAvDict(avdict_cfg);

    std::vector<std::thread*> thread_ptrs;
    std::vector<UdpPostSample*> sps;
    for(auto ipc_pr : ipcs){
        UdpPostSample *sp = new UdpPostSample();
        sps.push_back(sp);
        std::string ipc = ipc_pr.first;
        uint16_t group = ipc_pr.second;
        auto th = new std::thread([&, ipc](){
            while(1){
                sp->StartReadStream(ipc, decode_names, avdict, 1, 1);
                std::this_thread::sleep_for(std::chrono::seconds(30));
            }
        });
        auto th2 = new std::thread([&, host](){
            sp->StartSendOut(group, host.c_str(), port, 10, 50);
        });
        thread_ptrs.push_back(th);
        thread_ptrs.push_back(th2);
    }
    for(size_t i=0; i<thread_ptrs.size(); i++) {
        if(thread_ptrs[i]->joinable())thread_ptrs[i]->join();
    }
    for(size_t i=0; i<thread_ptrs.size(); i++) delete thread_ptrs[i];
    for(auto sp: sps) delete sp;
    return 0;
}
