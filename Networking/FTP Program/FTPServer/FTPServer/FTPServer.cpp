// FTPServer.cpp : Defines the entry point for the console application.
//

#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"
#include <WinSock2.h>
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

#define BUFFER_LENGTH 512
#define DEFAULT_PORT L"12345"

void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	WSACleanup();
	ExitProcess(dw);
}


int _tmain()
{
	std::wcout << _T("Enter Port To Start Server: ");
	UINT port;
	std::wcin >> port;

	WSADATA wsaData;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	std::wcout << _T("Starting Server...\n");

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorExit(_T("WSAStartup"));
	}

	ADDRINFOW hints, *result = NULL;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if (GetAddrInfo(NULL, DEFAULT_PORT, &hints, &result) != 0) {
		ErrorExit(_T("WSAStartup"));
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		FreeAddrInfoW(result);
		ErrorExit(_T("ListenSocket"));
	}

	if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		FreeAddrInfoW(result);
		closesocket(ListenSocket);
		ErrorExit(_T("Bind"));
	}

	FreeAddrInfoW(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(ListenSocket);
		ErrorExit(_T("Listen"));
	}

	if ((ClientSocket = accept(ListenSocket, NULL, NULL)) == INVALID_SOCKET) {
		closesocket(ListenSocket);
		ErrorExit(_T("Accept"));
	}

	closesocket(ListenSocket);

	int iResult;
	char dataBuffer[BUFFER_LENGTH];

	HANDLE hFile = INVALID_HANDLE_VALUE;

	do {

		iResult = recv(ClientSocket, dataBuffer, BUFFER_LENGTH, 0);
		if (hFile == INVALID_HANDLE_VALUE) {
			wchar_t filename[BUFFER_LENGTH];
			std::mbstowcs(filename, dataBuffer, iResult);
			hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				ErrorExit(_T("CreateFile"));
			}
		}
		else {
			DWORD dwBytesWritten;
			BOOL bErrorFlag = WriteFile(hFile, dataBuffer, (DWORD)iResult, &dwBytesWritten, NULL);
			if (bErrorFlag == FALSE) {
				ErrorExit(_T("WriteFile"));
			}
		}


	} while (iResult > 0);

    return 0;
}
