/*
可以发挥想象力定制奇特的宏，
比如宏可以引用整个函数的单词并进行替换，
也可以定制自己的控制结构
*/

import rpp.h

void main()
{
	5 times putsl('hello');
	sum=0;
	10 times sum+=2;
	putsl sum;
}

macro void times(rbuf<tword>& v,int index)
{
	count=v[index-1].val.toint
	for i=index;i<v.count;i++
		if v[i].val==';'
			break
	end=i
	for i=0;i<count;i++
		v[index].multi+=vword_to_vstr(v.sub(index+1,end+1))
	for i=index-1;i<=end;i++
		v[i].val.clear
}