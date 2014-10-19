#ifndef rstr_h__
#define rstr_h__

#include "rbuf.h"
#include "ralgo.h"
#ifdef _MSC_VER
#include <conio.h>
#else
#include <stdarg.h>
#endif

struct rstr
{
	rbuf<uchar> m_buf;

	~rstr()
	{
	}

	rstr()
	{
	}

	rstr(const void* p)
	{
		set(p);
	}

	rstr(void* p,int len)
	{
		m_buf.alloc(len);
		memcpy(begin(),p,len);
	}

	rstr(const void* p,const void* q)
	{
		/*if(p>=q)
		{
			error("");
		}*/
		int len=(int)((uchar*)q-(uchar*)p);
		m_buf.alloc(len);
		memcpy(begin(),p,len);
	}

	rstr(int i)
	{
		char tmp[20];
		sprintf(tmp,"%d",i);
		set(tmp);
	}

	rstr(uint i)
	{
		char tmp[20];
		sprintf(tmp,"%u",i);
		set(tmp);
	}

	rstr(double i)
	{
		char tmp[128];
		sprintf(tmp,"%f",i);
		set(tmp);
	}

	rstr(int8 i)
	{
		char tmp[100];
#ifdef _MSC_VER
		sprintf(tmp,"%I64d",i);
#else
		sprintf(tmp,"%lld",i);
#endif
		set(tmp);
	}

	rstr(const rstr& s)
	{
		m_buf.alloc_not_change(s.m_buf.m_cmax);
		m_buf.m_count=s.count();
		memcpy(begin(),s.begin(),s.count());
	}

	rstr(rstr&& s)
	{
		m_buf.move(s.m_buf);
	}

	int toint()
	{
		int result;
		sscanf(cstr_t(),"%d",&result);
		return result;
	}

	uint touint()
	{
		uint result;
		sscanf(cstr_t(),"%u",&result);
		return result;
	}

	double todouble()
	{
		double result;
		sscanf(cstr_t(),"%lf",&result);
		return result;
	}

	int8 toint8()
	{
		int64 result;
#ifdef _MSC_VER
		sscanf(cstr_t(),"%I64d",&result);
#else
		sscanf(cstr_t(),"%lld",&result);
#endif
		return result;
	}

	rstr torstr()
	{
		return *this;
	}

	uchar* cstr()
	{
		char temp=0;
		push(temp);
		pop();
		return begin();
	}

	char* cstr_t()
	{
		return (char*)cstr();
	}

	ushort* cstrw()
	{
		char temp=0;
		push(temp);
		push(temp);
		pop();
		pop();
		return (ushort*)begin();
	}

	wchar* cstrw_t()
	{
		return (wchar*)cstrw();
	}
	
	//赋值函数千万不要让编译器自动生成
	void operator=(const rstr& s)
	{
		m_buf.realloc_n_not_change(s.m_buf.m_cmax);
		m_buf.m_count=s.count();
		memcpy(begin(),s.begin(),s.count());
		//m_buf=a.m_buf;
	}

	void operator=(rstr&& s)
	{
		m_buf.free();
		m_buf.move(s.m_buf);
	}

	uchar& operator[](int num) const 
	{
		return m_buf[num];
	}

	friend rbool operator==(const rstr& a,const rstr& b)
	{
		return a.m_buf==b.m_buf;
	}

	friend rbool operator!=(const rstr& a,const rstr& b)
	{
		return a.m_buf!=b.m_buf;
	}

	//有待优化
	friend rstr operator+(const rstr& a,const rstr& b)
	{
		/*rstr ret;
		ret.m_buf=a.m_buf+b.m_buf;
		return ret;*/

		int total=a.count()+b.count();
		int max=r_max(a.m_buf.m_cmax,b.m_buf.m_cmax);
		rstr ret;
		if(total>max)
		{
			ret.m_buf.alloc_not_change(rbuf<char>::extend_num(total));
		}
		else
		{
			ret.m_buf.alloc_not_change(max);
		}
		ret.m_buf.m_count=total;
		memcpy(ret.begin(),a.begin(),a.m_buf.m_count);
		memcpy(ret.begin()+a.m_buf.m_count,b.begin(),b.m_buf.m_count);
		return r_move(ret);
	}

	void operator+=(const rstr& a)
	{
		m_buf+=a.m_buf;
	}

	void operator+=(uchar ch)
	{
		m_buf+=ch;
	}

	friend rbool operator<(const rstr& a,const rstr& b)
	{
		for(int i=0;i<a.count()&&i<b.count();i++)
			if(a[i]!=b[i])
				return a[i]<b[i];
		return a.count()<b.count();
	}

	friend rbool operator>(const rstr& a,const rstr& b)
	{
		return b<a;
	}

	friend rbool operator<=(const rstr& a,const rstr& b)
	{
		//return a<b||a==b;
		return !(a>b);
	}

	friend rbool operator>=(const rstr& a,const rstr& b)
	{
		//return a>b||a==b;
		return !(a<b);
	}

	uchar* set_size(int size)
	{
		m_buf.realloc_n(size);
		return begin();
	}

	void set(const void* p)
	{
		//todo 如果字符串长度为0没必要分配内存
		int len=(int)strlen((const char*)p);
		m_buf.realloc_n_not_change(rbuf<uchar>::extend_num(len));
		m_buf.m_count=len;
		memcpy(m_buf.begin(),p,len);
	}

	void push(uchar ch)
	{
		m_buf.push(ch);
	}

	void push_front(uchar ch)
	{
		m_buf.push_front(ch);
	}

	uchar pop()
	{
		return m_buf.pop();
	}

	uchar pop_front()
	{
		return m_buf.pop_front();
	}

	int count() const 
	{
		return m_buf.count();
	}

	int size() const
	{
		return count();
	}

	uchar* begin() const 
	{
		return m_buf.begin();
	}

	uchar* end() const 
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

	static uchar* next(void* p)
	{
		return ((uchar*)p)+1;
	}

	static uchar* prev(void* p)
	{
		return ((uchar*)p)-1;
	}

	uchar* pos(int num) const
	{
		return begin()+num;
	}

	int pos(void* p) const
	{
		return int((uchar*)p-begin());
	}

	rstr sub(int begin) const
	{
		return sub(begin,count());
	}

	rstr sub(int begin,int end) const
	{
		rstr ret;
		ret.m_buf=m_buf.sub(begin,end);
		return r_move(ret);
	}

	rbool erase(int begin,int end)
	{
		return m_buf.erase(begin,end);
	}

	rbool erase(int i)
	{
		return erase(i,i+1);
	}

	rbool insert(int pos,const rstr& a)
	{
		return m_buf.insert(pos,a.m_buf);
	}

	uchar get(int i) const
	{
		if(i>=count())
			return 0;
		else 
			return m_buf[i];
	}

	uchar get_top() const
	{
		uchar ret=0;
		if(count()>0)
		{
			ret=m_buf.m_p[count()-1];
		}
		return ret;
	}

	uchar get_bottom() const
	{
		uchar ret=0;
		if(count()>0)
		{
			ret=m_buf.m_p[0];
		}
		return ret;
	}

	rbool is_number() const
	{
		if(empty())
		{
			return false;
		}
		for(int i=0;i<count();i++)
		{
			if(!is_number(m_buf[i]))
			{
				return false;
			}
		}
		return true;
	}

	int find(const rstr& s,int begin=0) const
	{
		return r_find_pos(*this,s,begin);
	}

	int find_last(uchar ch) const
	{
		return m_buf.find_last(ch);
	}

	rbool exist(const rstr& s) const
	{
		return find(s)<count();
	}

	static rbool is_alpha(uchar ch)
	{
		return ((ch)>='a'&&(ch)<='z'||(ch)>='A'&&(ch)<='Z');
	}
	
	static rbool is_number(uchar ch)
	{
		return ((ch)>='0'&&(ch)<='9');
	}

	static uchar chartoup(uchar ch)
	{
		return ((ch)>='a'&&(ch)<='z'?(ch)-32:(ch));
	}
	
	static int upchartonum(uchar ch)
	{
		return ((ch)>='A'?(ch)-'A'+10:(ch)-'0');
	}

	static int chartonum(uchar ch)
	{
		return upchartonum(chartoup(ch));
	}

	void print()
	{
		printf("%s ",cstr());
	}

	void printl()
	{
		printf("%s\n",cstr());
	}

	void format_t(const char *szFormat,...)
	{
		char   szBuffer [1024] ;
		va_list pArgList ;
		va_start (pArgList, szFormat) ;
#ifdef _MSC_VER
		_vsnprintf (szBuffer, r_size (szBuffer),szFormat, pArgList) ;
#else
		vsnprintf (szBuffer, r_size (szBuffer),szFormat, pArgList) ;
#endif
		va_end (pArgList) ;
		set(szBuffer);
	}

	static rstr format(const char *szFormat,...)
	{
		char   szBuffer [1024] ;
		va_list pArgList ;
		va_start (pArgList, szFormat) ;
#ifdef _MSC_VER
		_vsnprintf (szBuffer, r_size (szBuffer),szFormat, pArgList) ;
#else
		vsnprintf (szBuffer, r_size (szBuffer),szFormat, pArgList) ;
#endif
		va_end (pArgList) ;
		return rstr(szBuffer);
	}

	static rstr hextodec(rstr s)
	{
		uint a;
		sscanf(s.cstr_t(),"%x",&a);
		return rstr(a);
	}

	static rstr bintodec(rstr s)
	{
		uint sum=0;
		uint pro=1;
		for(int i=s.count()-1;i>=0;i--)
		{
			if(s[i]=='1')
				sum+=pro;
			pro*=2;
		}
		return rstr(sum);
	}

	template<class T>
	static rstr join(const rbuf<T>& v,const rstr& s)
	{
		rstr ret;
		for(int i=0;i<v.count();i++)
		{
			if(i!=0)
			{
				ret+=s;
			}
			ret+=v[i].torstr();
		}
		return r_move(ret);
	}
};

#endif