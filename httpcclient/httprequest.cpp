#include "httprequest.h"


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
static const wsahold ws2_32_init_rii;
#endif


static std::string ItoA(size_t size){
    std::string tmp;
    while(size > 0){
        tmp += char(48 + size % 10);
        size /= 10;
    }
    return std::string(tmp.rbegin(), tmp.rend());
}


HttpRequest::HttpRequest()
{
  pem = true;
  wanted = true;
  thread_started = false;
  response_bytes = new char[buf_size]();
}

HttpRequest::~HttpRequest(){
    delete [] response_bytes;
}

void HttpRequest::PostDataSync(const char* host, uint16_t port, const char* body, size_t bsize, std::unordered_map<std::string, std::string> other_headers)
{
    tcp_fd fd = build_connect(host, port);
    if(fd != ~static_cast<tcp_fd>(0)){
        post_data(fd, body, bsize, other_headers);
        close_socket(fd);
    }else{
        printf("connect error\r\n");
    }
}


void HttpRequest::FeedNumber(int width, int height, int channel, int group, unsigned char* const data){
    if(is_free()){
        rcu_buff[1].update(data, width, height, channel, group);
        free_pem();
#ifdef m_debug
        printf("feed num  sent ok %d\r\n", rcu_buff[1].sent);
#endif

    }
}

void HttpRequest::StartThread(const char* host, uint16_t port, std::unordered_map<std::string, std::string> other_headers){
    if(!thread_started){
        thread_started = true;
        auto th = std::thread([&, port, host, other_headers](){
            while(1){
                //资源竞争损耗较大
                if(is_free()){

#ifdef m_debug
                    printf("http , b0: %d, b1:%d \r\n", rcu_buff[0].sent, rcu_buff[1].sent);
#endif
                    rcu_buff[0].swap(rcu_buff[1]);
                    if(!rcu_buff[0].sent){
#ifdef m_debug
                    printf("building connect, b0: %d, b1:%d, size: %lld \r\n", rcu_buff[0].sent, rcu_buff[1].sent,  rcu_buff[0].size);
#endif
                        tcp_fd fd = build_connect(host, port);
                        if(fd != ~static_cast<tcp_fd>(0)){
                            printf("connect ok %lld \r\n", fd);
                            post_data(fd, rcu_buff[0].data, rcu_buff[0].size, other_headers);
                            close_socket(fd);
                        }else{
                            printf("connect error %lld\r\n", fd);
                        }
                        rcu_buff[0].sent = true;
                    }

#ifdef m_debug
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
#endif
                    free_pem();
                }   
            }
        });
        th.detach();
    }
}

tcp_fd HttpRequest::build_connect(const char* host, uint16_t port)
{
    sockaddr_in addr_server;
    memset(&addr_server, 0, sizeof(sockaddr_in));
    addr_server.sin_family = AF_INET;
#ifndef _WIN32
    addr_server.sin_addr.s_addr = inet_addr(host);
#else
    inet_pton(AF_INET, host, &addr_server.sin_addr);
#endif
    addr_server.sin_port = htons(port);
    tcp_fd  sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == ~static_cast<tcp_fd>(0)) return sock;
    int status = connect(sock, reinterpret_cast<const sockaddr*>(&addr_server), sizeof(sockaddr_in));
    if(status < 0){
        return ~static_cast<tcp_fd>(0);
    }
    return sock;
}

void HttpRequest::post_data(tcp_fd sock, const char* body, size_t bsize, std::unordered_map<std::string, std::string> other_headers)
{

    std::string c_len = content_length;
    c_len += ItoA(bsize) + "\r\n";
    if(!other_headers.empty()){
        for(auto &pr: other_headers){
            c_len += pr.first + ": " + pr.second + "\r\n";
        }
    }
    c_len += "\r\n";

    //发送
    send(sock, headers0, static_cast<int>(strlen(headers0)), 0);
    send(sock, headers1, static_cast<int>(strlen(headers1)), 0);
    send(sock, c_len.c_str(), static_cast<int>(c_len.size()), 0);

    while(bsize>512){
        send(sock, body, 512, 0);
        body += 512;
        bsize -= 512;
    }
    if(bsize != 0) {
        send(sock, body, static_cast<int>(bsize), 0);
    }

    //30秒接收超时时间
    #ifndef _WIN32
            timeval tv_out;
            tv_out.tv_sec = 30;
            tv_out.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
    #else
            int tv_out=30000;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                       reinterpret_cast<const char*>(&tv_out), sizeof (tv_out));
    #endif
            memset(response_bytes, 0, buf_size);
            int len = recv(sock, response_bytes, buf_size, 0);
            //不能一次性读完，能读取部分即可
#ifdef m_debug
            if(len>0) printf("%s   \r\n", response_bytes);
#endif
}

void HttpRequest::close_socket(tcp_fd fd){
#ifdef _WIN32
    closesocket(fd);
#else
    close(fd);
#endif
}

bool HttpRequest::is_free()
{
  //强保证
  return pem.compare_exchange_strong(wanted, false);
}

void HttpRequest::free_pem(){
   pem.store(true, std::memory_order_relaxed);
}


