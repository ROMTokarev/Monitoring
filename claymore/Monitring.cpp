
#include "stdafx.h"
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <locale>
#include <sstream>

#include "sha256.h"

using namespace std;
#pragma comment(lib,"ws2_32.lib")


string website_HTML;
locale local;
bool get_Website(string url);
char buffer[10000];
int i = 0;

void usleep(__int64 usec)
{
	HANDLE timer;
	LARGE_INTEGER ft;

	ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}


//****************************************************

int main(void){
	for (int i=0;;i++)
	{

	  if (get_Website("127.0.0.1") == true)
	  {
		  cout << "Kill EthDcrMiner64.exe" << endl;
		  system("taskkill /f /im EthDcrMiner64.exe > nul");
		  i = 0;
		  Sleep(100000);
	  }
	  else
	  {
		//  cout << website_HTML << "\n" << endl;
		  cout << sha256(website_HTML) << endl;
	  }

	  if (i == 2000)
	  {
		  cout << "Kill EthDcrMiner64.exe fot TIME" << endl;
		  system("taskkill /f /im EthDcrMiner64.exe > nul");
		  i = 0;
		  Sleep(100000);
	  }

	  Sleep(5000);
	  website_HTML = "";
	}

}

//****************************************************

bool get_Website(string url){
	WSADATA wsaData;
	SOCKET Socket;
	SOCKADDR_IN SockAddr;
	int lineCount = 0;
	int rowCount = 0;
	struct hostent *host;
	string get_http;


	get_http = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";

	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0){
		cout << "WSAStartup failed.\n";
		return true;
	}

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	host = gethostbyname(url.c_str());

	SockAddr.sin_port = htons(3333);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0){
		return true;
	}

	send(Socket, get_http.c_str(), strlen(get_http.c_str()), 0);

	int iPendingBytes = 0, count;
	for (count = 0; count < 100 && iPendingBytes == 0; count++)
	{
		ioctlsocket(Socket, FIONREAD, (u_long FAR*) &iPendingBytes);
		Sleep(100);
	}
	if (iPendingBytes == 0) return true;

	int nDataLength;
	while ((nDataLength = recv(Socket, buffer, 10000, 0)) > 0){
		int i = 0;
		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r'){

			website_HTML += buffer[i];
			i += 1;
		}
	}

	closesocket(Socket);
	WSACleanup();

}
