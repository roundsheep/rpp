/*
分别以解释方式和编译方式运行example下所有例子，
并与标准答案进行对比，以此来检验编译器的正确性。
*/

import rdir.h
import rfile.h

rstr g_sta
int g_total

main
{
	g_sta='ok'
	start=rf.tick
	g_total=0
	v=rdir.get_file_bfs('./example/')
	for i in v
		name=rdir.get_name(v[i])
		suffix=rdir.get_suffix(name)
		if suffix!='h'
			continue
		a=name.sub(0,name.count-2)
		
		ifn a.sub(0,1).is_number
			continue
		if a=='14_2'||a=='14_3'||a=='14_4'||a=='22_2'||a=='36_8'||a=='36_9'
			continue
		if a.sub(0,3)=='16_'
			continue
		if a.sub(0,3)=='40_'
			continue
		
		if a!='21_2'
			rf.cmd('rpp example/'+a+'.h > example/answer/'+a+'_tmp.txt')
			check_interpret('example/answer/'+a+'_tmp.txt',
				'example/answer/'+a+'.txt')
		
		if a=='14_1'||a=='14_5'||a=='14_6'
			continue

		rf.cmd('rjit example\\'+a+'.h > example/answer/'+a+'_tmp.txt')
		check_jit('example/answer/'+a+'_tmp.txt','example/answer/'+a+'.txt')

		rf.cmd('rpack example\\'+a+'.h')
		rf.cmd('example\\'+a+'.exe > example/answer/'+a+'_tmp.txt')
		check_pack('example/answer/'+a+'_tmp.txt','example/answer/'+a+'.txt')
		rfile.remove('example\\'+a+'.exe')

		if a=='13'||a=='22_1'
			continue
		
		rf.cmd('run example\\'+a+'.h > example/answer/'+a+'_tmp.txt')
		check_exe('example/answer/'+a+'_tmp.txt','example/answer/'+a+'.txt')
	putsl
	putsl('total: '+g_total)
	putsl('time: '+(rf.tick-start))
	putsl('state: '+g_sta)
	rf.getch
}

check_interpret(rstr a,rstr b)
{
	g_total++
	cont=rfile.read_all_n(a);
	if cont.count>0&&cont==rfile.read_all_n(b)
		puts('ok ')
	else
		puts('* * * error ')
		g_sta='error'
	putsl('interpret '+a.sub(15)+' vs '+b.sub(15))
	rfile.remove(a)
}

check_exe(rstr a,rstr b)
{
	g_total++
	cont=rfile.read_all_n(a)
	cont=cont.sub(cont.find('\x73\x0d\x0a')+3)//去掉golink打印的信息
	if cont.count>0&&cont==rfile.read_all_n(b)
		puts('ok ')
	else
		puts('* * * error ')
		g_sta='error'
	putsl('exe '+a.sub(15)+' vs '+b.sub(15))
	rfile.remove(a)
}

check_jit(rstr a,rstr b)
{
	g_total++
	cont=rfile.read_all_n(a);
	if cont.count>0&&cont==rfile.read_all_n(b)
		puts('ok ')
	else
		puts('* * * error ')
		g_sta='error'
	putsl('jit '+a.sub(15)+' vs '+b.sub(15))
	rfile.remove(a)
}

check_pack(rstr a,rstr b)
{
	g_total++
	cont=rfile.read_all_n(a);
	if cont.count>0&&cont==rfile.read_all_n(b)
		puts('ok ')
	else
		puts('* * * error ')
		g_sta='error'
	putsl('pack '+a.sub(15)+' vs '+b.sub(15))
	rfile.remove(a)
}