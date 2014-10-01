/*
检查当前目录下文本文件的编码类型
*/

import "rdir.h"

main
{
	v=rdir.get_file_bfs('./')
	for i in v
	{
		if v[i].get_top==`/
			continue
		rfile file(v[i])
		rbuf<char> buf(3)
		ifn(file.read(3,buf.begin))
			break
		s=v[i]+' '
		if(buf[0]==0xff&&buf[1]==0xfe)
			s+='utf16'
		elif(buf[0]==0xef&&buf[1]==0xbb&&buf[2]==0xbf)
			s+='utf8'
		else
			s+='gbk'
		s.printl
	}
}