class rstr
{
public:
	rbuf<char> m_buf;

	~rstr()
	{
	}

	rstr()
	{
	}

	rstr(const char* p)
	{
		set(p);
	}

	rstr(void* p,int len)
	{
		m_buf.alloc(len);
		rf.memcpy(begin(),p,len);
	}

	rstr(const char* p,const char* q)
	{
		if(p>=q)
		{
			rf::error("");
		}
		int len=q-p;
		m_buf.alloc(len);
		rf.memcpy(begin(),p,len);
	}

	rstr(char ch)
	{
		m_buf+=ch;
	}

	rstr(int i)
	{
		rbuf<char> tmp(20);
		rf.sprintf(tmp.begin,"%d",i);
		set(tmp.m_p);
	}

	rstr(uint i)
	{
		rbuf<char> tmp(20);
		rf.sprintf(tmp.begin,"%u",i);
		set(tmp.m_p);
	}

	rstr(int8 i)
	{
		rbuf<char> tmp(128);
		rf.sprintf8(tmp.begin,"%I64d",i);
		set(tmp.m_p);
	}
	
	rstr(double i)
	{
		rbuf<char> tmp(1024);
		rf.sprintf8(tmp.begin,"%f",i);
		set(tmp.m_p);
	}

	rstr(float i)
	{
		//sprintf用%f时以8字节打印
		set(rstr(double(i)).cstr)
	}

	rstr(rstr& s)
	{
		m_buf.alloc_not_change(s.m_buf.m_cmax);
		m_buf.m_count=s.count();
		rf::memcpy(begin(),s.begin(),s.count());
	}

	int toint()
	{
		int ret;
		rf.sscanf(cstr(),"%d",&ret);
		return ret;
	}

	uint touint()
	{
		uint ret;
		rf.sscanf(cstr(),"%u",&ret);
		return ret;
	}

	int8 toint8()
	{
		int8 ret;
		rf.sscanf(cstr(),"%I64d",&ret);
		return ret;
	}
	
	double todouble()
	{
		double ret;
		rf.sscanf8(cstr(),"%lf",&ret);
		return ret;
	}
	
	rstr torstr()
	{
		return this
	}
	
	rstrw torstrw()
	{
		rstr temp=rcode::utf8_to_utf16(this);
		s_ret.rstrw()
		s_ret.set_size(temp.size)
		rf.memcpy(s_ret.begin,temp.begin,temp.size);
	}

	void operator=(rstr& a)
	{
		m_buf=a.m_buf;
	}

	char& operator[](int num)
	{
		return m_buf[num];
	}

	friend bool operator==(rstr& a,rstr& b)
	{
		return a.m_buf==b.m_buf;
	}

	friend bool operator!=(rstr& a,rstr& b)
	{
		return a.m_buf!=b.m_buf;
	}

	friend rstr operator+(rstr& a,rstr& b)
	{
		int total=a.count()+b.count();
		int max=rf::max(a.m_buf.m_cmax,b.m_buf.m_cmax);
		rstr ret;
		if(total>max)
		{
			ret.m_buf.alloc_not_change(rbuf<char>.extend_num(total));
		}
		else
		{
			ret.m_buf.alloc_not_change(max);
		}
		ret.m_buf.m_count=total;
		rf::memcpy(ret.begin(),a.begin(),a.m_buf.m_count);
		rf::memcpy(ret.begin()+a.m_buf.m_count,b.begin(),b.m_buf.m_count);
		return ret;
	}

	void operator+=(rstr& a)
	{
		m_buf+=a.m_buf;
	}

	friend bool operator<(rstr& a,rstr& b)
	{
		return rf::strcmp(a.cstr(),b.cstr())<0;
	}

	friend bool operator>(rstr& a,rstr& b)
	{
		return rf::strcmp(a.cstr(),b.cstr())>0;
	}

	friend bool operator<=(rstr& a,rstr& b)
	{
		return rf::strcmp(a.cstr(),b.cstr())<=0;
	}

	friend bool operator>=(rstr& a,rstr& b)
	{
		return rf::strcmp(a.cstr(),b.cstr())>=0;
	}

	char* set_size(int size)
	{
		m_buf.realloc_n(size);
		return begin();
	}

	void set(char* p)
	{
		int len=rf::strlen(p);
		m_buf.realloc_n_not_change(rbuf<char>.extend_num(len));
		m_buf.m_count=len;
		rf::memcpy(m_buf.begin(),p,len);
	}

	void push(char ch)
	{
		m_buf.push(ch);
	}

	char pop()
	{
		return m_buf.pop();
	}

	int count()
	{
		return m_buf.count();
	}

	int size()
	{
		return m_buf.count();
	}

	char* cstr()
	{
		char temp=0;
		m_buf.push(temp);
		m_buf.pop();
		return m_buf.begin();
	}

	wchar* cstrw()
	{
		char temp=0;
		m_buf.push(temp);
		m_buf.push(temp);
		m_buf.pop();
		m_buf.pop();
		return m_buf.begin();
	}

	char* begin()
	{
		return m_buf.begin();
	}

	char* end()
	{
		return m_buf.end();
	}

	bool empty()
	{
		return m_buf.empty();
	}

	void clear()
	{
		m_buf.clear();
	}

	char* pos(int num)
	{
		return begin()+num;
	}

	int pos(char* p)
	{
		return p-begin();
	}

	rstr sub(int begin,int end=count)
	{
		rstr ret;
		ret.m_buf=m_buf.sub(begin,end);
		return ret;
	}

	bool erase(int begin,int end)
	{
		return m_buf.erase(begin,end);
	}

	bool erase(int i)
	{
		return erase(i,i+1);
	}

	bool insert(int pos,rstr& a)
	{
		return m_buf.insert(pos,a.m_buf);
	}

	char get(int i)
	{
		return m_buf.get(i);
	}

	char get_top()
	{
		char ret=0;
		if(count()>0)
		{
			ret=m_buf.m_p[count()-1];
		}
		return ret;
	}

	char get_bottom()
	{
		char ret=0;
		if(count()>0)
		{
			ret=m_buf.m_p[0];
		}
		return ret;
	}

	bool is_number()
	{
		if(empty())
		{
			return false;
		}
		for(int i=0;i<count();i++)
		{
			if(!rf::is_number(m_buf[i]))
			{
				return false;
			}
		}
		return true;
	}
	
	//查找子串
	int find(rstr& m,int begin=0)
	{
		return r_find_pos<rstr>(this,m,begin)
	}
	
	rstr replace(rstr src,rstr dst)
	{
		rstr ret
		for i=0;i<count;i++
			if this.sub(i,i+src.count)==src
				ret+=dst
				i=i+src.count-1
			else
				ret+=this[i]
		return ret
	}

	rbuf<rstr> split(rstr m)
	{
		return r_split_r<rstr>(this,m)
	}
	
	void reverse()
	{
		m_buf.reverse
	}

	void print()
	{
		rf.print(this)
	}

	void printl()
	{
		rf.printl(this)
	}
	
	static rstr format(rstr f,int a)
	{
		rbuf<char> temp(1024)
		rf.sprintf(temp.begin,f.cstr,a);
		s_ret.rstr()
		s_ret.set(temp.begin)
	}
};