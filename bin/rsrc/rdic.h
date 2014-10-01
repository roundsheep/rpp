import 'rset.h'

template<typename T>
struct rdic_i
{
	T val;
	rstr key;

	friend rbool operator<(const rdic_i<T>& a,const rdic_i<T>& b)
	{
		return a.key<b.key;
	}

	friend rbool operator!=(const rdic_i<T>& a,const rdic_i<T>& b)
	{
		return a.key!=b.key;
	}
};

template<typename T>
struct rdic
{
	rset<rdic_i<T> > m_set;

	rbool exist(const rstr& key)
	{
		rdic_i<T> item;
		item.key=key;
		return m_set.exist(item);
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