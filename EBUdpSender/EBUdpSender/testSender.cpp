
#include "udpwrapper.h"

int main(int argc, char *argv[]){
    int index = -1;
    if(argc < 2) index = CreateUdpSocketList(10, "127.0.0.1", 8089, 6);
    else{
        uint16_t port = (uint16_t)atoi(argv[1]);
        index = CreateUdpSocketList(10, "127.0.0.1", port, 6);
    }
    int width=1080, height=1080, sample=3;
    uint8_t* data = new uint8_t[width*height*sample]();
    for(uint8_t i=0; i<255; i++){
        memset(data, i, width*height*sample*sizeof(uint8_t));
        SendDataOut(index, data, i, width, height, sample,100);
        sleep(5);
    }
    return 0;
}
