#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Winsock2.h"
#pragma comment(lib, "WS2_32.lib")
#include <iostream>
#include <string>
using namespace std;

string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);

bool GetRequestFromClient(char* name, short port, struct sockaddr* from, int* flen);
bool PutAnswerToClient(char* name, struct sockaddr* to, int* lto);
void GetServer(char* call, short port, struct sockaddr* from, int* flen);

SOCKET sS;

int main(int argc, char* argv[])
{
    char NAME[6] = "Hello";
    WSADATA wsaData;
    SOCKADDR_IN server;
    SOCKADDR_IN client;

    server.sin_family = AF_INET;
    server.sin_port = htons(2000);
    //server.sin_addr.s_addr = INADDR_ANY; 
    //server.sin_addr.s_addr = inet_addr("172.20.10.2");
    server.sin_addr.s_addr = inet_addr("192.168.43.160");



    memset(&client, 0, sizeof(client));
    int lclient = sizeof(client);
    int lserver = sizeof(server);

    int optval = 1;

    cout << "Server callsign: " << NAME << endl;
    try
    {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)                              throw SetErrorMsgText("Startup:", WSAGetLastError());
        GetServer(NAME, 2000, (sockaddr*)&client, &lclient);
        if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)             throw SetErrorMsgText("socket:", WSAGetLastError());
        if (bind(sS, (LPSOCKADDR)&server, sizeof(server)) == SOCKET_ERROR)          throw SetErrorMsgText("bind:", WSAGetLastError());

        char serverhostname[128] = "";
        if (gethostname(serverhostname, sizeof(serverhostname)) == SOCKET_ERROR)    throw SetErrorMsgText("gethostname:", WSAGetLastError());
        cout << "Server Hostname: " << serverhostname << endl;

        while (true)
        {
            if (GetRequestFromClient(NAME, htons(2000), (sockaddr*)&client, &lclient)) {
                PutAnswerToClient(NAME, (sockaddr*)&client, &lclient);
                hostent* chostname = gethostbyaddr((char*)&client.sin_addr, sizeof(client.sin_addr), AF_INET);
                cout << "Client Hostname: " << chostname->h_name << endl;
            }
        }

        if (closesocket(sS) == SOCKET_ERROR)             throw SetErrorMsgText("closesocket:", WSAGetLastError());
        if (WSACleanup() == SOCKET_ERROR)                throw SetErrorMsgText("Cleanup:", WSAGetLastError());
    }
    catch (string errorMsgText)
    {
        cout << endl << "WSAGetLastError: " << errorMsgText;
    }
    return 0;
}

bool GetRequestFromClient(char* name, short port, struct sockaddr* from, int* flen) {
    char bfrom[50];
    try {
        while (true) {
            if (recvfrom(sS, bfrom, sizeof(bfrom), NULL, from, flen) == SOCKET_ERROR) throw SetErrorMsgText("GetRequestFromClient recvfrom: ", WSAGetLastError()); 
            if (strcmp(name, bfrom) == 0) {
                cout << endl << "Client IP: " << inet_ntoa(((struct sockaddr_in*)from)->sin_addr) << endl;
                cout << "Client Port: " << ntohs(((struct sockaddr_in*)from)->sin_port) << endl;
                return true;
            }
        }
    }
    catch (string errorMsgText)
    {
        if (WSAGetLastError() == WSAETIMEDOUT) return false;
        throw SetErrorMsgText("GetRequestFromClient:", WSAGetLastError());
    }
}

bool PutAnswerToClient(char* name, struct sockaddr* to, int* lto) {
    try 
    {
        if ((sendto(sS, name, strlen(name) + 1, NULL, to, *lto)) == SOCKET_ERROR)   throw SetErrorMsgText("sendto:", WSAGetLastError());
        return true;
    }
    catch (string errorMsgText) 
    {
        throw SetErrorMsgText("PutAnswerToClient: ", WSAGetLastError());
    }
    return false;
}

void GetServer(char* call, short port, struct sockaddr* from, int* flen) {
    SOCKET cC;
    SOCKADDR_IN all;
    all.sin_family = AF_INET;
    all.sin_port = htons(port);
    //all.sin_addr.s_addr = INADDR_BROADCAST;
    //all.sin_addr.s_addr = inet_addr("172.20.10.15");
    all.sin_addr.s_addr = inet_addr("192.168.43.255");

    

    int timeout = 1000;

    int countServers = 1;
    int optval = 1;
    char buf[50];

    try {
        if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)                              throw  SetErrorMsgText("socket:", WSAGetLastError());
        if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR)   throw  SetErrorMsgText("setsocketopt:", WSAGetLastError());
        if (setsockopt(cC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int)) == SOCKET_ERROR)   throw  SetErrorMsgText("setsocketopt:", WSAGetLastError());

        if (sendto(cC, call, strlen(call) + 1, NULL, (sockaddr*)&all, sizeof(all)) == SOCKET_ERROR)  throw SetErrorMsgText("sendto:", WSAGetLastError());
        if (recvfrom(cC, buf, sizeof(buf), NULL, from, flen) == SOCKET_ERROR)                        throw  SetErrorMsgText("recvfrom:", WSAGetLastError());
        if (strcmp(call, buf) == 0) {
            countServers++;
            cout << "Server N" << countServers;
            hostent* shostname = gethostbyaddr((char*)&((SOCKADDR_IN*)from)->sin_addr, sizeof(SOCKADDR_IN), AF_INET);
            cout << "\tIP: " << inet_ntoa(((SOCKADDR_IN*)from)->sin_addr) << endl;
            cout << "\t\tPort: " << ntohs(((struct sockaddr_in*)from)->sin_port) << endl;
            cout << "\t\tHostname: " << shostname->h_name << endl;
        }
    }
    catch (string errorMsgText)
    {
        if (WSAGetLastError() == WSAETIMEDOUT) {
            cout << "Total number of servers with that callsign: " << countServers << endl;
            if (closesocket(cC) == SOCKET_ERROR) throw SetErrorMsgText("closesocket: ", WSAGetLastError());
        }
        else throw SetErrorMsgText("GetServer:", WSAGetLastError());
    }
}