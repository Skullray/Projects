// FTPClient.cpp : Defines the entry point for the console application.
//

#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <iostream>

#define BUFFER_LENGTH 512

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

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

	//cleanup
	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	WSACleanup();
	ExitProcess(dw);
}

int _tmain()
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	ADDRINFOW *result = NULL, *ptr = NULL, hints;
	char sendbuff[BUFFER_LENGTH];
	wchar_t *FilePath;
	wchar_t *ServerAddr;
	wchar_t *Port;
	int iResult;

	std::wcout << _T("Enter The File To Transfer: ");
	std::wcin.getline(FilePath, MAX_PATH);

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		ErrorExit(_T("WSAStartup"));
	}

	HANDLE hFile = CreateFile(FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		ErrorExit(_T("CreateFile"));
	}

	for (int i = wcslen(FilePath); i > 0; i--) {
		if (FilePath[i-1] == '\\') {
			for (int j = 0; j < wcslen(FilePath) - (i - 1); j++) {
				sendbuff[j] = static_cast<char>(FilePath[i + j]);
			}
		}
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	std::wcout << _T("Enter The Server IP: ");
	std::wcin.getline(ServerAddr, 16);

	std::wcout << _T("Enter The Port: ");
	std::wcin.getline(Port, 7);

	iResult = GetAddrInfo(ServerAddr, Port, &hints, &result);
	if (iResult != 0) {
		ErrorExit(_T("GetAddrInfo"));
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {		//Connect Loop

		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			ErrorExit(_T("socket"));
		}

		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	FreeAddrInfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		ErrorExit(_T("Connect Loop"));
	}

	iResult = send(ConnectSocket, sendbuff, strlen(sendbuff), 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ErrorExit(_T("send"));
	}

	while (TRUE) {
		LPDWORD dwBytesRead;
		if (FALSE == ReadFile(hFile, sendbuff, BUFFER_LENGTH, dwBytesRead, NULL)) {
			ErrorExit(_T("ReadFile"));
		}
		iResult = send(ConnectSocket, sendbuff, *dwBytesRead, 0);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ErrorExit(_T("File send"));
		}

		if (BUFFER_LENGTH > *dwBytesRead) {
			iResult = shutdown(ConnectSocket, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				closesocket(ConnectSocket);
				ErrorExit(_T("Shutdown"));
			}
			break;
		}
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

    return 0;
}

