/*
惰性求值的方式实现短路求值
RPP是同时支持call_by_name和call_by_need和call_by_value的语言
该示例只能以JIT模式运行

目前call_by_name和call_by_need无法嵌套使用，比如下面的写法是错误的：
	if int.and(int.and(2>1),3!=4)
*/

import eval.h

void main()
{
	int* p=null
	//不能用 p!=null&&*p==2 因为 && 是非惰性的
	if int.and(p!=null,*p==2)
		putsl('and')
	
	rbuf<int> v
	//不能用 v.empty||v[0]!=5 因为 || 是非惰性的
	if int.or(v.empty,v[0]!=5)
		putsl('or')
	
	//int.and和int.or是惰性的，可以带多个参数
	if int.and(2>1,p==null,v.empty,3!=4)
		putsl('true')
}