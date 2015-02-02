/*
RPP的macro具有和Lisp宏一样强大的表达能力
这种宏可以用写代码对代码进行变换，这就比C语言的宏强大得多
下面是变参宏示例
注意macro只能是全局的
*/

import rpp.h

void main()
{
	putsl sum(98,56)
	putsl sum(1,2,3,4)
	putsl sum(43,534,65,645,54,2)
}

macro void sum(rbuf<tword>& v,int index)
{
	left=index+1
	right=find_symm_sbk(v,left)
	result=0
	for i=index+2;i<right;i+=2
		result+=v[i].val.toint
	for i=index;i<=right;i++
		v[i].val.clear
	v[index].val=result
}