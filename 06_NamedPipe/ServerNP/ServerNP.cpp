#include <iostream>
#include <string>
#include "Windows.h"
using namespace std;

string GetErrorMsgText(int code);
string SetPipeError(string msgText, int code);

#define NAME L"\\\\.\\pipe\\Tube"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	HANDLE sH;
	char wbuf[50]{};
	char rbuf[50]{};
	LPDWORD wbytes = 0, rbytes = 0;
	try
	{
		SECURITY_DESCRIPTOR* m_pSecDesc = (SECURITY_DESCRIPTOR*)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		SECURITY_ATTRIBUTES m_pSecAttrib = { sizeof(SECURITY_ATTRIBUTES), m_pSecDesc, TRUE };
		InitializeSecurityDescriptor(m_pSecDesc, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(m_pSecDesc, TRUE, (PACL)NULL, FALSE);

		if ((sH = CreateNamedPipe(NAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL, INFINITE, &m_pSecAttrib)) == INVALID_HANDLE_VALUE) throw SetPipeError("Create:", GetLastError());

		while (true) {
			if (!ConnectNamedPipe(sH, NULL))									throw SetPipeError("Connect:", GetLastError());
			while (ReadFile(sH, rbuf, sizeof(rbuf), rbytes, NULL)) {
				if (!WriteFile(sH, rbuf, sizeof(rbuf), wbytes, NULL))			throw SetPipeError("WriteFile: ", GetLastError());
				cout << "recieved:" << rbuf << endl;

				if (rbuf[0] == '\0') {
					cout << "Shutting down the server..." << endl;
					if (!DisconnectNamedPipe(sH)) 								throw SetPipeError("DisconnectNamedPipe: ", GetLastError());
					if (!CloseHandle(sH))										throw SetPipeError("CloseHandle: ", GetLastError());
					return 0;
				}
			}
			if (!DisconnectNamedPipe(sH)) 										throw SetPipeError("DisconnectNamedPipe: ", GetLastError());
		}
		if (!CloseHandle(sH))													throw SetPipeError("CloseHandle: ", GetLastError());
	}
	catch (string ErrorPipeText)
	{
		cout << endl << ErrorPipeText;
	}
	return 0;
}