/*
组合遍历
*/

main
{
	n=7
	sum=0
	for i=0;i<n;i++
		for j=i+1;j<n;j++
			for k=j+1;k<n;k++
				sum++
	putsl(sum)

	putsl(A(3,6))
	putsl(A(6,6))
	putsl(C(3,7))
}

int A(int a,int b)
{
	temp=1
	for i=0;i<a;i++
		temp*=b-i
	return temp
}

int C(int a,int b)
{
	return A(a,b)/A(a,a)
} 