#ifndef UDPPOSTSAMPLE_H
#define UDPPOSTSAMPLE_H



#include "ebdecoder.h"
#include "udpwrapper.h"


#ifndef _WIN32
typedef  socklen_t m_socklen_t;
#else
typedef  int m_socklen_t;
#endif

class UdpPostSample
{
public:
    UdpPostSample();
    ~UdpPostSample();
    void StartSendOut(uint16_t group, const char *host, uint16_t port, int socket_num, int usec);
    void StartReadStream(const std::string &url, const std::vector<std::string> &decoder_names_pri, const std::vector<EbOptionDict> &options, int rate = 2, int av_task_theads=4);

private:
    uint8_t* m_rcu_data[3];
    int m_width;
    int m_height;
    int m_sample;
    int escape;
};

#endif // UDPPOSTSAMPLE_H
