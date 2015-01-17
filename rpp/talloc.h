#ifndef talloc_h__
#define talloc_h__

#include "../rlib/rstr.h"

struct talloc_i
{
	uchar* start;
	int commit_len;
	int max_len;
};

struct talloc
{
	rbuf<talloc_i> m_list;

	enum
	{
		c_page_size=4096
	};

	~talloc()
	{
#ifdef _MSC_VER
		for(int i=0;i<m_list.count();i++)
		{
			VirtualFree(m_list[i].start,0,MEM_RELEASE);
		}
#endif
	}

	uchar* alloc(int len)
	{
#ifdef _MSC_VER
		if(len<=0)
		{
			return null;
		}
		if(!m_list.empty())
		{
			talloc_i& top=m_list.top();
			if(top.commit_len+len<=top.max_len)
			{
				uchar* ret=top.start+top.commit_len;
				top.commit_len+=r_ceil_div(len,4)*4;
				return ret;
			}
		}
		talloc_i item;
		item.max_len=r_ceil_div(len,c_page_size)*c_page_size;
		item.commit_len=0;
		item.start=(uchar*)VirtualAlloc(null,item.max_len,
			MEM_COMMIT,PAGE_EXECUTE_READWRITE);
		if(item.start==null)
		{
			return null;
		}
		m_list.push(item);
		return alloc(len);
#else
		return null;
#endif
	}

	static uchar* alloc_v(int len)
	{
#ifdef _MSC_VER
		return (uchar*)VirtualAlloc(null,len,
			MEM_COMMIT,PAGE_EXECUTE_READWRITE);
#endif
	}

	static void free_v(uchar* addr)
	{
#ifdef _MSC_VER
		VirtualFree(addr,0,MEM_RELEASE);
#endif
	}
};

#endif
