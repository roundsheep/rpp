/*
文件搜索工具
*/

import rfile.h
import rdir.h

void main()
{
	dir=\\./
	arr=array<rstr>['cpp','h']
	str='s_ret'
	//todo utf16转换
	v=rdir.get_file_bfs(dir)
	for i in v
		name=rdir.get_name(v[i])
		suffix=rdir.get_suffix(name)
		if(v[i].get_top()==`/)
			continue
		ifn arr.exist(suffix)
			continue
		s=rfile.read_all_n(v[i])
		if s.find(str)<s.count
			putsl(v[i])
}