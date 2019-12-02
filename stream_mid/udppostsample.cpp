#include "udppostsample.h"


UdpPostSample::UdpPostSample()
{
    m_rcu_data[0] = new uint8_t[2080*1980*3]();
    m_rcu_data[1] = new uint8_t[2080*1980*3]();
    m_rcu_data[2] = new uint8_t[2080*1980*3]();
    m_width = 0;
    m_height = 0;
    m_sample = 0;
    escape = 1;
}

UdpPostSample::~UdpPostSample()
{
    delete [] m_rcu_data[0];
    delete [] m_rcu_data[1];
    delete [] m_rcu_data[2];
}


void UdpPostSample::StartReadStream(const std::string &url, const std::vector<std::string> &decoder_names_pri, const std::vector<EbOptionDict> &options, int rate, int av_task_theads)
{
    EbDecoder::DecodeVideo(url, decoder_names_pri, options,
                           [&](int width, int height, int s, unsigned char *const &data){
                                if(m_width==0){
                                    m_height = height;
                                    m_width = width;
                                    m_sample = s;
                                }
                                if(width==0) escape = 0;
                                size_t size = sizeof(uint8_t)*static_cast<size_t>(height*width*s);
                                memcpy(m_rcu_data[2], data, size);
                                std::swap(m_rcu_data[2], m_rcu_data[0]);

                           },
    rate, av_task_theads);
}

void UdpPostSample::StartSendOut(uint16_t group, const char *host, uint16_t port, int socket_num, int usec)
{
    int sks=CreateUdpSocketList(group, host, port, socket_num);
    while(sks==-1){
        std::cerr << "failed to gen sks, try again" << std::endl;
        sks=CreateUdpSocketList(group, host, port, socket_num);
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    std::cout << "current sks index: " << sks << std::endl;
    uint16_t pic_id = 0;
    while(escape){
        if(m_width > 0){
           std::swap(m_rcu_data[1], m_rcu_data[0]);
           SendDataOut(sks, m_rcu_data[1], pic_id, m_width, m_height, m_sample, usec);
           pic_id ++;
        }else{
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}
