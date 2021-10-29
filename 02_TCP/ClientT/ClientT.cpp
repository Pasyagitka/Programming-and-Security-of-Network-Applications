#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Winsock2.h"        
#pragma comment(lib, "WS2_32.lib")  

#include <iostream>
#include <Windows.h>
#include <string>
#include <Windows.h>
#include <ctime>

using namespace std;
string  GetErrorMsgText(int code);
string  SetErrorMsgText(string msgText, int code);


int main()
{
	SOCKET  cC;
	WSADATA wsaData;
	SOCKADDR_IN serv;         

	serv.sin_family = AF_INET;					   
	serv.sin_port = htons(2000);                  
	//serv.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	//serv.sin_addr.s_addr = inet_addr("172.20.10.11");
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");

	char ibuf[50]; 
	string obuf; 
	int libuf = 0, 
		lobuf = 0; 

	int messagesNumber;

	clock_t begin, end;
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) throw  SetErrorMsgText("Startup:", WSAGetLastError());
		if ((cC = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET) throw  SetErrorMsgText("socket:", WSAGetLastError());
		if ((connect(cC, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) throw  SetErrorMsgText("connect:", WSAGetLastError());

		cout << "Number of messages: ";
		cin >> messagesNumber;

		begin = clock();
		if (messagesNumber == 0) {
			obuf = "";
			if ((lobuf = send(cC, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL)) == SOCKET_ERROR)		throw SetErrorMsgText("send:", WSAGetLastError());
		}
		else 
		for (int i = 0; i < messagesNumber; i++) {
			obuf = "Hello from Client " + to_string(i);

			if ((lobuf = send(cC, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL)) == SOCKET_ERROR)		throw SetErrorMsgText("send:", WSAGetLastError());
			if ((libuf = recv(cC, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR)							throw SetErrorMsgText("recv:", WSAGetLastError());

			cout << "return from server" << ibuf << endl;
			string sbuf = ibuf;
			obuf = "Hello from Client " + to_string(stoi(sbuf.substr(18)) + 1);
			if ((lobuf = send(cC, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL)) == SOCKET_ERROR)		throw SetErrorMsgText("send:", WSAGetLastError());
			if ((libuf = recv(cC, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR)							throw SetErrorMsgText("recv:", WSAGetLastError());
		}
		end = clock();
		cout << "Exchange time: " << ((double)(end - begin) / CLK_TCK) << " sec." << endl;

		if (closesocket(cC) == SOCKET_ERROR)	throw  SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)		throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << "WSAGetLastError: " << errorMsgText;
	}

	return 0;
}
