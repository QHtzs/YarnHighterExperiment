#include "pushclient.h"
#include "apis.h"

#ifdef __cplusplus
extern "C"{
#endif

static PushClient *global_client = nullptr;

static void LazyInit(){
    global_client = new PushClient();
}

MEXPORT void AddCaCertificate(const char *pem){
    if(global_client == nullptr) LazyInit();
    global_client->add_ca_certificate(pem);
}

MEXPORT void SetIgnoreSslError(){
    if(global_client == nullptr) LazyInit();
    global_client->set_ignore_sslerror();
}

MEXPORT void ConnectToServer(const char* jid, const char* pw){
    if(global_client == nullptr) LazyInit();
    global_client->connect_to_host(jid, pw);
}

MEXPORT void SendMessage(const char* jidbare, const char* message){
    if(global_client == nullptr) LazyInit();
    global_client->send_message(jidbare, message);
}

MEXPORT void SendImage(const char* jidbare, const char* filename){
    if(global_client == nullptr) LazyInit();
    global_client->send_image_file(jidbare, filename);
}

#ifdef __cplusplus
}
#endif


