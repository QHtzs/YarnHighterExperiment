#include "im_buff.h"



IM_BUFF::IM_BUFF(){
    size = 0;
    cap = 1024*1024*3;
    data = new char[cap]();
    sent = true;
}

IM_BUFF::~IM_BUFF(){
    delete [] data;
}


void IM_BUFF::swap(IM_BUFF &im){
    std::swap(size, im.size);
    std::swap(data, im.data);
    std::swap(sent, im.sent);
}


void IM_BUFF::update(const unsigned char* const data0, int width, int height, int channel, int group){
    size_t bsize = static_cast<size_t>(8 + width * height * channel);
    if(bsize > cap){
        printf("buff cap is too small\r\n");
    }else{
        size = bsize;
        msetchar(data, width);
        msetchar(data+2, height);
        msetchar(data+4, channel);
        msetchar(data+6, group);
        memcpy(data+8, data0, bsize-8);
        sent = false;
    }
}

void IM_BUFF::msetchar(char* ptr, int to_uint16){
     uint16_t tmp = static_cast<uint16_t>(to_uint16);
     // bigendian
     ptr[1] = static_cast<char>( tmp & 0xff );
     ptr[0] = static_cast<char>((tmp >> 8) & 0xff);
}
