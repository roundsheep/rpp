/*
裸机运行细胞自动机（生命游戏）
*/

import rbufm.h

main
{
	rbufm<char> arr(25,80)
	char* p=4194304
	for i=0 to 24
		for j=0 to 79
			arr[i,j]=(*p).toint%7
			p++
	char col=0
	for
		view(arr,col)
		delay
		trans(arr)
}

view(rbufm<char>& arr,char& col)
{
	for i=0 to 24
		for j=0 to 79
			if arr[i,j]==1
				rf.text(i*80+j,`*.tochar)//后面加上col可以变换颜色
			else
				rf.text(i*80+j,` .tochar)
			col++
}

//宏不能省略大括号
mac addc(a,b)
{
	if(a>=0&&a<25&&b>=0&&b<80)
	{
		if(arr[a,b]==1)
		{
			count++;
		}
	}
}

trans(rbufm<char>& arr)
{
	for i=0 to 24
		for j=0 to 79
			count=0
			addc(i-1,j-1)
			addc(i-1,j)
			addc(i-1,j+1)
			addc(i,j-1)
			addc(i,j+1)
			addc(i+1,j-1)
			addc(i+1,j)
			addc(i+1,j+1)
			if count==3
				arr[i,j]=1
			elif count==2
				;
			else
				arr[i,j]=0
}

delay
{
	for i=0 to 400_0000
		;
}