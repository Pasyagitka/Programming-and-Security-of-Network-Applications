#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include "Winsock2.h"				// заголовок  WS2_32.dll
#pragma comment(lib, "WS2_32.lib")  // экспорт  WS2_32.dll

#include <iostream>
#include <Windows.h>
#include <string>
#include <Windows.h>

using namespace std;
string  GetErrorMsgText(int code);
string  SetErrorMsgText(string msgText, int code);


int main()
{
    SOCKET sS;
    WSADATA wsaData;					

	SOCKADDR_IN serv;                   
	serv.sin_family = AF_INET;         
	serv.sin_port = htons(2000);       
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");  

	SOCKET cS;							
	SOCKADDR_IN clnt;					

	char ibuf[50];                      
	string obuf;						
	int  libuf = 0,                     
		 lobuf = 0;                      

    try
    {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)					throw  SetErrorMsgText("Startup:", WSAGetLastError());
        if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)throw  SetErrorMsgText("socket:", WSAGetLastError());
		if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)	throw  SetErrorMsgText("bind:", WSAGetLastError());
		if (listen(sS, SOMAXCONN) == SOCKET_ERROR)						throw  SetErrorMsgText("listen:", WSAGetLastError());

		while (true) 
		{
			memset(&clnt, 0, sizeof(clnt));
			int lclnt = sizeof(clnt);

			if ((cS = accept(sS, (sockaddr*)&clnt, &lclnt)) == INVALID_SOCKET) throw SetErrorMsgText("accept:", WSAGetLastError());

			cout << endl << "Connected  " << endl;
			printf("IP  %s\n", inet_ntoa(clnt.sin_addr));
			cout << "Port  " << htons(clnt.sin_port) << endl;

			while (true)
			{
				memset(&clnt, 0, sizeof(clnt));
				libuf = recv(cS, ibuf, sizeof(ibuf), NULL);

				if (libuf == SOCKET_ERROR)					throw SetErrorMsgText("recv: ", WSAGetLastError());

				if (libuf == 0) {
					cout << "connection closed...\n";
					break;
				}

				if (ibuf[0] == '\0') {
					cout << "Empty message! Stopping server....";
					if (closesocket(cS) == SOCKET_ERROR)   throw SetErrorMsgText("closesocket:", WSAGetLastError());
					if (closesocket(sS) == SOCKET_ERROR)   throw SetErrorMsgText("closesocket:", WSAGetLastError());
					if (WSACleanup() == SOCKET_ERROR)	   throw SetErrorMsgText("Cleanup:", WSAGetLastError());
					return 0;
				}
				cout << endl << "recieved:" << ibuf << endl;

				string obuf = ibuf;
				if ((lobuf = send(cS, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL)) == SOCKET_ERROR)	throw SetErrorMsgText("send:", WSAGetLastError());
			}
			if (closesocket(cS) == SOCKET_ERROR)   throw SetErrorMsgText("closesocket:", WSAGetLastError());
		}
		if (closesocket(sS) == SOCKET_ERROR)			throw SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)				throw SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << "WSAGetLastError: " << errorMsgText;
	}
	return 0;
}

