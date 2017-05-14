// FTPClient.cpp : Defines the entry point for the console application.
//

#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <psapi.h>
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

BOOL GetFileNameFromHandle(HANDLE hFile, TCHAR *szTempFile)
{
	BOOL bSuccess = FALSE;
	TCHAR pszFilename[MAX_PATH + 1];
	HANDLE hFileMap;

	// Get the file size.
	DWORD dwFileSizeHi = 0;
	DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

	if (dwFileSizeLo == 0 && dwFileSizeHi == 0)
	{
		_tprintf(TEXT("Cannot map a file with a length of zero.\n"));
		return FALSE;
	}

	// Create a file mapping object.
	hFileMap = CreateFileMapping(hFile,
		NULL,
		PAGE_READONLY,
		0,
		1,
		NULL);

	if (hFileMap)
	{
		// Create a file mapping to get the file name.
		void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

		if (pMem)
		{
			if (GetMappedFileName(GetCurrentProcess(),
				pMem,
				pszFilename,
				MAX_PATH))
			{

				// Translate path with device name to drive letters.
				TCHAR szTemp[BUFFER_LENGTH];
				szTemp[0] = '\0';

				if (GetLogicalDriveStrings(BUFFER_LENGTH - 1, szTemp))
				{
					TCHAR szName[MAX_PATH];
					TCHAR szDrive[3] = TEXT(" :");
					BOOL bFound = FALSE;
					TCHAR* p = szTemp;

					do
					{
						// Copy the drive letter to the template string
						*szDrive = *p;

						// Look up each device name
						if (QueryDosDevice(szDrive, szName, MAX_PATH))
						{
							size_t uNameLen = _tcslen(szName);

							if (uNameLen < MAX_PATH)
							{
								bFound = _tcsnicmp(pszFilename, szName, uNameLen) == 0
									&& *(pszFilename + uNameLen) == _T('\\');

								if (bFound)
								{
									// Reconstruct pszFilename using szTempFile
									// Replace device path with DOS path
									StringCchPrintf(szTempFile,
										MAX_PATH,
										TEXT("%s%s"),
										szDrive,
										pszFilename + uNameLen);
									StringCchCopyN(pszFilename, MAX_PATH + 1, szTempFile, _tcslen(szTempFile));
								}
							}
						}

						// Go to the next NULL character.
						while (*p++);
					} while (!bFound && *p); // end of string
				}
			}
			bSuccess = TRUE;
			UnmapViewOfFile(pMem);
		}

		CloseHandle(hFileMap);
	}
	//_tprintf(TEXT("File name is %s\n"), pszFilename);
	return(bSuccess);
}

int _tmain()
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	ADDRINFOW *result = NULL, *ptr = NULL, hints;
	char sendbuff[BUFFER_LENGTH];
	wchar_t FilePath[MAX_PATH];
	wchar_t ServerAddr[20];
	wchar_t Port[10];
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

	TCHAR szTempFile[MAX_PATH];

	GetFileNameFromHandle(hFile, szTempFile);

	for (int i = wcslen(szTempFile); i > 0; i--) {
		if (szTempFile[i-1] == L'\\' || szTempFile[i-1] == L'/') {
			//std::wcout << i << _T(" ");
			for (int j = 0; j < wcslen(szTempFile) - (i - 1); j++) {
				sendbuff[j] = static_cast<char>(szTempFile[i + j]);
				//std::wcout << szTempFile[i + j];
			}
			break;
		}
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//std::wcout << _T("Enter The Server IP: ");
	//std::wcin.getline(ServerAddr, 16);
	wcscpy(ServerAddr, _T("192.168.0.103"));

	//std::wcout << _T("Enter The Port: ");
	//std::wcin.getline(Port, 7);
	wcscpy(Port, _T("12345"));

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

	int value = 1;
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &value, sizeof(value));
	iResult = send(ConnectSocket, sendbuff, strlen(sendbuff)+1, 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ErrorExit(_T("send"));
	}

	value = 0;
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&value, sizeof(value));

	//__asm int 3;

	while (TRUE) {
		DWORD dwBytesRead;
		if (FALSE == ReadFile(hFile, sendbuff, BUFFER_LENGTH, &dwBytesRead, NULL)) {
			ErrorExit(_T("ReadFile"));
		}
		iResult = send(ConnectSocket, sendbuff, dwBytesRead, 0);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ErrorExit(_T("File send"));
		}

		if (BUFFER_LENGTH > dwBytesRead) {
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

