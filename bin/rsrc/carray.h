
class carray<T,N>
{
	T m_buf[N]
	
	carray<T,N>()
	{
	}
	
	carray<T,N>(carray<T,N>& v)
	{
		this=v
	}

	T& [](int num)
	{
		return (&m_buf)[num]
	}
	
	void =(carray<T,N>& v)
	{
		for i=0;i<N;i++
			this[i]=v[i]
	}
	
	void =(rbuf<T>& v)
	{
		for i=0;i<N;i++
			this[i]=v[i]
	}
	
	int count()
	{
		return N
	}
	
	T* begin()
	{
		return &m_buf
	}
}