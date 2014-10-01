/*
编译生成exe并运行
*/

import rfile.h
import rdir.h

main
{
	v=rf.get_param
	if v.count<3
		putsl('invaild param')
		return
	a=v[2]
	if rf.cmd('rpp.exe -win '+a)==0
		putsl('compile error')
		return
	b=rdir.get_real_name(a)
	if rf.cmd('nasm\\nasm.exe -f win32 '+b+'.asm -o '+b+'.obj')==0
		clear(b)
		putsl('asm error')
		return
	cmd='nasm\\GoLink.exe /console /mix /entry _main '
	cmd+=b+'.obj msvcr100.dll ws2_32.dll kernel32.dll user32.dll'
	if rf.cmd(cmd)==0
		clear(b)
		putsl('link error')
		return
	clear(b)
	if v.count==3
		rf.cmd(b+'.exe')
	elif v.count==4&&v[3]=='-build'
		;
	elif v.count==4&&v[3]=='-del'
		rf.cmd(b+'.exe')
		rfile.remove(rcode.gbk_to_utf8(b)+'.exe')
	elif v.count==4&&v[3]=='-check'
		rfile.remove(rcode.gbk_to_utf8(b)+'.exe')
}

clear(rstr b)
{
	b=rcode.gbk_to_utf8(b)
	rfile.remove(b+'.asm')
	rfile.remove(b+'.obj')
}