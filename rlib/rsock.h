#ifndef rsock_h__
#define rsock_h__

#include "rbase.h"
#include <stdio.h>
#include "rstr.h"

#ifdef _MSC_VER
#include <windows.h>
typedef UINT_PTR rsocket;
#else
typedef int rsocket;
#endif

struct rsock//only use TCP
{
	rsocket m_socket;

	enum
	{
		c_time_out=10,
		c_max_ip_len=15,
	};

	rsock();
	~rsock();

	rbool socket();
	rbool connect(const char *hostname,int port);
	rbool close();
	rbool close(rsocket socket);
	int send(int size,const void* data);//failed will return -1
	int recv(int size,void* data);
	int send_all(int size,const void* data);
	rbool bind(int port);
	rbool listen();
	rbool accept(rsocket* socket,char* clientip=null,int* port=null);
	//clientip array at least c_max_ip_len+1 length
	rbool listen_direct(int port);
	rbool connect_direct(const char *hostname,int port);
	rbool sends(rstr s);
	rstr recvs();

	static int s_socket(int af,int type,int protocol);
	static int s_connect(int s,void* name,int namelen);
	static int s_close(int s);
	static int s_send(int s,void* data,int size,int flags);
	static int s_recv(int s,void* data,int size,int flags);
	static int s_bind(int s,void* addr,int namelen);
	static int s_listen(int s,int backlog);
	static int s_accept(int s,void* addr,int* addrlen);
};


#endif