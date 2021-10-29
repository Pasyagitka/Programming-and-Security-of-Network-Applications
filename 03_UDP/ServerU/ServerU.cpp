#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Winsock2.h" // заголовок WS2_32.dll
#pragma comment(lib, "WS2_32.lib") // экспорт WS2_32.dll
#include <iostream>
#include <string>

using namespace std;

string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);


int main(int argc, char* argv[])
{
    SOCKET sS;
    WSADATA wsaData;

    SOCKADDR_IN server;
    server.sin_family = AF_INET; // используется IP-адресация 
    server.sin_port = htons(2000); // порт 2000
    server.sin_addr.s_addr = INADDR_ANY; // любой собственный IP

    SOCKADDR_IN client;
    memset(&client, 0, sizeof(client)); // обнулить память
    int lclient = sizeof(client); // размер SOCKADDR_IN
    
    char bfrom[50]; //буфер ввода
    string bto; //буфер вывода

    int bytesfrom = 0,
        bytesto = 0;

    int accepted = 0;

    try
    {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)          throw SetErrorMsgText("Startup:", WSAGetLastError());
        if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) throw SetErrorMsgText("socket:", WSAGetLastError());
        if (bind(sS, (LPSOCKADDR)&server, sizeof(server)) == SOCKET_ERROR)  throw SetErrorMsgText("bind:", WSAGetLastError());

       
        while (true)
        {
            memset(&client, 0, sizeof(client));
            
            //loss
            //Sleep(10);

            (bytesfrom = recvfrom(sS, bfrom, sizeof(bfrom), NULL, (sockaddr*)&client, &lclient));
            if (bytesfrom == SOCKET_ERROR) { throw SetErrorMsgText("recvfrom: ", WSAGetLastError()); }
            if (bytesfrom == 0) {
                cout << endl << "Connection closed...\n";
                break;
            }
            accepted++;

            if (bfrom[0] == '\0') {
                cout << endl << "Accepted: " << accepted << endl;
                cout << "Empty message! Stopping server....";
                if (closesocket(sS) == SOCKET_ERROR)                throw SetErrorMsgText("closesocket:", WSAGetLastError());
                if (WSACleanup() == SOCKET_ERROR)                   throw SetErrorMsgText("Cleanup:", WSAGetLastError());
                return 0;
            }
            cout << endl << "Recieved from client: " << bfrom;
            
            //loss
            cout << "\t---\t" << "Sending return to client... ";
            bto = bfrom;
            if ((bytesto = sendto(sS, bto.c_str(), strlen(bto.c_str()) + 1, NULL, (sockaddr*)&client, lclient)) == SOCKET_ERROR) throw SetErrorMsgText("sendto: ", WSAGetLastError());
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