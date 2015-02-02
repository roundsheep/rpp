/*
下面是编译期计算示例，sum是一个宏，计算从0加到N
*/

import rpp.h

void main()
{
	putsl sum(100)//输出5050
	putsl sum(10)//输出55
}

macro void sum(rbuf<tword>& v,int index)
{
	result=0
	param=v.get(index+2).val.toint
	for i=0;i<=param;i++
		result+=i
	v[index].val=result
	for i=1;i<=3;i++
		v[index+i].val.clear
}