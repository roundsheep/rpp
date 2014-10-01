/*
60行RPP实现的lisp解释器
未处理lambda、上下文等，有待完善
*/

void main()
{
	for
		putsl(eval(getsl))
}

rstr eval(rstr s)
{
	return eval(tokenize(s).split(' '))
}

rstr eval(rbuf<rstr>& v)
{
	if v.get(0)!='('
		return v.get(0)
	next=find(v.sub(2))+2
	switch v.get(1)
	case '+'
		return eval(v.sub(2,next)).toint+eval(v.sub(next,v.count-1)).toint
	case '-'
		return eval(v.sub(2,next)).toint-eval(v.sub(next,v.count-1)).toint
	case '*'
		return eval(v.sub(2,next)).toint*eval(v.sub(next,v.count-1)).toint
	case '/'
		return eval(v.sub(2,next)).toint/eval(v.sub(next,v.count-1)).toint
	return ''
}

int find(rbuf<rstr>& v)
{
	return r_cond(v.get(0)=='(',find_sbk(v)+1,1)
}

int find_sbk(rbuf<rstr>& v)
{
	count=0
	for i in v
		if '('==v[i]
			++count
		if ')'==v[i]
			--count
		if 0==count
			return i
	return v.count
}

rstr tokenize(rstr s)
{
	rstr ret
	for i in s
		if s[i]==`(
			ret+=' ( '
		elif s[i]==`)
			ret+=' ) '
		elif s[i]==0xa||s[i]==0xd||s[i]==9
			ret+=' '
		else
			ret+=s[i]
	return ret
}