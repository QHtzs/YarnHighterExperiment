#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H


#include "im_buff.h"
#include <stdio.h>
#include <atomic>
#include <memory>
#include <chrono>
#include <thread>
#include <string>
#include <cstring>
#include <unordered_map>


#ifndef _WIN32
#include <stdint-gcc.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
typedef  int tcp_fd;


#else
#include <stdint.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
typedef SOCKET tcp_fd;

#endif



typedef IM_BUFF IM;

/// 简单的http请求端，不做过多封装
/// 不完全的拓展
/// 只支持短连接
/// 只支持post
/// 只支持content-length形式(预先告知长度,非chunk)发送数据
/// 仅仅依托于tcp(http1.1或更早版本)
/// 只支持部分posix系统，不支持windows
/// 不支持gzip压缩等
/// 非并发安全
/// 不支持url;path固定
class HttpRequest
{
public:
    HttpRequest();
    ~HttpRequest();
    void PostDataSync(const char* host, uint16_t port, const char* body, size_t bsize, std::unordered_map<std::string, std::string> other_headers=std::unordered_map<std::string, std::string>());

    void FeedNumber(int width, int height, int channel, int group, unsigned char* const data);

    void StartThread(const char *host, uint16_t port, std::unordered_map<std::string, std::string> other_headers=std::unordered_map<std::string, std::string>());

private:
    tcp_fd build_connect(const char* host, uint16_t port);
    void post_data(tcp_fd sock, const char* body, size_t bsize, std::unordered_map<std::string, std::string> other_headers);
    void close_socket(tcp_fd fd);
    bool is_free();
    void free_pem();

    std::atomic_bool pem;
    bool wanted;
    bool thread_started;

    char* response_bytes;

    IM rcu_buff[2];

    static const size_t buf_size = 512;



protected:
    static constexpr const char* content_length  = "Content-Length: " ;
    static constexpr const char* headers0 = "POST /PIC HTTP/1.1\r\n";
    static constexpr const char* headers1 ="User-Agent: EBE\r\nContent-Type: application/x-www-form-urlencodeed\r\n";
};

#endif // HTTPREQUEST_H
