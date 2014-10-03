#ifdef _MSC_VER
#include <winsock2.h>
#pragma comment(lib,"Ws2_32")
#else
#endif
#include "rclass.h"
#include "rf.h"

#ifndef _MSC_VER
#include "code.txt"
#endif

static rclass g_rclass;

void new_fail_handle()
{
	rf::error("memory insufficient");
}

rclass::rclass()
{
#ifdef _MSC_VER
	set_new_handler(new_fail_handle);

	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested=MAKEWORD(1,1);

	if(0!=WSAStartup(wVersionRequested,&wsaData))
	{
		return;
	}	

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		return;
	}
#endif
}

rclass::~rclass()
{
#ifdef _MSC_VER
	WSACleanup();
#endif
}
