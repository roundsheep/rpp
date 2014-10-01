import rdic.h

main
{
	rdic<int> a
	a['abc']=3
	a['121213']=4
	a['1']=1
	for i=1 to 1000
		a[i.torstr]=i
	a['121213'].printl
}