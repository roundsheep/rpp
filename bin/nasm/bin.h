
import rfile.h

main
{
	v=rf.get_param
	if v.count<3
		putsl('invaild param')
		return
	a=v[2]
	if rf.cmd('..\\rpp.exe -grub '+a)==0
		putsl('compile error')
		return
	b=a.sub(0,a.count-2)
	if rf.cmd('nasm.exe '+b+'.asm -o '+b+'.bin')==0
		putsl('asm error')
	rfile.remove(rcode.gbk_to_utf8(b)+'.asm')
}