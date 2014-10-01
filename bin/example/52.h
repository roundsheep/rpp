/*
全排列的非递归实现，支持去掉重复
*/

void main()
{
	rbuf<int> v
	#v.push(1,2,3,4)
	for
		v.join('').printl
		ifn next_permutation<int>(v)
			break
}

bool next_permutation<T>(rbuf<T>& v)
{
	if v.count<=1
		return false
	next=v.count-1
	for
		temp=next
		next--
		if v[next]<v[temp]
			mid=v.count-1
			for !(v[next]<v[mid])
				mid--
			r_swap<T>(v[next],v[mid])
			r_reverse<T>(v,temp)
			return true
		if next==0
			r_reverse<T>(v,0)
			return false
}