#include <iostream>
#include <string>
#include "Windows.h"
using namespace std;

string GetErrorMsgText(int code);
string SetMailslotError(string msgText, int code);

//#define NAME L"\\\\.\\mailslot\\Box"
#define NAME L"\\\\*\\mailslot\\Box"						//cannot write more than 424 bytes at a time to the mailslot. ERROR_BAD_NETPATH
//#define NAME L"\\\\PasyagitkaASUS\\mailslot\\Box"			
#define MESSAGES_NUMBER 10

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	HANDLE cH;
	char wbuf[500]{};
	LPDWORD wbytes = 0, rbytes = 0;

	clock_t begin, end;
	try
	{
		if ((cH = CreateFile(NAME, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE) throw SetMailslotError("CreateFile: ", GetLastError());
		
		begin = clock();
		for (int i = 0; i < MESSAGES_NUMBER; i++) {
			string stringbuf = "Hello from Mailslot-client " + to_string(i);						//* - придет, . придет, PA придет
			//string stringbuf = "Hello from Mailslot-client " + to_string(i) + string(400, '.');	//* - не придет, . придет, PA не придет
			strcpy_s(wbuf, stringbuf.c_str());
			if (!WriteFile(cH, wbuf, strlen(wbuf)+1, wbytes, NULL)) throw SetMailslotError("WriteFile: ", GetLastError());
			cout << wbuf << endl;
		}
		end = clock();
		cout << "Exchange time: " << ((double)(end - begin) / CLK_TCK) << " sec." << endl;

		if (!CloseHandle(cH)) throw SetMailslotError("CloseHandle: ", GetLastError());
	}
	catch (string ErrorPipeText)
	{
		cout << endl << ErrorPipeText;
	}
	return 0;
}