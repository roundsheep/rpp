/*
仅用位运算实现加减乘除，面试专用
*/

main
{
	fadd(1,2).printl
	fadd(19,29999).printl

	fsub(-1,3).printl
	fsub(19,2345).printl

	fmul(99,98).printl
	fmul(-29,235).printl

	fdiv(99,98).printl
	fdiv(9,3).printl
	fdiv(-9,3).printl
	fdiv(8,3).printl
}

int fadd(int x,int y)
{
	for
		a=AND(x,y)
		b=XOR(x,y)
		x=SHL(a,1)
		y=b
		ifn a
			break
	return b
}

int fneg(int x)
{
	return fadd(NOT(x),1)
}

int fsub(int x,int y)
{
	return fadd(x,fneg(y))
}

int fmul(int x,int y)
{
	m=1
	z=0
	if x<0
		x=fneg(x)
		y=fneg(y)
	for x>=m&&y
		if AND(x,m)
			z=fadd(y,z)
		y=SHL(y,1)
		m=SHL(m,1)
	return z
}

int fdiv(int x,int y)
{
	c=0
	sign=0
	if x<0
		x=fneg(x)
		sign=XOR(sign,1)
	if y<0
		y=fneg(y)
		sign=XOR(sign,1)
	if y
		for x>=y
			x=fsub(x,y)
			c=fadd(c,1)
	if sign
		c=fneg(c)
	return c
}