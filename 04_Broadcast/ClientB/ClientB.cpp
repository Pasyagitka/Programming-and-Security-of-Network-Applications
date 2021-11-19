#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:4996) 
#include "Winsock2.h" 
#pragma comment(lib, "WS2_32.lib")
#include <iostream>
#include <string>
using namespace std;

string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);
bool GetServer(char* call, short port, struct sockaddr* from, int* flen);


int main(int argc, char* argv[])
{
    char CALL[6] = "Hello";
    WSADATA wsaData;
    SOCKET cS;
    SOCKADDR_IN server;
    int lserver = sizeof(server);

    cout << "Client: " << CALL << endl;
    try
    {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)	            throw SetErrorMsgText("WSAStartup:", WSAGetLastError());
        if (!GetServer(CALL, 2000, (sockaddr*)&server, &lserver))   cout << "GetServer returned false" << endl;
        if (WSACleanup() == SOCKET_ERROR)				            throw SetErrorMsgText("WSACleanup:", WSAGetLastError());
    }
    catch (string errorMsgText) 
    {
        cout << endl << "WSAGetLastError: " << errorMsgText;
    }
    return 0;
}

bool GetServer(char* call, short port, struct sockaddr* from, int* flen)
{
    SOCKET cC;
    SOCKADDR_IN all;
    all.sin_family = AF_INET;
    all.sin_port = htons(port);
    //all.sin_addr.s_addr = INADDR_BROADCAST;
    //all.sin_addr.s_addr = inet_addr("172.20.10.15");
    all.sin_addr.s_addr = inet_addr("192.168.43.255");


    int optval = 1;
    char bfrom[50];

    int timeout = 1000;

    try {
        if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)                                 throw SetErrorMsgText("socket: ", WSAGetLastError());
        if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR)      throw SetErrorMsgText("opt: ", WSAGetLastError());
        if (setsockopt(cC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int)) == SOCKET_ERROR)      throw SetErrorMsgText("opt:", WSAGetLastError());

        if (sendto(cC, call, strlen(call) + 1, NULL, (sockaddr*)&all, sizeof(all)) == SOCKET_ERROR)     throw SetErrorMsgText("send:", WSAGetLastError());
        if (recvfrom(cC, bfrom, sizeof(bfrom), NULL, from, flen) == SOCKET_ERROR)                       throw SetErrorMsgText("recvfrom:", WSAGetLastError());
        if (strcmp(call, bfrom) != 0)  return false; 

        if (closesocket(cC) == SOCKET_ERROR)                                                            throw SetErrorMsgText("CloseSocket: ", WSAGetLastError());

        cout << "Server IP: " << inet_ntoa(((struct sockaddr_in*)from)->sin_addr) << endl;
        cout << "Server Port: " << ntohs(((struct sockaddr_in*)from)->sin_port) << endl;
        return true;
    }
    catch (string errorMsgText)
    {
        if (WSAGetLastError() == WSAETIMEDOUT) {
            cout << "There is no server with that callsign" << endl;
            return false;
        }
        throw SetErrorMsgText("GetServer:", WSAGetLastError());
    }
}