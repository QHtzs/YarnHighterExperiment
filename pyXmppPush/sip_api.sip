%TypeHeaderCode
#include <sip_api.h>
%End


int  AllocNewClient();

void LastClientConnectToServer(int client_id, const char* jid, const char *pwd);

void LastClientAddCaCertificates(int client_id, const char* base64_ca_pub);

void LastClientSetMessage(int client_id, const char *bid, const char* msg);

void LastClientSendFile(int client_id, const char* jid, const char* filename);
