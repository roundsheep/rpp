//实际大小为400
struct WSAData 
{
	ushort wVersion;
	ushort wHighVersion;
	char szDescription[257];
	char szSystemStatus[129];
	ushort iMaxSockets;
	ushort iMaxUdpDg;
	char* lpVendorInfo;
	
	~WSAData()
	{
		rfs.WSACleanup
	}
	
	WSAData()
	{
		rbuf<char> buf(400)
		rfs.WSAStartup(0x00000101.toushort,buf.begin)
	}
}

namespace rfs
{
	//ver is 2 bytes
	int WSAStartup(int ver,char* data)
	{
		push data
		push ver
		calle "WSAStartup"
		mov s_ret,eax
	}
	
	int WSACleanup()
	{
		calle "WSACleanup"
		mov s_ret,eax
	}
	
	int socket(int af,int type,int protocol)
	{
		push protocol
		push type
		push af
		calle "socket"
		mov s_ret,eax
	}

	int connect(int s,void* name,int namelen)
	{
		push namelen
		push name
		push s
		calle "connect"
		mov s_ret,eax
	}
	
	int close(int sock)
	{
		push sock
		calle "closesocket"
		mov s_ret,eax
	}
	
	int send(int s,void* data,int size,int flags)
	{
		push flags
		push size
		push data
		push s
		calle "send"
		mov s_ret,eax
	}

	int recv(int s,void* data,int size,int flags)
	{
		push flags
		push size
		push data
		push s
		calle "recv"
		mov s_ret,eax
	}
	
	int bind(int s,void* addr,int namelen)
	{
		push namelen
		push addr
		push s
		calle "bind"
		mov s_ret,eax
	}

	int listen(int s,int backlog)
	{
		push backlog
		push s
		calle "listen"
		mov s_ret,eax
	}

	int accept(int s,void* addr,int* addrlen)
	{
		push addrlen
		push addr
		push s
		calle "accept"
		mov s_ret,eax
	}
	
	char* gethostbyname(char* name)
	{
		void[rf.find_dll('gethostbyname'),name]
		mov s_ret,eax
	}
}

struct rfs.hostent 
{
	char* h_name;
	char** h_aliases;
	short h_addrtype;
	short h_length;
	char** h_addr_list;
};