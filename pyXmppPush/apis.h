#ifndef APIS_H
#define APIS_H

#include <vector>

#ifdef XmppLib
//
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


#ifdef __cplusplus
extern "C"{
#endif


MEXPORT void AddCaCertificate(const char *pem);

MEXPORT void SetIgnoreSslError();

MEXPORT void ConnectToServer(const char* jid, const char* pw);

MEXPORT void SendMessage(const char* jidbare, const char* message);

MEXPORT void SendImage(const char* jidbare, const char* filename);

#ifdef __cplusplus
}
#endif

#endif // APIS_H


