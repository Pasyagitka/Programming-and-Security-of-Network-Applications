#include <iostream>
#include <string>
#include "Windows.h"
#pragma warning(disable : 4996)

using namespace std;

string GetErrorMsgText(int code);
string SetPipeError(string msgText, int code);

//#define NAME L"\\\\PERKALPC\\pipe\\Tube"
#define NAME L"\\\\PasyagitkaASUS\\pipe\\Tube"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	HANDLE cH;
	char rbuf[50];
	char wbuf[50];

	DWORD rbufsize = sizeof(rbuf);
	DWORD wbufsize = sizeof(wbuf);
	LPDWORD wbytes = 0, rbytes = 0;

	int messagesNumber;
	clock_t begin, end;
	try
	{
		cout << "Number of messages: ";
		cin >> messagesNumber;

		begin = clock();

		if (messagesNumber == 0) {
			strcpy_s(wbuf, "\0");
			if (!CallNamedPipe(NAME, wbuf, wbufsize, rbuf, rbufsize, &rbufsize, NMPWAIT_WAIT_FOREVER)) 			throw SetPipeError("CallNamedPipe 0: ", GetLastError());
			cout << "Shutting down the server..." << endl;
		}
		else
			for (int i = 0; i < messagesNumber; i++) {
				strcpy_s(wbuf, ("Hello from Client " + to_string(i)).c_str());

				if (!CallNamedPipe(NAME, wbuf, wbufsize, rbuf, rbufsize, &rbufsize, NMPWAIT_WAIT_FOREVER)) 			throw SetPipeError("CallNamedPipe 1: ", GetLastError());
				cout << "Return from server: " << rbuf << endl;

				string sbuf = rbuf;
				strcpy_s(wbuf, ("Hello from Client " + to_string(stoi(sbuf.substr(18)) + 1) + "\0").c_str());
				if (!CallNamedPipe(NAME, wbuf, wbufsize, rbuf, rbufsize, &rbufsize, NMPWAIT_WAIT_FOREVER)) 			throw SetPipeError("CallNamedPipe 2: ", GetLastError());
			}
		end = clock();
		cout << "Exchange time: " << ((double)(end - begin) / CLK_TCK) << " sec." << endl;
	}
	catch (string ErrorPipeText)
	{
		cout << endl << ErrorPipeText;
	}
	return 0;
}