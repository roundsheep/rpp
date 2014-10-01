/*
KMP算法
*/

void main()
{
	putsl(find('abc123','c1'))
	putsl(find('abc123','c2'))
}

int find(rstr s,rstr p)
{
	next=get_next(p)
	i=0
	j=0
	for i<s.count&&j<p.count
		if j==-1||s[i]==p[j]
			i++
			j++
		else
			j=next[j]
	if j==p.count
		return i-j
	return -1
}

rbuf<int> get_next(rstr p)
{
	rbuf<int> next(p.count)
	next[0]=-1
	k=-1
	j=0
	for j<p.count-1
		if k==-1||p[j]==p[k]
			j++
			k++
			if p[j]!=p[k]
				next[j]=k
			else
				next[j]=next[k]
		else
			k=next[k]
	return next
}