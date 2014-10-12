#ifndef rhash_h__
#define rhash_h__

#include "rstr.h"

template<typename T>
struct rhash_i
{
	T val;
	rstr name;
	rhash_i* pre;
	rhash_i* next;
};

template<typename T>
struct rhash
{
	rbuf<rhash_i<T>*> m_hash;
	int m_count;

	~rhash()
	{
		clear();
	}

	rhash()
	{
		init();
	}

	T& operator[](const rstr& name)
	{
		T* p=(T*)find(name);
		if (p)
			return *p;
		//找不到则插入一个空值
		T a;
		insert(a,name);
		p=(T*)find(name);
		return *p;
	}

	void init()
	{
		//两个字节消耗256K内存，如果用3个字节需要64M内存
		m_hash.realloc_n(65536);
		for (int i=0;i<m_hash.count();i++)
			m_hash[i]=null;
		m_count=0;
	}

	void clear()
	{
		if (m_count==0)
			return;
		for (int i=0;i<m_hash.count();i++)
		{
			if (m_hash[i]!=null)
			{
				rhash_i<T>* p=m_hash[i];
				while (p!=null)
				{
					rhash_i<T>* temp=p;
					p=p->next;
					delete temp;
				}
			}
		}
		init();
	}

	rbool exist(const rstr& name)
	{
		return find(name)!=null;
	}

	T* find(const rstr& name)
	{
		int addr=get_addr(name);
		rhash_i<T>* p=m_hash[addr];
		while (p)
		{
			if (p->name==name)
				return (T*)p;
			p=p->next;
		}
		return null;
	}
	
	//如果重复则不插入
	void insert_c(T& a,rstr name="")
	{
		if (exist(name))
			return;
		insert(a,name);
	}

	void insert(T& a,rstr name="")
	{
		int addr=get_addr(name);
		rhash_i<T>* p=new rhash_i<T>;
		p->name=name;
		p->next=m_hash[addr];
		p->pre=null;
		p->val=a;
		if (m_hash[addr]!=null)
			m_hash[addr]->pre=p;
		m_hash[addr]=p;
		m_count++;
	}

	rbool erase(rstr name="")
	{
		rhash_i<T>* p=(rhash_i<T>*)find(name);
		if (p==null)
			return false;
		if (p->pre!=null)
		{
			p->pre->next=p->next;
			if (p->next!=null)
				p->next->pre=p->pre;
		}
		else
		{
			m_hash[get_addr(name)]=p->next;
			if (p->next!=null)
				p->next->pre=null;
		}
		delete p;
		m_count--;
		return true;
	}

	T* begin()
	{
		for (int i=0;i<m_hash.count();i++)
			if (m_hash[i]!=null)
				return (T*)m_hash[i];
		return null;
	}

	T* rend()
	{
		for (int i=m_hash.count()-1;i>=0;i--)
			if (m_hash[i]!=null)
			{
				rhash_i<T>* p=m_hash[i];
				while (p!=null&&p->next!=null)
					p=p->next;
				return (T*)p;
			}
		return null;
	}
	
	//这个next有点慢，能不能把一个桶的最后一个元素的next指向下一个桶？
	T* next(T* cur)
	{
		rhash_i<T>* p=(rhash_i<T>*)cur;
		if (p->next!=null)
			return (T*)p->next;
		int addr=get_addr(p->name);
		for (int i=addr+1;i<m_hash.count();i++)
			if (m_hash[i]!=null)
				return (T*)m_hash[i];
		return null;
	}

	static int get_addr(const rstr& name)
	{
		return (int)bkdr_hash16(name.begin(),name.count());
	}

	static int bkdr_hash32(void* data,int len)
	{
		uint seed=131; // 31 131 1313 13131 131313 etc..
		uint hash=0;
		for (int i=0;i<len;++i)
		{
			hash=(hash*seed)+((uchar*)data)[i];
		}
		return (int)hash;
	}

	static ushort bkdr_hash16(void* data,int len)
	{
		int val=bkdr_hash32(data,len);
		return val%65536;
	}
};

#endif