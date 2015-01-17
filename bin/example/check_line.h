/*
检查代码每行是否超过88字符
rpp -jit example\check_line.h ../rpp/
*/

import rdir.h
import rfile.h

bool check_line(rstrw s)
{
	count=0
	for i=0;i<s.count;i++
		temp=s.sub(i,i+1).torstr.get(0)
		if rcode.is_utf8_3(temp)||rcode.is_utf8_2(temp)
			count+=2
		elif s[i]==r_char('	').toint
			count+=8
		else
			count++
	if count<=88
		return true
	putsl 'count '+count
	putsl(s.torstr)
	return false
}

bool check_s(rstrw s)
{
	rstrw temp
	for i=0;i<s.count;i++
		if s[i]==10&&s.get(i-1)!=13
			return false//temp+=13.toushort
		if s[i]==13&&s.get(i+1)!=10
			return false
		temp+=s[i]
	v=r_split_e_r<rstrw>(temp,'\r\n')
	for i in v
		ifn check_line(v[i])
			putsl('line '+(i+1))
			return false
	return true
}

bool check_file(rstr name)
{
	rfile file(name)
	rbuf<char> buf(3)
	ifn(file.read(3,buf.begin))
		return false
	ok=false
	if(buf[0]==0xff&&buf[1]==0xfe)
		s=rfile.read_all_n(name).sub(2)
		ok=check_s(rstrw(s.begin,s.count))
	elif(buf[0]==0xef&&buf[1]==0xbb&&buf[2]==0xbf)
		s=rfile.read_all_n(name).sub(3)
		ok=check_s(rstrw(s))
	if ok
		return true
	putsl(name)
	return false
}

void main()
{
	if rf.get_param.count<3
		return
	v=rdir.get_file_bfs(rf.get_param.top)
	for i in v
		name=v[i]
		if name.sub(name.count-2)=='.h'
			ifn check_file(name)
				break
		elif name.sub(name.count-4)=='.cpp'
			ifn check_file(name)
				break
}