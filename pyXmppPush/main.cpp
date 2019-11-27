#include "pushclient.h"
#include <vector>


static PushClient global_client;

LIB_EXPORT void SetCaCerts(std::vector<char*> pems){
    QByteArrayList alist;
    for(size_t i=0; i<pems.size(); i++) alist << pems[i];
    global_client.set_ca_certifys(alist);
}

LIB_EXPORT void SetIgnoretsl(){
    global_client.set_ignore_sslerror();
}

LIB_EXPORT void ConnectToServer(const char* jid, const char* pw){
    global_client.connect_to_host(jid, pw);
}

LIB_EXPORT void SendMessage(const char* jidbare, const char* message){
    global_client.send_message(jidbare, message);
}

LIB_EXPORT void SendImage(const char* jidbare, const char* filename){
    global_client.send_image_file(jidbare, filename);
}
