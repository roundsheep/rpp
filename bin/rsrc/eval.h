
import rpp.h

bool evalue(rstr s)
{
	return eval(s,get_up_func,get_up_ebp)
}

bool eval(rstr s,tfunc* ptfi=null,int vebp=0)
{
	return eval_in(s.cstr,ptfi,vebp)
}

bool eval_in(char* s,tfunc* ptfi,int vebp)
{
	push vebp
	push ptfi
	push s
	calle "eval"
	mov s_ret,eax
	add esp,12
}

bool eval_vstr(rbuf<rstr>* vstr,tfunc* ptfi,int vebp)
{
	push vebp
	push ptfi
	push vstr
	calle "eval_vstr"
	mov s_ret,eax
	add esp,12
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
	push pasm
	calle "get_cur_func"
	mov s_ret,eax
	add esp,4
}

dynamic bool and(rbuf<rbuf<rstr>>* p,tfunc* ptfi,int vebp)
{
	for i=0;i<p->count;i++
		rbuf<rstr> v
		v.push('return')
		v+=(*p)[i]
		ifn eval_vstr(&v,ptfi,vebp)
			return false
	return true
}

dynamic bool or(rbuf<rbuf<rstr>>* p,tfunc* ptfi,int vebp)
{
	for i=0;i<p->count;i++
		rbuf<rstr> v
		v.push('return')
		v+=(*p)[i]
		if eval_vstr(&v,ptfi,vebp)
			return true
	return false
}