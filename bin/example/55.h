/*
栈排序与栈颠倒
*/

void main()
{
	rbuf<int> stack
	#stack.push(3,4,1,9,10,2)
	print(stack)
	func(stack)
	print(stack)
	reverse(stack)
	print(stack)
	sort(stack)
	print(stack)
}

void print(rbuf<int>& stack)
{
	for i=0 to stack.count-1
		putsl(stack[i])
	putsl
}

void sort(rbuf<int>& stack)
{
	if stack.empty
		return
	a=stack.pop
	if stack.empty
		stack.push(a)
		return
	sort(stack)
	b=stack.pop
	if a<b
		stack.push(a)
		sort(stack)
		stack.push(b)
	else
		stack.push(b)
		sort(stack)
		stack.push(a)
}

void func(rbuf<int>& stack)
{
	if stack.count<=1
		return
	a=stack.pop
	func(stack)
	b=stack.pop
	if a<b
		stack.push(a)
		func(stack)
		stack.push(b)
	else
		stack.push(b)
		stack.push(a)
}

void reverse(rbuf<int>& stack)
{
	if stack.count<=1
		return
	a=stack.pop
	reverse(stack)
	b=stack.pop
	reverse(stack)
	stack.push(a)
	reverse(stack)
	stack.push(b)
}