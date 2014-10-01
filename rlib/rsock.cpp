#ifdef _MSC_VER
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rsock.h"

int rsock::s_socket(int af,int type,int protocol)
{
	return ::socket(af,type,protocol);
}

int rsock::s_connect(int s,void* name,int namelen)
{
	return ::connect(s,(const struct sockaddr*)name,namelen);
}

int rsock::s_close(int s)
{
#ifdef _MSC_VER
	return ::closesocket(s);
#else
	return ::close(s);
#endif
}

int rsock::s_send(int s,void* data,int size,int flags)
{
	return ::send(s,(const char*)data,size,flags);
}

int rsock::s_recv(int s,void* data,int size,int flags)
{
	return ::recv(s,(char*)data,size,flags);
}

int rsock::s_bind(int s,void* addr,int namelen)
{
	return ::bind(s,(const struct sockaddr*)addr,namelen);
}

int rsock::s_listen(int s,int backlog)
{
	return ::listen(s,backlog);
}

int rsock::s_accept(int s,void* addr,int* addrlen)
{
#ifdef _MSC_VER
	return ::accept((SOCKET)s,(struct sockaddr*)addr,addrlen);
#else
	return ::accept(s,(struct sockaddr*)addr,addrlen);
#endif
}

rsock::rsock()
{
#ifdef _MSC_VER
	m_socket=INVALID_SOCKET;
#else
	m_socket=-1;
#endif
}

rsock::~rsock()
{
#ifdef _MSC_VER
	if(m_socket!=INVALID_SOCKET)
	{
		close();
	}
#else
	if(m_socket<0)
	{
		close();
	}
#endif
}

rbool rsock::socket()
{
	m_socket=::socket(AF_INET,SOCK_STREAM,0);
#ifdef _MSC_VER
	return m_socket!=INVALID_SOCKET;
#else
	return m_socket>=0;
#endif
}

rbool rsock::connect(const char *hostname,int port)
{
	struct hostent *hp=gethostbyname(hostname);

	if(null==hp)
	{
		return false;
	}

	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
#ifdef _MSC_VER
	memcpy(&addr.sin_addr.S_un.S_addr,hp->h_addr_list[0],hp->h_length);
#else
	memcpy(&addr.sin_addr.s_addr,hp->h_addr_list[0],hp->h_length);
#endif
	addr.sin_port=htons((ushort)port);

	int nRevTime = c_time_out*1000;
	setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char *)&nRevTime,r_size(nRevTime));
	setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nRevTime,r_size(nRevTime));
	//设置非阻塞方式连接 
#ifdef _MSC_VER
	unsigned long ul=1;

	if(SOCKET_ERROR==ioctlsocket(m_socket,FIONBIO,(unsigned long*)&ul))
	{
		return false; 
	}
	
	if(SOCKET_ERROR!=::connect(m_socket,(struct sockaddr *)&addr,r_size(addr)))
	{
		return false;
	}

	struct timeval timeout; 
	fd_set r;
	FD_ZERO(&r);
	FD_SET(m_socket,&r);
	timeout.tv_sec=c_time_out;//连接超时
	timeout.tv_usec=0;
	
	if(select(0,0,&r,0,&timeout)<=0)
	{
		return false;
	}

	ul=0; 

	if(SOCKET_ERROR==ioctlsocket(m_socket,FIONBIO,(unsigned long*)&ul))
	{
		return false;
	}
#else
	/*if(0!=fcntl(m_socket, F_SETFL, fcntl(m_socket, F_GETFL) | O_NONBLOCK))
	{
		return false;
	}*/

	unsigned long ul = 1;

	if(0!=ioctl(m_socket, FIONBIO, (unsigned long*)&ul))
	{
		return false; 
	}
	
	if(0<=::connect(m_socket,(struct sockaddr *)&addr,r_size(addr)))
	{
		printf("2\n");
		return false;
	}

	struct timeval timeout ; 
	fd_set r;
	FD_ZERO(&r);
	FD_SET(m_socket, &r);
	timeout.tv_sec = c_time_out;//连接超时
	timeout.tv_usec =0;
	
	if(select(m_socket+1,0,&r,0,&timeout)<=0)
	{
		return false;
	}

	ul=0; 

	if(0!=ioctl(m_socket,FIONBIO,(unsigned long*)&ul))
	{
		return false; 
	}
#endif

	return true;
}

rbool rsock::close()
{
#ifdef _MSC_VER
	int ret=closesocket(m_socket);
	m_socket=INVALID_SOCKET;
#else
	int ret=::close(m_socket);
	m_socket=-1;
#endif
	return 0==ret;
}

rbool rsock::close(rsocket socket)
{
#ifdef _MSC_VER
	int ret=closesocket(socket);
#else
	int ret=::close(socket);
#endif
	return 0==ret;
}

int rsock::send(int size,const void* data)
{
#ifdef _MSC_VER
	int ret=::send(m_socket,(char *)data,size,0);
#else
	int ret=::send(m_socket,(char *)data,size,MSG_NOSIGNAL);
#endif
	/*if(ret<=0)
	{
		return -1;
	}*/
	return ret;
}

int rsock::recv(int size,void* data)
{
#ifdef _MSC_VER
	int ret=::recv(m_socket,(char *)data,size,0);
#else
	int ret=::recv(m_socket,(char *)data,size,MSG_NOSIGNAL);
#endif
	/*if(ret<=0)
	{
		return -1;
	}*/
	return ret;
}

int rsock::send_all(int size,const void* data)
{
	int sendBytes=0;
	while(sendBytes<size)
	{
		int temp=send(size-sendBytes,(char *)data+sendBytes);
		if(temp<=0)
		{
			return sendBytes;
		}
		elif(temp<size-sendBytes)
		{
			return sendBytes+temp;
		}
		sendBytes+=temp;
	}
	return sendBytes;
}

rbool rsock::bind(int port)
{
	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
#ifdef _MSC_VER
	addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
#else
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
#endif
	addr.sin_port=htons((ushort)port);
	return ::bind(m_socket,(struct sockaddr *)&addr,r_size(addr))>=0;
}

rbool rsock::listen()
{
#ifdef _MSC_VER
	return ::listen(m_socket,5)>=0;
#else
	return ::listen(m_socket,5)>=0;
#endif
}

rbool rsock::accept(rsocket* socket,char* clientip,int* port)
{
	struct sockaddr_in addr;
	int len=r_size(addr);
	
#ifdef _MSC_VER
	*socket=::accept(m_socket,(struct sockaddr *)&addr,&len);

	if(INVALID_SOCKET==*socket)
	{
		return false;
	}
#else
	*socket=::accept(m_socket,(struct sockaddr *)&addr,(socklen_t *)&len);

	if(*socket<0)
	{
		return false;
	}
#endif
	return true;
}

rbool rsock::listen_direct(int port)
{
	return socket()&&bind(port)&&listen();
}

rbool rsock::connect_direct(const char *hostname,int port)
{
	return socket()&&connect(hostname,port);
}

rbool rsock::sends(rstr s)
{
	if(4!=send(4,&s.m_buf.m_count))
	{
		return false;
	}
	return s.count()==send_all(s.count(),s.begin());
}

rstr rsock::recvs()
{
	rstr ret;
	int size;
	if(recv(4,&size)!=4)
	{
		return ret;
	}
	if(size<=0)
	{
		return ret;
	}
	ret.m_buf.realloc_n(size);
	int recvBytes=0;
	while(recvBytes<size)
	{
		int temp=recv(size-recvBytes,ret.begin()+recvBytes);
		if(temp<=0)
		{
			ret.clear();
			return ret;
		}
		recvBytes+=temp;
	}
	if(recvBytes!=size)
	{
		ret.clear();
	}
	return ret;
}