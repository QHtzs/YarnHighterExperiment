#ifndef SIP_API_H
#define SIP_API_H

#ifndef MEXPORT

#ifdef XmppLib
#ifdef _WIN32
#define MEXPORT __declspec(dllexport)
#else
#define MEXPORT
#endif
//
#else
//
#ifdef _WIN32
#define MEXPORT __declspec(dllimport)
#else
#define MEXPORT
#endif
//
#endif

#endif

#ifdef __cplusplus
extern "C"{
#endif

MEXPORT int  AllocNewClient();

MEXPORT void LastClientConnectToServer(int client_id, const char* jid, const char *pwd);

MEXPORT void LastClientAddCaCertificates(int client_id, const char* base64_ca_pub);

MEXPORT void LastClientSetMessage(int client_id, const char *bid, const char* msg);

MEXPORT void LastClientSendFile(int client_id, const char* jid, const char* filename);

#ifdef __cplusplus
}
#endif

#endif // SIP_API_H
