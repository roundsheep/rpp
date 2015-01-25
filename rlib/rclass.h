#ifndef rclass_h__
#define rclass_h__

#include "rbuf.h"
#include "rf.h"
#ifdef _MSC_VER
#include <string>
#pragma comment(lib,"Ws2_32")
#else
#include "code.txt"
#endif

struct rclass//本类用于初始化环境，只能实例化一次（单例模式）
{
	rclass()
	{
#ifdef _MSC_VER
		std::set_new_handler(new_fail_handle);

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

	~rclass()
	{
#ifdef _MSC_VER
		WSACleanup();
#endif
	}

	static void new_fail_handle()
	{
		rf::error("memory insufficient");
	}
};

#endif