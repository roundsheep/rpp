/*
RPP不支持重载小括号，
但是可以重载中括号实现C++的函数对象（仿函数）
*/

void main()
{
	A<int,int> a=lambda(int,int n){return n*n}
	puts a[3]
}

class A<T1,T2>
{
	void* m_func
	
	=(void* p)
	{
		m_func=p
	}
	
	T1 [](T2 n)
	{
		return T1[m_func,n]
	}
}