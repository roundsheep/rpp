#ifndef rdic_h__
#define rdic_h__

#include "rset.h"

template<typename T>
struct rdic_i
{
	T val;//val必须前置
	rstr key;

	friend rbool operator<(const rdic_i<T>& a,const rdic_i<T>& b)
	{
		return a.key<b.key;
	}
};

template<typename T>
struct rdic
{
	rset<rdic_i<T> > m_set;

	void clear()
	{
		m_set.clear();
	}

	T* begin() const
	{
		return (T*)m_set.begin();
	}

	T* end() const
	{
		return (T*)m_set.end();
	}

	T* next(T* p) const
	{
		return (T*)m_set.next((rdic_i<T>*)p);
	}

	rstr get_key(T* p)
	{
		return ((rdic_i<T>*)p)->key;
	}

	rbool empty() const
	{
		return m_set.empty();
	}

	int count() const
	{
		return m_set.count();
	}

	rbool exist(const rstr& key) const
	{
		rdic_i<T> item;
		item.key=key;
		return m_set.exist(item);
	}

	T* find(const rstr& key) const
	{
		rdic_i<T> item;
		item.key=key;
		return (T*)m_set.find(item);
	}

	void insert(const rstr& key,const T& val)
	{
		rdic_i<T> item;
		item.key=key;
		item.val=val;
		m_set.insert(item);
	}

	T& operator[](const rstr& key)
	{
		rdic_i<T> item;
		item.key=key;
		m_set.insert_c(item);
		rdic_i<T>* p=m_set.find(item);
		return p->val;
	}
};

#endif
