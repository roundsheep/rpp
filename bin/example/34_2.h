//嵌套类和嵌套命名空间

namespace A
{
}

class A.B
{
	int m_b
	
	A.B()
	{
		putsl('constructor')
	}

	static void func()
	{
		putsl('func')
	}
}

namespace A.B.C
{
	void func()
	{
		putsl('func')
	}
}

void main()
{
	A.B b
	b.m_b=3
	putsl(b.m_b)
	A.B.func()
	A.B.C.func()
}