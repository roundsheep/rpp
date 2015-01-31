/*
惰性求值的方式实现短路求值
RPP是同时支持call_by_name和call_by_need和call_by_value的语言
该示例只能以JIT模式运行
*/

import eval.h

void main()
{
	int* p=null

	//不能用 p!=null&&*p==2 因为 && 是非惰性的
	if and(p!=null,*p==2)
		putsl('and')
	
	rbuf<int> v

	//不能用 v.empty||v[0]!=5 因为 || 是非惰性的
	if or(v.empty,v[0]!=5)
		putsl('or')
	
	//and和or是惰性的，可以带多个参数
	if and(2>1,p==null,v.empty,3!=4)
		putsl('true')

	if and(and(2>1),3!=4)
		putsl('true')

	a=1
	b=2
	c=3

	putsl and(and(a,b),c)
	putsl or(or(a,b),c)

	a=0
	b=1
	c=0

	putsl and(and(a,b),c)
	putsl or(or(a,b),c)
	putsl or(and(a,b),c)
	putsl and(or(a,b),c)
}