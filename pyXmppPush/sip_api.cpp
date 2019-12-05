#include "sip_api.h"
#include "pushclient.h"
#include <QList>

// for sip
static QList<PushClient* > p_clients;


MEXPORT int AllocNewClientNew(){
    PushClient* client = new PushClient(nullptr);
    p_clients.push_back(client);
    client->set_ignore_sslerror();
    return p_clients.size()-1;
}


MEXPORT void LastClientConnectToServer(int client_id, const char* jid, const char *pwd){
    p_clients[client_id]->connect_to_host(jid, pwd);
}


MEXPORT void LastClientAddCaCertificates(int client_id, const char* base64_ca_pub){
    p_clients[client_id]->add_ca_certificate(base64_ca_pub);
}

MEXPORT void LastClientSetMessage(int client_id, const char *bid, const char* msg){
    p_clients[client_id]->send_message(bid, msg);
}


MEXPORT void LastClientSendFile(int client_id, const char* jid, const char* filename){
    p_clients[client_id]->send_image_file(jid, filename);
}

