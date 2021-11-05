#include <iostream>
#include <string>
#include "Windows.h"
using namespace std;

string GetErrorMsgText(int code);
string SetPipeError(string msgText, int code);

//#define NAME L"\\\\.\\pipe\\Tube"
#define NAME L"\\\\PERKALPC\\pipe\\Tube"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	HANDLE cH;
	string stringbuf;
	char rbuf[50];
	char wbuf[50];

	LPDWORD wbytes = 0, rbytes = 0;
	DWORD wb;

	int messagesNumber;
	clock_t begin, end;

	try
	{
		if ((cH = CreateFile(NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
			throw  SetPipeError("CreateFile: ", GetLastError());

		cout << "Number of messages: ";
		cin >> messagesNumber;

		begin = clock();

		if (messagesNumber == 0) {
			strcpy_s(wbuf, "\0");
			if (!TransactNamedPipe(cH, wbuf, sizeof(wbuf), rbuf, sizeof(rbuf), &wb, NULL))				throw SetPipeError("TransactNamedPipe 0: ", GetLastError());
			cout << "Shutting down the server..." << endl;
		}
		else {
			for (int i = 0; i < messagesNumber; i++) {
				stringbuf = "Hello from Client " + to_string(i);
				strcpy_s(wbuf, stringbuf.c_str());

				if (!TransactNamedPipe(cH, wbuf, sizeof(wbuf), rbuf, sizeof(rbuf), &wb, NULL))				throw SetPipeError("TransactNamedPipe 1: ", GetLastError());
				cout << "Return from server: " << rbuf << endl;

				//strcpy_s(wbuf, ("Hello from Client " + to_string(stoi(stringbuf.substr(18)) + 1) + "\0").c_str());
				//if (!TransactNamedPipe(cH, wbuf, sizeof(wbuf), rbuf, sizeof(rbuf), &wb, NULL))				throw SetPipeError("TransactNamedPipe 2: ", GetLastError());
			}
		}
		end = clock();
		cout << "Exchange time: " << ((double)(end - begin) / CLK_TCK) << " sec." << endl;

		if (!CloseHandle(cH)) throw SetPipeError("CloseHandle: ", GetLastError());
	}
	catch (string ErrorPipeText)
	{
		cout << endl << ErrorPipeText;
	}
	return 0;
}