// FTPServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>


int _tmain()
{
	std::wcout << _T("Starting Server....\n\nEnter Port To Start Server: ");
	UINT port;
	std::wcin >> port;
    return 0;
}

