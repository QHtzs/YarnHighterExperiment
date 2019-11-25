#include "udpwrapper.h"
#include <thread>




static void callback(uint8_t* data, size_t g, size_t w, size_t h, size_t s){
    printf("%zu , %zu, %zu, %zu---sum--", g, w, h, s);
    size_t sum=0;
    size_t p = w*h*s;
    for(size_t i=0; i<p; i++) sum += data[i];
    printf("cur: %zu, next: %zu \r\n", sum, sum+p);
}


int main(int argc,char *args[]) {
    auto thd = std::thread([=](){
        uint16_t w, h, s;
        uint8_t* ch = new uint8_t[1080*1100*4]();
        for(int i=0; i<40; i++){
            sleep(10);
            bool bl = GetCurrentPic(10, &w, &h, &s, ch, 1080*1100*4);
            if(!bl) { printf("no pic\r\n"); continue;}
            printf("--%d , %d, %d, %d---sum--", 10, w, h, s);
            size_t p = w*h*s;
            size_t sum = 0;
            for(size_t i=0; i<p; i++) sum += ch[i];
            printf("--cur: %zu, next: %zu \r\n", sum, sum+p);
        }
        delete[] ch;
    });
    thd.detach();
 if(argc<2) StartListener(8089, &callback);
 else {
     StartListener((uint16_t)atoi(args[1]), &callback);
 }
 return 0;
}


