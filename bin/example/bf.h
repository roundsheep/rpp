/*
BF解释器增强版，支持32位运算
这个语言并不像看上去那么简单，下面是一个BF版的Hello World：
++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.
>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.
*/

void main()
{
	code=getsl
	rbuf<int> data(30000)
	rf.memset(data.begin,0,data.size)
	pd=0
	for pc=0;pc<code.count;pc++
		switch code[pc]
		case `>
			pd++
		case `<
			pd--
		case `+
			data[pd]++
		case `-
			data[pd]--
		case `.
			puts(rstr.format('%c',data[pd]))
		case `,
			data[pd]=rf.getch
		case `[
			ifn data[pd]
				pc=find< ++ >(pc,code,`[,`])
				continue//这里也可用continued
		case `]
			if data[pd]
				pc=find< -- >(pc,code,`],`[)
				continue
}

int find<T>(int pc,rstr& code,int left,int right)
{
	count=0
	for i=pc;;i T
		if code[i]==left
			count++
		elif code[i]==right
			count--
		if count==0
			return i
}