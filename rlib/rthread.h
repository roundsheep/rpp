#ifndef rthread_h__
#define rthread_h__

#include "rbase.h"
typedef void (*rthread_start)(void *);
#ifdef _MSC_VER
#include <windows.h>
#include <process.h>
typedef uint rthread_ret;//32 bit
#define rthread_call __stdcall
typedef uint (__stdcall *prthread)(void *);
#else
#include <pthread.h>
typedef void *rthread_ret;
#define rthread_call
typedef void *(*prthread)(void *);
#endif

typedef rthread_ret (rthread_call  *thread_start2)(void*);

struct rthread
{
	static int create(thread_start2 start,void* param=null)
	{
		int tid;
#ifdef _MSC_VER
		tid=(int)_beginthreadex(null,0,start,param,0,null);
		if(0==tid||1==tid)
		{
			return 0;
		}
		else
		{
			return tid;
		}
#else
		if(0!=pthread_create((pthread_t*)&tid,null,start,param))
		{
			return 0;
		}
		else
		{
			return tid;
		}
#endif
	}

	static void wait_exit(int tid)
	{
#ifdef _MSC_VER
		WaitForSingleObject((HANDLE)tid,INFINITE);
#else
		pthread_join((pthread_t)tid,null);
#endif
	}
};

#endif
