#ifndef IM_BUFF_H
#define IM_BUFF_H


#include <cstring>
#include <stdio.h>
#include <iostream>

class IM_BUFF {

public:
    IM_BUFF();

    ~IM_BUFF();

    void swap(IM_BUFF &im);

    void update(const unsigned char* const data0, int width, int height, int channel, int group);

    void msetchar(char* ptr, int to_uint16);

    size_t size;
    size_t cap;
    bool sent;
    char* data;


};

#endif // IM_BUFF_H
