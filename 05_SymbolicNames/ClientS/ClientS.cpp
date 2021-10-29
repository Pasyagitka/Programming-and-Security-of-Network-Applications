#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:4996) 
#include "Winsock2.h" // заголовок WS2_32.dll
#pragma comment(lib, "WS2_32.lib") // экспорт WS2_32.dll
#include <iostream>
#include <string>
using namespace std;

string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);
bool GetServerByName(char* name, char* call, struct sockaddr* from, int* flen);

int main(int argc, char* argv[])
{
    //char NAME[15] = "PasyagitkaASUS";
    char NAME[9] = "PERKALPC";

    char CALL[6] = "Hello";
    WSADATA wsaData;
    SOCKET cS;
    SOCKADDR_IN server;
    int lserver = sizeof(server);

    cout << "Client: " << CALL << endl;
    try
    {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)	                    throw SetErrorMsgText("WSAStartup:", WSAGetLastError());
        if (!GetServerByName(NAME, CALL, (sockaddr*)&server, &lserver))     cout << "GetServerByName returned false" << endl;
        if (WSACleanup() == SOCKET_ERROR)				                    throw SetErrorMsgText("WSACleanup:", WSAGetLastError());
    }
    catch (string errorMsgText)
    {
        cout << endl << "WSAGetLastError: " << errorMsgText;
    }
    return 0;
}

bool GetServerByName(char* name, char* call, struct sockaddr* from, int* flen)
{
    try {
        hostent *host = gethostbyname(name);
        if (host == NULL) throw GetErrorMsgText(WSAGetLastError());

        SOCKET cC;
        SOCKADDR_IN server;
        server.sin_family = AF_INET;
        server.sin_port = htons(2000);

        in_addr* addr_list = (in_addr*)*(host->h_addr_list);
        server.sin_addr = *addr_list;
        int i = 0;
        struct in_addr addr;
        while (host->h_addr_list[i] != 0) {
            addr.s_addr = *(u_long*)host->h_addr_list[i++];
            printf("\tIPv4 Address #%d: %s\n", i, inet_ntoa(addr));
        }
        server.sin_addr.s_addr = ((in_addr*)host->h_addr_list[0])->s_addr;


        int optval = 1;
        char bfrom[50];

        int timeout = 1000;
   
        if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)                                         throw SetErrorMsgText("socket: ", WSAGetLastError());
        if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR)              throw SetErrorMsgText("opt: ", WSAGetLastError());
        if (setsockopt(cC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int)) == SOCKET_ERROR)              throw SetErrorMsgText("opt:", WSAGetLastError());

        if (sendto(cC, call, strlen(call) + 1, NULL, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)       throw SetErrorMsgText("send:", WSAGetLastError());
        if (recvfrom(cC, bfrom, sizeof(bfrom), NULL, from, flen) == SOCKET_ERROR)                               throw SetErrorMsgText("recvfrom:", WSAGetLastError());

        if (closesocket(cC) == SOCKET_ERROR)                                                                    throw SetErrorMsgText("CloseSocket: ", WSAGetLastError());

        cout << "Server Name: " << name << endl;
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
        throw SetErrorMsgText("GetServerByName:", WSAGetLastError());
    }
}