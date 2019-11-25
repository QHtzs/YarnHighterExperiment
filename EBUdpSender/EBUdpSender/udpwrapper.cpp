#include "udpwrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

static SockRefer global_socket_map{};

static ImageMap  global_image_map{};

static Hungup listener_rid{};

#ifdef _WIN32

struct wsahold{
    wsahold(){
        WORD sockVersion = MAKEWORD(2, 2);
        WSADATA wsaData;
        int ws = WSAStartup(sockVersion, &wsaData);
        printf("wsa started status: %d\r\n", ws);
    }

    ~wsahold(){
       WSACleanup();
       printf(" WSA Cleaned up\r\n");
    }
};

static std::shared_ptr<wsahold> instance = std::shared_ptr<wsahold>(new wsahold());

#endif

UDPWRAPPERSHARED_EXPORT int CreateUdpSocketList(uint16_t group, const char *host, uint16_t port, int socket_num)
{
    UdpSocketList* head = new UdpSocketList;
    UdpSocketList* tail = nullptr;
    uint8_t tmp[512] = {0};
    Packet pack;
    pack.head_flag = 123;
    pack.req = 1;
    pack.ack = 0;
    pack.group_flag = group;
    pack.dlen512 = tmp;
    size_t size = Packet::size();
    uint8_t* dst= new uint8_t[size]();
    sockaddr_in addr_server;
    memset(&addr_server, 0, sizeof(sockaddr_in));
    addr_server.sin_family = AF_INET;
#ifndef _WIN32
    addr_server.sin_addr.s_addr = inet_addr(host);
#else
    inet_pton(AF_INET, host, &addr_server.sin_addr);
#endif
    addr_server.sin_port = htons(port);
    int Len = sizeof(sockaddr_in);
    auto lambda = [](EB_SOCKET fd){
#ifndef _WIN32
        close(fd);
#else
        closesocket(fd);
#endif
    };
    for(int i=0; i<socket_num; i++){
        EB_SOCKET fd = socket(AF_INET, SOCK_DGRAM, 0);
        if(static_cast<int>(fd) < 0 ) continue;

#ifndef _WIN32
        int status = connect(fd, reinterpret_cast<const sockaddr*>(&addr_server), Len);
         //not real connect, just bind server ip:port, and then read write
        //printf("status %d\r\n", status);
        if(status<0) {lambda(fd); continue;}
#endif

        pack.Series(dst, size);
#ifndef _WIN32
        m_ssize_t l = write(fd, dst, size);
#else
        //m_ssize_t l = _write(fd, dst, static_cast<unsigned int>(size)); windows, socket fd is not the same as file fd, is different with posix system
        //m_ssize_t l = send(static_cast<EB_SOCKET>(fd), reinterpret_cast<const char*>(dst), static_cast<int>(size), 0); send failed
        m_ssize_t l = sendto(fd, reinterpret_cast<const char*>(dst), static_cast<int>(size), 0,
                             reinterpret_cast<const sockaddr*>(&addr_server), Len);
#endif
        if(l<0) {lambda(fd); continue;}

#ifndef _WIN32
        timeval tv_out;
        tv_out.tv_sec = 2;
        tv_out.tv_usec = 0;
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
#else
        int tv_out=2000;
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,
                   reinterpret_cast<const char*>(&tv_out), sizeof (tv_out));
#endif

#ifndef _WIN32
        m_ssize_t rl =read(fd, dst, size);
#else
        //printf("reading...\r\n");
        //m_ssize_t rl = _read(fd, dst, static_cast<unsigned int>(size)); windows, socket fd is not the same as file fd, is different with posix system
         m_ssize_t rl = recv(fd, reinterpret_cast<char*>(dst), static_cast<int>(size), 0);
       // printf("read...%d\r\n",rl);
#endif
        if(rl < 0){lambda(fd); continue;}
        Packet t_pk = Packet::Deseries(dst, size);
        if(t_pk.ack==1 && t_pk.req == 0){
            if(tail==nullptr){
                tail = head;
                head->next = nullptr;
                head->socket_fd = fd;
                head->group = group;
                head->target_host = addr_server;
            }else{
                UdpSocketList* node = new UdpSocketList;
                node->next = nullptr;
                node->socket_fd = fd;
                node->group = group;
                node->target_host = addr_server;
                tail->next = node;
                tail = node;
            }
         }

    }
    delete []dst;
    if(tail==nullptr){
      delete head;
      return -1;
    }
    head->is_head = true;
    tail->next = head; //circle
    int gen_index = -1;
    for(uint16_t ui=0; ui<65535; ui++){
        if(global_socket_map.PutIn(ui, head)){
            gen_index = static_cast<int>(ui);
            break;
        }
    }
    return gen_index;
}


UDPWRAPPERSHARED_EXPORT void UnRefUdpSocketList(int index){
    if(index < 0 || index > 65535) return;
    UdpSocketList* sks = nullptr;
    global_socket_map.Replace(static_cast<uint16_t>(index), sks);
    if(sks==nullptr) return;
    UdpSocketList* node = sks->next;
    UdpSocketList* ptr;
    while(node != sks){
        ptr = node;
        node = node->next;
        delete ptr;
    }
    delete sks;
}


UDPWRAPPERSHARED_EXPORT void SendDataOut(int sks_index, uint8_t* data, uint16_t pic_id, int width, int height, int sample, int u_sec)
{
        if(sks_index < 0 || sks_index >65535) return;
        UdpSocketList* sks = global_socket_map[static_cast<uint16_t>(sks_index)];
        if(sks == nullptr){
            printf("invalid sks_index");
            return;
        }
        UdpSocketList* head = sks;
        int size = sample*width*height;
        int index = 0;
        uint16_t pic_index = 0;
        size_t p_size = Packet::size();
        uint8_t* dst= new uint8_t[p_size]();
        m_ssize_t w_status = -1;


#ifdef _WIN32
    int toLen = static_cast<int>(sizeof(sockaddr_in));
    timeval u_time_out;
    fd_set fdsets;
    FD_ZERO(&fdsets);
    FD_SET(static_cast<EB_SOCKET>(head->next->socket_fd), &fdsets);
    u_time_out.tv_sec = u_sec/1000000;
    u_time_out.tv_usec = u_sec % 1000000;
#endif

        while (size - index > 512) {
            Packet pk;
            pk.head_flag = 123;
            pk.group_flag = sks->group;
            pk.req = 0;
            pk.ack = 0;
            pk.pic_id = pic_id;
            pk.pic_index = pic_index++;
            pk.width = static_cast<uint16_t>(width);
            pk.height = static_cast<uint16_t>(height);
            pk.sample = static_cast<uint16_t>(sample);
            pk.total_size = 512;
            pk.dlen512 = data + index;
            index += 512;

            pk.Series(dst, p_size);
#ifndef _WIN32
            w_status = write(sks->socket_fd, dst, p_size);
#else
           // w_status = send(sks->socket_fd, reinterpret_cast<const char*>(dst), static_cast<int>(p_size), 0);
            w_status = sendto(sks->socket_fd, reinterpret_cast<const char*>(dst),
                              static_cast<int>(p_size), 0, reinterpret_cast<sockaddr*>(&sks->target_host), toLen);
#endif
            if(w_status < 0) printf("send failed");
            sks = sks->next;
#ifndef _WIN32
            if(sks==head && u_sec > 0) usleep(u_sec);
#else
            if(sks==head && u_sec > 0) select(0, nullptr, nullptr, &fdsets, &u_time_out);
#endif

        }

        if(size - index>0){
            Packet pk;
            pk.head_flag = 123;
            pk.group_flag = sks->group;
            pk.req = 0;
            pk.ack = 0;
            pk.pic_id = pic_id;
            pk.pic_index = pic_index;
            pk.width = static_cast<uint16_t>(width);
            pk.height = static_cast<uint16_t>(height);
            pk.sample = static_cast<uint16_t>(sample);
            pk.total_size = static_cast<uint16_t>(size-index);
            pk.dlen512 = data + index;
            pk.Series(dst, p_size);
#ifndef _WIN32
            w_status = write(sks->socket_fd, dst, p_size);
#else
            //w_status = send(static_cast<EB_SOCKET>(sks->socket_fd), reinterpret_cast<const char*>(dst), static_cast<int>(p_size), 0);
            w_status = sendto(sks->socket_fd, reinterpret_cast<const char*>(dst),
                              static_cast<int>(p_size), 0, reinterpret_cast<sockaddr*>(&sks->target_host), toLen);
#endif
            if(w_status < 0) printf("send failed");
            sks = sks->next;
        }

        delete [] dst;
}


UDPWRAPPERSHARED_EXPORT void StartListener(uint16_t port, M_CALLBACK callback){
    if(listener_rid[port] && *listener_rid[port]){
        printf("failed to listen, port acquired\r\n");
        return;
    }
    EB_SOCKET udp = socket(AF_INET, SOCK_DGRAM, 0);
    if(static_cast<int>(udp) < 0){
        printf("failed to create socket");
        return;
    }

    sockaddr_in addr_server;
    memset(&addr_server, 0, sizeof(sockaddr_in));
    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.s_addr = INADDR_ANY;
    addr_server.sin_port = htons(port);

    if(bind(udp, reinterpret_cast<struct sockaddr *>(&addr_server), sizeof(sockaddr)) < 0){
        printf("failed to bind socket: error:%d\r\n", errno);
        return;
    }

    GReducer reduction;
    size_t p_size = Packet::size();
    size_t tcv_size = 200*p_size;
    uint8_t* dst= new uint8_t[p_size]();
    sockaddr client_addr;
    memset(&client_addr, 0, sizeof(sockaddr_in));
    socklen_t sk_len = static_cast<socklen_t>(sizeof(sockaddr));

    setsockopt(udp, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&tcv_size),
               sizeof(size_t));

    //std::atomic_bool* CTN = new std::atomic_bool(true);

    bool *CTN = new bool(true);
    if(!listener_rid.PutIn(port, CTN)){
#ifndef _WIN32
        close(udp);
#else
        closesocket(udp);
#endif
        return;
    }
    listener_rid[port] = CTN;


    while (*CTN){
   #ifndef _WIN32
          m_ssize_t rl = recvfrom(udp, dst, p_size, 0, &client_addr, &sk_len);
   #else
          m_ssize_t rl = recvfrom(static_cast<SOCKET>(udp), reinterpret_cast<char*>(dst),
                                 static_cast<int>(p_size), 0, &client_addr, &sk_len);
   #endif
           if(rl<0) {
               printf("errno %d \r\n", errno);
               continue;
           }
           Packet pk = Packet::Deseries(dst, p_size);
           //char tmp_str[16];
           //inet_ntop(AF_INET, &reinterpret_cast<sockaddr_in*>(&client_addr)->sin_addr, (PSTR)tmp_str, 16);
           // printf("%s %d\r\n", tmp_str, reinterpret_cast<sockaddr_in*>(&client_addr)->sin_port);
           if(pk.head_flag == 123){
               if(pk.ack==0 && pk.req == 1){
                   pk.req = 0;
                   pk.ack = 1;
                   pk.Series(dst, p_size);
#ifndef _WIN32
                   sendto(udp, dst, p_size, 0, &client_addr, sk_len);
#else
                   sendto(static_cast<SOCKET>(udp), reinterpret_cast<const char*>(dst),
                          static_cast<int>(p_size), 0, &client_addr, sk_len);

#endif
               }else{
                   uint16_t group = pk.group_flag;
                   size_t len = (pk.width*pk.height*pk.sample+512) &(~511); //align
                   Reducer *rducer;
                   if(reduction[group]==nullptr){
                        rducer = new Reducer();
                        if(!reduction.PutIn(group, rducer))delete rducer;
                   }
                   rducer = reduction[group];
                   rducer->extend(len);



                   if(pk.pic_id==rducer->last_pic_id) memcpy(rducer->last+512*pk.pic_index, pk.dlen512, 512);
                   else if(pk.pic_id==rducer->cur_pic_id) memcpy(rducer->current+512*pk.pic_index, pk.dlen512, 512);
                   else{

                       if(global_image_map[group] == nullptr){
                           ImagePtr im_ptr = new Image();
                           if(!global_image_map.PutIn(group, im_ptr)) delete im_ptr;
                       }

                       global_image_map[group]->SetData(pk.width, pk.height, pk.sample, rducer->last); //error

                       if(callback){ callback(rducer->last, group, pk.width, pk.height, pk.sample);}

                       uint8_t* swp = rducer->last;
                       rducer->last = rducer->current;
                       rducer->current = swp;
                       rducer->last_pic_id = rducer->cur_pic_id;
                       rducer->cur_pic_id = pk.pic_id;

                       memcpy((rducer->current)+512*pk.pic_index, pk.dlen512, 512);
                   }
               }
           }
       }
      *CTN = false;
      delete[] dst;
      printf("server stop\r\n");

}


UDPWRAPPERSHARED_EXPORT void StopListener(int port){
    if(port < 0 || port >65535) return ;
    uint16_t p = static_cast<uint16_t>(port);
    if(listener_rid[p]){
        *listener_rid[p] = false;
    }
}


UDPWRAPPERSHARED_EXPORT bool GetCurrentPic(uint16_t group, uint16_t* _width, uint16_t* _height, uint16_t* _channel, uint8_t* dst, size_t dst_buf_size){
    if(global_image_map[group]==nullptr) return false;
    return global_image_map[group]->GetInfo(_width,  _height, _channel,  dst,  dst_buf_size);
}


UDPWRAPPERSHARED_EXPORT uint16_t* GetCurrentAllPicGroup(int* size){
    size_t num = 0;
    for(uint16_t i=0; i<65535; i++)if(global_image_map[i]) num++;
    uint16_t* ret = new uint16_t[num]();
    *size = static_cast<int>(num);
    num = 0;
    for(uint16_t i=0; i<65535; i++) if(global_image_map[i]) ret[num++]=i;
    return ret;
}

#ifdef __cplusplus
}
#endif
