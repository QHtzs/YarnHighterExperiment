#include "pushclient.h"
#include "apis.h"

#ifdef __cplusplus
extern "C"{
#endif

static PushClient global_client;

MEXPORT void AddCaCertificate(const char *pem){
    global_client.add_ca_certificate(pem);
}

MEXPORT void SetIgnoreSslError(){
    global_client.set_ignore_sslerror();
}

MEXPORT void ConnectToServer(const char* jid, const char* pw){
    global_client.connect_to_host(jid, pw);
}

MEXPORT void SendMessage(const char* jidbare, const char* message){
    global_client.send_message(jidbare, message);
}

MEXPORT void SendImage(const char* jidbare, const char* filename){
    global_client.send_image_file(jidbare, filename);
}

#ifdef __cplusplus
}
#endif


