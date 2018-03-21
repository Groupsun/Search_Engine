#include <winsock2.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>
#include "Self_Define_Functions.h"
#include "header.h"
using namespace std;

#pragma comment(lib, "WS2_32.lib")

Searcher searcher;
int sizeofpage;

void load()
{
	long beg, end;
	beg = clock();
	readwebpage();
	savefile();
	//readfile();
	end = clock();
	cout << end - beg << endl;
}

void searchcore()
{
	//scanf("%d", &sizeofpage);
	sizeofpage = 128444;
	initNLPIR();
	readfile();
	readoutpage(sizeofpage);
	//mainsearch(string);
}

std::string UTF8_To_GBK(const std::string &source)
{
	enum { GB2312 = 936 };

	unsigned long len = ::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, NULL, NULL);
	if (len == 0)
		return std::string();
	wchar_t *wide_char_buffer = new wchar_t[len];
	::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, wide_char_buffer, len);

	len = ::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, NULL, NULL, NULL, NULL);
	if (len == 0)
	{
		delete[] wide_char_buffer;
		return std::string();
	}
	char *multi_byte_buffer = new char[len];
	::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, multi_byte_buffer, len, NULL, NULL);

	std::string dest(multi_byte_buffer);
	delete[] wide_char_buffer;
	delete[] multi_byte_buffer;
	return dest;
}

std::string GBK_To_UTF8(const std::string &source)
{
	enum { GB2312 = 936 };

	unsigned long len = ::MultiByteToWideChar(GB2312, NULL, source.c_str(), -1, NULL, NULL);
	if (len == 0)
		return std::string();
	wchar_t *wide_char_buffer = new wchar_t[len];
	::MultiByteToWideChar(GB2312, NULL, source.c_str(), -1, wide_char_buffer, len);

	len = ::WideCharToMultiByte(CP_UTF8, NULL, wide_char_buffer, -1, NULL, NULL, NULL, NULL);
	if (len == 0)
	{
		delete[] wide_char_buffer;
		return std::string();
	}
	char *multi_byte_buffer = new char[len];
	::WideCharToMultiByte(CP_UTF8, NULL, wide_char_buffer, -1, multi_byte_buffer, len, NULL, NULL);

	std::string dest(multi_byte_buffer);
	delete[] wide_char_buffer;
	delete[] multi_byte_buffer;
	return dest;
}

char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 2];
	memset(wstr, 0, len + 2);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 2];
	memset(str, 0, len + 2);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

int main()
{
	searchcore();
	WSADATA wsadata;
	int port = 8080;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		printf("Failed to load Winsock");
		return -1;
	}

	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port);
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retVal = bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
	if (retVal == SOCKET_ERROR) {
		printf("Failed bind:%d\n", WSAGetLastError());
		return -1;
	}

	if (listen(sockSrv, 10) == SOCKET_ERROR) {
		printf("Listen failed:%d", WSAGetLastError());
		return -1;
	}

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);

	int start, finish, exetime;

	while (1)
	{
		SOCKET sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
		if (sockConn == SOCKET_ERROR) {
			printf("Accept failed:%d", WSAGetLastError());
			break;
		}
		printf("Accept client IP:[%s]\n", inet_ntoa(addrClient.sin_addr));

		char recvBuf[100];
		memset(recvBuf, 0, sizeof(recvBuf));
		recv(sockConn, recvBuf, sizeof(recvBuf), 0);
		string input = recvBuf;
		cout << input << endl;
		start = clock();
		Searcher *result = mainsearch(input);
		finish = clock();
		exetime = finish - start;
		char et[20];
		sprintf(et, "%d", exetime);
		int iSendtime = send(sockConn, et, 20, 0);
		int outputsize = result->iv.size();
		char strint[10];
		sprintf(strint, "%d", outputsize);
		int iSendfirst = send(sockConn, strint, sizeof(strint), 0);
		if (iSendfirst == SOCKET_ERROR) {
			printf("send failed");
			break;
		}

		vector<pair<int, double>>::iterator it;
		string tmp = "", next = "";
		string soutline;
		char url[1000];
		char title[1000];
		char outline[10000];
		int iSend1, iSend2, iSend3;
		for (it = result->iv.begin(); it != result->iv.end(); it++)
		{
			next = pagelist[it->first].geturl();
			soutline = GBK_To_UTF8(pagelist[it->first].getoutline());
			strcpy(url, next.c_str());
			strcpy(title, pagelist[it->first].gettitle().c_str());
			strcpy(outline, soutline.c_str());
			if (tmp == "" || tmp != next)
			{
				iSend1 = send(sockConn, url, 1000, 0);
				if (iSend1 == SOCKET_ERROR) {
					printf("send failed");
					break;
				}
				iSend2 = send(sockConn, title, 1000, 0);
				if (iSend2 == SOCKET_ERROR) {
					printf("send failed");
					break;
				}
				iSend3 = send(sockConn, outline, 10000, 0);
				if (iSend3 == SOCKET_ERROR) {
					printf("send failed");
					break;
				}
			}
			tmp = next;
		}
		closesocket(sockConn);
	}

	closesocket(sockSrv);
	WSACleanup();
	//system("pause");
	return 0;
}