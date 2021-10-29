#pragma warning(disable:4996) 
#include "Winsock2.h" // заголовок WS2_32.dll
#pragma comment(lib, "WS2_32.lib") // экспорт WS2_32.dll
#include <iostream>
#include <string>
using namespace std;


string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);

int main(int argc, char* argv[])
{
	SOCKET cC;
	WSADATA wsaData;

	//3
	SOCKADDR_IN server;
	int lserver = sizeof(server);

	server.sin_family = AF_INET; // используется IP-адресация 
	server.sin_port = htons(2000); // TCP-порт 2000
	//server.sin_addr.s_addr = inet_addr("192.168.100.5");
	//server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_addr.s_addr = inet_addr("172.20.10.4"); 


	string bto; //буфер вывода
	char bfrom[50]; //буфер ввода

	int bytesto = 0, //количество принятых байт
		bytesfrom = 0; //количество отправленных байь

	int messagesNumber;

	clock_t begin, end;
	try
	{
		//1
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) throw SetErrorMsgText("Startup:", WSAGetLastError());
		//2
		if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)	throw SetErrorMsgText("socket:", WSAGetLastError());

		cout << "Number of messages: ";
		cin >> messagesNumber;

		begin = clock();

		//Остановить работу сервера - получение сообщения нулевой длины
		if (messagesNumber == 0) {
			bto = "";
			if ((bytesto = sendto(cC, bto.c_str(), strlen(bto.c_str()) + 1, NULL, (SOCKADDR*)&server, lserver)) == SOCKET_ERROR)
				throw SetErrorMsgText("sendto:", WSAGetLastError());
		}
		else
		for (int i = 0; i < messagesNumber; i++) {
			//bto = "HELLO"; //4 lab
			bto = "Hello from ClientU " + to_string(i);
			cout << endl << "Sending message: " + bto;

			//3
			if ((bytesto = sendto(cC, bto.c_str(), strlen(bto.c_str()) + 1, NULL, (SOCKADDR*)&server, lserver)) == SOCKET_ERROR) 	throw SetErrorMsgText("sendto:", WSAGetLastError());

			//4
			//if ((bytesfrom = recvfrom(cC, bfrom, sizeof(bfrom), NULL, (SOCKADDR*)&server, &lserver)) == SOCKET_ERROR) 	throw SetErrorMsgText("recvfrom:", WSAGetLastError());
			//cout << "\t---\t" << "Return from server - " << bfrom;
			//

			//string sbuf = bfrom;
			//bto = "Hello from ClientU " + to_string(stoi(sbuf.substr(18)) + 1);
			//if ((bytesto = sendto(cC, bto.c_str(), strlen(bto.c_str()) + 1, NULL, (SOCKADDR*)&server, lserver)) == SOCKET_ERROR)	throw SetErrorMsgText("send:", WSAGetLastError());
			//if ((bytesfrom = recvfrom(cC, bfrom, sizeof(bfrom), NULL, (SOCKADDR*)&server, &lserver)) == SOCKET_ERROR)				throw SetErrorMsgText("recv:", WSAGetLastError());
		}
		end = clock();
		cout << endl << "Exchange time: " << ((double)(end - begin) / CLK_TCK) << " sec." << endl;

		//5
		if (closesocket(cC) == SOCKET_ERROR)				    			throw SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)									throw SetErrorMsgText("cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << "WSAGetLastError: " << errorMsgText;
	}
	return 0;
}