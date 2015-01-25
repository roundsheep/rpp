/*
分别以解释方式和编译方式运行example下所有例子，
并与标准答案进行对比，以此来检验编译器的正确性。
*/

import rdir.h
import rfile.h

int g_sta_ok
int g_total

main
{
	use_paral=rf.get_param.get_top=='-paral'
	g_sta_ok=1
	start=rf.tick
	g_total=0
	v=rdir.get_file_bfs('./example/')
	rbuf<rstr> vname
	for i in v
		name=rdir.get_name(v[i])
		suffix=rdir.get_suffix(name)
		if suffix!='h'
			continue
		a=name.sub(0,name.count-2)
		if is_ignore(a)
			continue
		vname.push(a)
	rbuf<int> vtime
	num=4
	time=rf.tick
	
	if use_paral
		vname.parallel(num,lambda(rstr& b){
			check_interpret(b)
		})
	else
		for i in vname
			check_interpret(vname[i])
			
	vtime.push(rf.tick-time)
	time=rf.tick
	
	if use_paral
		vname.parallel(num,lambda(rstr& b){
			check_jit(b)
		})
	else
		for i in vname
			check_jit(vname[i])
	
	vtime.push(rf.tick-time)
	time=rf.tick
	
	if use_paral
		vname.parallel(num,lambda(rstr& b){
			check_pack(b)
		})
	else
		for i in vname
			check_pack(vname[i])
	
	vtime.push(rf.tick-time)
	time=rf.tick
	
	if use_paral
		vname.parallel(num,lambda(rstr& b){
			check_exe(b)
		})
	else
		for i in vname
			check_exe(vname[i])
	
	vtime.push(rf.tick-time)

	putsl
	for i in vtime
		putsl 'time'+i+': '+vtime[i]
	putsl
	putsl('total: '+g_total)
	putsl('time: '+(rf.tick-start))
	putsl('state: '+r_cond<rstr>(g_sta_ok,'ok','error'))
	rf.getch
}

bool is_ignore(rstr a)
{
	ifn a.sub(0,1).is_number
		return true
	if a=='14_2'||a=='22_2'||a=='36_8'||a=='36_9'
		return true
	if a.sub(0,3)=='16_'
		return true
	if a.sub(0,3)=='40_'
		return true
	return false
}

check_interpret(rstr a)
{
	if a=='21_2'||a=='44'||a=='14_1'
		return
	rf.cmd('rpp example/'+a+'.h > example/answer/'+a+'_tmp.txt')
	check_interpret('example/answer/'+a+'_tmp.txt',
		'example/answer/'+a+'.txt')
}

check_jit(rstr a)
{
	rf.cmd('rjit example\\'+a+'.h > example/answer/'+a+'_tmp.txt')
	check_jit('example/answer/'+a+'_tmp.txt','example/answer/'+a+'.txt')
}

check_pack(rstr a)
{
	rf.cmd('rpack example\\'+a+'.h')
	rf.cmd('example\\'+a+'.exe > example/answer/'+a+'_tmp.txt')
	check_pack('example/answer/'+a+'_tmp.txt','example/answer/'+a+'.txt')
	rfile.remove('example\\'+a+'.exe')
}

check_exe(rstr a)
{
	if a=='13'||a=='22_1'||a=='44'||a=='14_1'
			return
	rf.cmd('run example\\'+a+'.h > example/answer/'+a+'_tmp.txt')
	check_exe('example/answer/'+a+'_tmp.txt','example/answer/'+a+'.txt')
}

check_interpret(rstr a,rstr b)
{
	//不严谨，应该使用互斥体
	g_total++
	cont=rfile.read_all_n(a);
	if cont.count>0&&cont==rfile.read_all_n(b)
		puts('ok ')
	else
		puts('* * * error ')
		g_sta_ok=0
	putsl('interpret '+a.sub(15)+' vs '+b.sub(15))
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
		g_sta_ok=0
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
		g_sta_ok=0
	putsl('pack '+a.sub(15)+' vs '+b.sub(15))
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
		g_sta_ok=0
	putsl('exe '+a.sub(15)+' vs '+b.sub(15))
	rfile.remove(a)
}