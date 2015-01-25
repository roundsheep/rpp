
import rpp.h

bool evalue(rstr s)
{
	return eval(s,get_up_func,get_up_ebp)
}

bool eval(rstr s,tfunc* env,uint v_ebp)
{
	if v_ebp!=0
		s="mov _PENV,"+rstr(v_ebp)+";"+s
	return eval_in(s.cstr,env)
}

bool eval_in(char* s,tfunc* env)
{
	sub esp,4
	push env
	push s
	calle "eval"
	mov s_ret,eax
	add esp,8
	add esp,4
}

define get_up_func get_up_func_in(ebp,sizeof(s_local))
define get_cur_ebp ebp
define get_up_ebp (*ebp.to<int*>)

tfunc* get_up_func_in(int v_ebp,int local_size)
{
	return get_cur_func_in(*(v_ebp+local_size+4).to<int*>);
}

tfunc* get_cur_func()
{
	mov ecx,[ebp+4]
	return get_cur_func_in(ecx)
}

tfunc* get_cur_func_in(int pasm)
{
	sub esp,4
	push pasm
	calle "get_cur_func"
	mov s_ret,eax
	add esp,4
	add esp,4
}