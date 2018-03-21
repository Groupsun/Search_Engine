#include <WinSock2.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <WS2tcpip.h>
using namespace std;

#pragma comment(lib, "WS2_32.lib")

void main(int argc, char **argv)
{
	// input;
	//getline(cin, input);
	//ofstream out("out.txt");

	WSADATA wsaData;
	string input;
	int i = 1;
	while (argv[i] != NULL)
	{
		input += argv[i];
		i++;
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed to load Winsock");
		return;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(8080);
	inet_pton(AF_INET, "127.0.0.1", (void*)&addrSrv.sin_addr.S_un.S_addr);

	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == sockClient) {
		printf("Socket() error:%d", WSAGetLastError());
		return;
	}

	if (connect(sockClient, (struct  sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET) {
		printf("Connect failed:%d", WSAGetLastError());
		return;
	}
	else
	{
		int iSend = send(sockClient, input.c_str(), 100, 0);
		if (iSend == SOCKET_ERROR) {
			printf("send failed");
			return;
		}
	}

	int exetime;
	char et[20];
	recv(sockClient, et, 20, 0);
	sscanf_s(et, "%d", &exetime);
	cout << exetime << endl;
	char strint[10];
	recv(sockClient, strint, sizeof(strint), 0);
	int recvsize;
	sscanf_s(strint, "%d", &recvsize);
	cout << recvsize << endl;
	char url[1000], title[1000], outline[10000];
	for (int i = 0; i < recvsize; i++)
	{
		recv(sockClient, url, 1000, 0);
		recv(sockClient, title, 1000, 0);
		recv(sockClient, outline, 10000, 0);
		cout << url << endl;
		cout << title << endl;
		cout << outline << endl;
	}

	//system("pause");
	//out.close();

	closesocket(sockClient);
	WSACleanup();
}