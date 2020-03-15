
#include "httprequest.h"
#include "ffmpegavdecoder.h"


int main(int argc, char* argv[])
{

#ifdef m_debug
    const char *url = "rtsp://admin:HikYSHWQR@10.0.0.202/Streaming/Channels/1";
    const char *decoder_name = "h264";
    int group = 10;
    const char* host = "127.0.0.1";
    std::cout << url << std::endl;
    std::cout << decoder_name << std::endl;
    std::cout << group << std::endl;
    std::cout << host << std::endl;

#else
    if(argc < 5) return -1;
    const char *url = argv[1];
    const char *decoder_name = argv[2];
    auto tmp = std::string(argv[3]);
    int group = 0;
    for(auto &ch: tmp){
        group = 10*group + ch - 48;
    }

    const char* host = argv[4];
    std::cout << url << std::endl;
    std::cout << decoder_name << std::endl;
    std::cout << group << std::endl;
    std::cout << host << std::endl;
#endif
    HttpRequest req;
    req.StartThread(host, 18888);
    printf("thread started");
    FfmpegAvDecoder::DecoderToRgb(url,
                                  group,
                                  decoder_name,
                                  [&req](int width, int height, int channel, int group, unsigned char* const data){
                                        req.FeedNumber(width, height, channel, group, data);
                                   }
    );
    printf("decoder exit");
    return 0;
}
