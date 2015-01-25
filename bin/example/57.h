/*
4个线程并行计算1个数组的5个元素
*/

void main()
{
	v=array<int>[1,2,3,4,5]
	v.join(' ').printl
	v.parallel(4,lambda(int& a){
		a*=3
	})
	v.join(' ').printl
}