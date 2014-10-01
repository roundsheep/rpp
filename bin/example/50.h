
main
{
	test1
	test2
	test3
	test4
	test5
}

//浮点测试
test1
{
	putsl 0.2+0.3
	putsl 0.2*0.3
	putsl 0.2/0.3
	putsl 0.2-0.3
	putsl 0.2<0.3
	putsl 0.2>0.3
	a=99.2*20
	putsl a
	putsl(-2)
}

//逻辑运算测试
test2
{
	putsl 0&&0
	putsl 1&&0
	putsl 0&&1
	putsl 1&&1
	putsl 0||0
	putsl 1||0
	putsl 0||1
	putsl 1||1
}

//自动类型转换测试
test3
{
	uint a=1
	int c=5
	putsl a+sizeof(int)!=c
}

//64位运算测试
test4
{
	int8 a=9
	int8 b=3
	putsl a/b
	putsl a*b
	putsl a%b
	putsl a-b
	putsl a<b
	putsl a>b
}

test5
{
	if (2)==2
		putsl('ok',,,)
}