
#include "rstr.h"

struct rstrw
{
	rbuf<ushort> m_buf;

	~rstrw()
	{
	}

	rstrw()
	{
	}

	rstrw(const ushort* p)
	{
		set(p);
	}
	
	rstrw(const uchar* p,int len)
	{
		set_size(len);
		rf.memcpy(m_buf.begin(),p,len);
	}

	rstrw(const rstrw& s)
	{
		m_buf.alloc_not_change(s.m_buf.m_cmax);
		m_buf.m_count=s.count();
		rf.memcpy(begin(),s.begin(),s.size());
	}

	rstrw(const rstr& s)
	{
		rstr tmp=rcode::utf8_to_utf16(s);
		set_size(tmp.size());
		rf.memcpy(begin(),tmp.begin(),tmp.size());
	}

	rstr torstr()
	{
		return rcode::utf16_to_utf8(rstr(begin(),size()));
	}

	ushort* cstrw()
	{
		m_buf.push(0.toushort);
		m_buf.pop();
		return begin();
	}

	void operator=(const rstrw& a)
	{
		m_buf=a.m_buf;
	}

	ushort& operator[](int num) const 
	{
		return m_buf[num];
	}

	friend rbool operator==(const rstrw& a,const rstrw& b)
	{
		return a.m_buf==b.m_buf;
	}

	friend rbool operator!=(const rstrw& a,const rstrw& b)
	{
		return a.m_buf!=b.m_buf;
	}

	friend rstrw operator+(const rstrw& a,const rstrw& b)
	{
		rstrw ret;
		ret.m_buf=a.m_buf+b.m_buf;
		return ret;
	}

	void operator+=(const rstrw& a)
	{
		m_buf+=a.m_buf;
	}

	void operator+=(ushort ch)
	{
		m_buf+=ch;
	}

	friend rbool operator<(const rstrw& a,const rstrw& b)
	{
		for(int i=0;i<a.count()&&i<b.count();i++)
			if(a[i]!=b[i])
				return a[i]<b[i];
		return a.count()<b.count();
	}

	friend rbool operator>(const rstrw& a,const rstrw& b)
	{
		return b<a;
	}

	friend rbool operator<=(const rstrw& a,const rstrw& b)
	{
		return !(a>b);
	}

	friend rbool operator>=(const rstrw& a,const rstrw& b)
	{
		return !(a<b);
	}

	ushort* set_count(int count)
	{
		m_buf.realloc_n(count);
		return begin();
	}

	ushort* set_size(int size)
	{
		return set_count(size/2);
	}

	void set(const ushort* p)
	{
		int len=cdecl["lstrlenW",p];
		m_buf.realloc_n_not_change(rbuf<ushort>::extend_num(len));
		m_buf.m_count=len;
		rf.memcpy(m_buf.begin(),p,len*2);
	}

	void push(ushort ch)
	{
		m_buf.push(ch);
	}

	void push_front(ushort ch)
	{
		m_buf.push_front(ch);
	}

	ushort pop()
	{
		return m_buf.pop();
	}

	ushort pop_front()
	{
		return m_buf.pop_front();
	}

	int count() const 
	{
		return m_buf.count();
	}

	int size() const
	{
		return count()*2;
	}

	ushort* begin() const 
	{
		return m_buf.begin();
	}

	ushort* end() const 
	{
		return m_buf.end();
	}

	rbool empty() const 
	{
		return m_buf.empty();
	}

	void clear()
	{
		m_buf.clear();
	}

	static ushort* next(ushort* p)
	{
		return p+1;
	}

	static ushort* prev(ushort* p)
	{
		return p-1;
	}

	ushort* pos(int num) const
	{
		return begin()+num;
	}

	int pos(ushort* p) const
	{
		return p-begin();
	}

	rstrw sub(int begin) const
	{
		return sub(begin,count());
	}

	rstrw sub(int begin,int end) const
	{
		rstrw ret;
		ret.m_buf=m_buf.sub(begin,end);
		return ret;
	}

	rbool erase(int begin,int end)
	{
		return m_buf.erase(begin,end);
	}

	rbool erase(int i)
	{
		return erase(i,i+1);
	}

	rbool insert(int pos,const rstrw& a)
	{
		return m_buf.insert(pos,a.m_buf);
	}

	ushort get(int i) const
	{
		if(i>=count())
			return 0.toushort;
		else 
			return m_buf[i];
	}

	ushort get_top() const
	{
		ushort ret=0;
		if(count()>0)
		{
			ret=m_buf.m_p[count()-1];
		}
		return ret;
	}

	ushort get_bottom() const
	{
		ushort ret=0;
		if(count()>0)
		{
			ret=m_buf.m_p[0];
		}
		return ret;
	}

	int find(rstrw s,int begin=0)
	{
		return r_find_pos<rstrw>(*this,s,begin);
	}

	int find_last(ushort ch)
	{
		return m_buf.find_last(ch);
	}
	
	printl()
	{
		this.torstr.printl
	}
};
