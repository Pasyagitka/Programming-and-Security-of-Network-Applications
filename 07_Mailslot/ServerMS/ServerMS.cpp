#include <iostream>
#include <string>
#include "Windows.h"
using namespace std;

string GetErrorMsgText(int code);
string SetMailslotError(string msgText, int code);

#define NAME L"\\\\.\\mailslot\\Box"
#define THREE_MINUTES 3*60*1000
#define MAX_MESSAGE_LENGTH 500

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	HANDLE sH;
	char rbuf[500]{};
	LPDWORD wbytes = 0, rbytes = 0;

	try
	{
		wcout << "Mailslot name: " << NAME << endl;
		if ((sH = CreateMailslot(NAME, MAX_MESSAGE_LENGTH, THREE_MINUTES, NULL)) == INVALID_HANDLE_VALUE)		throw SetMailslotError("Create:", GetLastError());

		while (true) {
			if(!ReadFile(sH, rbuf, sizeof(rbuf), rbytes, NULL))		throw SetMailslotError("ReadFile: ", GetLastError());
			cout << "Received: " << rbuf << endl;
		}

		if (!CloseHandle(sH))	throw SetMailslotError("CloseHandle: ", GetLastError());
	}
	catch (string ErrorPipeText)
	{
		cout << endl << ErrorPipeText;
	}
	return 0;
}