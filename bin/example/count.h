/*
统计当前目录下代码行数
如果使用utf16统计更准确
*/

import rdir.h
import rfile.h

int g_sum=0

count_line(rstr name)
{
	s=rfile.read_all_n(name)
	sum=1
	for i=0;i<s.count;i++
		if s[i]==0xa
			sum++
	g_sum+=sum
	puts(name+': '+sum+' lines\n')
}

main
{
	path='./'
#ifdef _RVM
	if rf.get_param.count==3
		path=rf.get_param.top
#endif
		
	v=rdir.get_file_bfs(path)
	for i in v
		name=v[i]
		if name.sub(name.count-2)=='.h'
			count_line(name)
		elif name.sub(name.count-4)=='.cpp'
			count_line(name)
	puts('total: '+g_sum)
}