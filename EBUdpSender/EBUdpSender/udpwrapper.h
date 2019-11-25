#ifndef UDPWRAPPER_H
#define UDPWRAPPER_H

#ifndef _WIN32
#define S_IMPORT
#define S_EXPORT
#else

#ifdef ISLIB
#define S_IMPORT __declspec(dllimport)
#define S_EXPORT __declspec(dllexport)
#else
#define S_IMPORT
#define S_EXPORT
#endif

#endif

#if defined(UDPWRAPPER_LIBRARY)
#  define UDPWRAPPERSHARED_EXPORT S_EXPORT
#else
#  define UDPWRAPPERSHARED_EXPORT S_IMPORT
#endif

#include <stdio.h>
#include <atomic>
#include <memory>
#include <chrono>
#include <thread>
#include <string.h>
#include <malloc.h>
#include <mutex>
#include <stdlib.h>

#ifndef _WIN32
#include <stdint-gcc.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef  int EB_SOCKET;
typedef  ssize_t  m_ssize_t;

#else
#include <stdint.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

typedef SOCKET EB_SOCKET;
typedef int m_ssize_t;
#endif


template <class T>
class RWContainer{

public:
    RWContainer(){
        for(size_t i=0; i<65535; i++) _values[i] = nullptr;
    }

    RWContainer(const RWContainer&) =delete;

    RWContainer& operator=(const RWContainer&) = delete;

    ~RWContainer(){
        T* ptr;
        for(size_t i=0; i<65536; i++){
            ptr = _values[i];
            if(ptr) delete ptr;
        }
    }

    T* Replace(uint16_t index, T* value){
        _mut.lock();
        std::swap(value, _values[index]);
        _mut.unlock();
        return value;
    }

    bool PutIn(uint16_t index, T* value){
        bool ret = false;
        _mut.lock();
        if(_values[index]==nullptr){
            _values[index] = value;
            ret = true;
        }
        _mut.unlock();
        return ret;
    }

    T*& operator[](uint16_t t){
        return _values[t];
    }

private:
    T*  _values[65535];   //60*sizeof(ulong)<=240kb
    std::mutex _mut;
};


#ifdef __cplusplus
extern "C" {
#endif


//no left refer(&), using pointer instead

typedef  struct CircleUdpSockets{
    CircleUdpSockets* next;

    EB_SOCKET socket_fd;

    uint16_t group;
    uint16_t group_m;

    sockaddr_in target_host;  //support different platform

    bool is_head;

    CircleUdpSockets():is_head(false){}

    ~CircleUdpSockets(){
#ifdef _WIN32
        closesocket(socket_fd);
#else
        close(socket_fd);
#endif
        if(is_head){
            CircleUdpSockets* node = this->next;
           CircleUdpSockets* ptr;
            while(node != this){
                ptr = node;
                node = node->next;
                delete ptr;
            }
        }
    }
} UdpSocketList;


typedef struct UdpFrame {
    uint16_t head_flag;
    uint16_t group_flag;

    uint16_t req;
    uint16_t ack;

    uint16_t pic_id;
    uint16_t pic_index;

    uint16_t width;
    uint16_t height;

    uint16_t sample;
    uint16_t total_size;

    uint8_t* dlen512;

    UdpFrame(){
        head_flag = 65534;
        group_flag = 65534;
        req = 65534;
        ack = 65534;
        pic_id = 65534;
        pic_index = 65534;
        width = 1;
        height = 1;
        sample = 1;
        total_size = 0;
        dlen512 = nullptr;
    }

    constexpr static size_t size(){return sizeof(UdpFrame)+ 512 - sizeof(uint8_t*);}

    bool Series(uint8_t* dst, size_t dst_buf_size){
        if(dst_buf_size < this->size()) return false;
        memset(dst, 0, dst_buf_size);
        //bigendian
        dst[0] = (head_flag >> 8) & 0xff;
        dst[1] = head_flag & 0xff;

        dst[2] = (group_flag >> 8) & 0xff;
        dst[3] = group_flag & 0xff;

        dst[4] = (req >> 8) & 0xff;
        dst[5] = req & 0xff;

        dst[6] = (ack >> 8) & 0xff;
        dst[7] = ack & 0xff;

        dst[8] = (pic_id >> 8) & 0xff;
        dst[9] = pic_id & 0xff;

        dst[10] = (pic_index >> 8) & 0xff;
        dst[11] = pic_index & 0xff;

        dst[12] = (width >> 8) & 0xff;
        dst[13] = width & 0xff;

        dst[14] = (height >> 8) & 0xff;
        dst[15] = height & 0xff;

        dst[16] = (sample >> 8) & 0xff;
        dst[17] = sample & 0xff;

        dst[18] = (total_size >> 8) & 0xff;
        dst[19] = total_size & 0xff;

        memcpy(dst+20, dlen512, total_size);

        return true;
    }

    static bool is_bigendian(){
        uint16_t tmp = 1;
        return reinterpret_cast<uint8_t*>(&tmp)[1] == 1;
    }

    static UdpFrame Deseries(uint8_t* dst, size_t dst_buf_size){
        UdpFrame frame;
        if(is_bigendian()){
             frame = reinterpret_cast<UdpFrame&>(dst);
        }else{
            if(dst_buf_size < frame.size()) return frame;
            frame.head_flag =  (dst[0]<<8 & 0xff00) | (dst[1] & 0xff);
            frame.group_flag = (dst[2]<<8 & 0xff00) | (dst[3] & 0xff);
            frame.req =  (dst[4]<<8 & 0xff00) | (dst[5] & 0xff);
            frame.ack =  (dst[6]<<8 & 0xff00) | (dst[7] & 0xff);
            frame.pic_id =  (dst[8]<<8 & 0xff00) | (dst[9] & 0xff);
            frame.pic_index =  (dst[10]<<8 & 0xff00) | (dst[11] & 0xff);
            frame.width =  (dst[12]<<8 & 0xff00) | (dst[13] & 0xff);
            frame.height=  (dst[14]<<8 & 0xff00) | (dst[15] & 0xff);
            frame.sample =  (dst[16]<<8 & 0xff00) | (dst[17] & 0xff);
            frame.total_size =  (dst[18]<<8 & 0xff00) | (dst[19] & 0xff);
        }
        frame.dlen512 = dst+20;
        return frame;
    }

} Packet;


typedef struct Reducer{
    uint8_t* last;
    uint8_t* current;
    uint16_t last_pic_id;
    uint16_t cur_pic_id;
    size_t len;
    Reducer(){
        last=nullptr;
        current=nullptr;
        last_pic_id=100;
        cur_pic_id=101;
        len=0;
    }

    ~Reducer() {
        if(last) free(last);
        if(current) free(current);
    }

    void extend(size_t l){
        if(l <= len) return;
        else{
            if(last==nullptr){
                last = reinterpret_cast<uint8_t*>(malloc(l));
                current = reinterpret_cast<uint8_t*>(malloc(l));
                len = l;
            }else{
                last = reinterpret_cast<uint8_t*>(realloc(last, l));
                current = reinterpret_cast<uint8_t*>(realloc(current, l));
                len = l;
            }
        }
    }

} ReduceToPic;


typedef struct Image{
    uint16_t width;
    uint16_t height;

    uint16_t channel; //2
    uint16_t __t; //2

    size_t   msize;

    size_t   dsize;

    uint8_t* data;

    std::mutex _mut;  //92

    Image(){
        width=0;
        height=0;
        channel = 0;
        __t = 0;
        msize = 0;
        dsize = 0;
        data = nullptr;
    }

    Image(const Image &im) =delete;

    Image& operator=(const Image &im) =delete;

    ~Image(){
        if(data) free(data);
    }

    bool GetInfo(uint16_t* _width, uint16_t* _height, uint16_t* _channel, uint8_t* dst, size_t dst_buf_size){
        bool ret = true;
        _mut.lock();
        if(__t==0 || msize > dst_buf_size) ret=false;
        else{
            *_width = width;
            *_height = height;
            *_channel = channel;
            memcpy(dst, data, msize);
            _mut.unlock();
        }
        return ret;
    }

    void SetData(uint16_t _width, uint16_t _height, uint16_t _channel, uint8_t* forcopy){
        size_t sz = _width*_height*_channel;
        _mut.lock();
        if(sz > dsize){
            if(dsize == 0) data = reinterpret_cast<uint8_t*>(malloc(sz*sizeof(uint8_t)));
            else{
                uint8_t* ptr = reinterpret_cast<uint8_t*>(realloc(data, sz));
                if(ptr==nullptr){
                     _mut.unlock();
                    return;
                }
                else data = ptr;
            }
            dsize = sz;
        }
        width = _width;
        height = _height;
        channel = _channel;
        msize = sz;
        __t = 1;
        memcpy(data, forcopy, sz);
        _mut.unlock();
    }

} *ImagePtr;


typedef RWContainer<ReduceToPic> GReducer;

typedef RWContainer<UdpSocketList> SockRefer;

typedef RWContainer<Image> ImageMap;

typedef RWContainer<bool>  Hungup;

typedef void(*M_CALLBACK)(uint8_t*, size_t, size_t, size_t, size_t);

UDPWRAPPERSHARED_EXPORT int CreateUdpSocketList(uint16_t group, const char *host, uint16_t port, int socket_num);

UDPWRAPPERSHARED_EXPORT void UnRefUdpSocketList(int index);

UDPWRAPPERSHARED_EXPORT void SendDataOut(int sks_index, uint8_t* data, uint16_t pic_id, int width, int height, int sample, int u_sec);

UDPWRAPPERSHARED_EXPORT void StartListener(uint16_t port, M_CALLBACK callback);

UDPWRAPPERSHARED_EXPORT void StopListener(int port);

UDPWRAPPERSHARED_EXPORT bool GetCurrentPic(uint16_t group, uint16_t* _width, uint16_t* _height, uint16_t* _channel, uint8_t* dst, size_t dst_buf_size);

UDPWRAPPERSHARED_EXPORT uint16_t* GetCurrentAllPicGroup(int* size);


#ifdef __cplusplus
}
#endif


#endif // UDPWRAPPER_H
