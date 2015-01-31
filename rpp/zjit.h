#ifndef zjit_h__
#define zjit_h__

#include "zjiti.h"
#include "znasm.h"
#include "zjitf.h"

struct zjit
{
	static rbool run(tsh& sh)
	{
		init_addr_list(sh);
#ifdef _MSC_VER
		tfunc* ptfi=zfind::func_search(*sh.m_main,"__declare");
		if(ptfi!=null)
		{
			func_to_x86(sh,*ptfi,tenv());
		}
		ptfi=zfind::func_search(*sh.m_main,"main_c");
		if(ptfi==null)
		{
			rf::printl("main not find");
			return false;
		}
		ifn(func_to_x86(sh,*ptfi,tenv()))
		{
			return false;
		}
		((void (*)())(ptfi->code))();
		return true;
#else
		return false;
#endif
	}

	//将一个函数翻译成X86代码
	static rbool func_to_x86(tsh& sh,tfunc& tfi,tenv env)
	{
#ifdef _MSC_VER
		if(!tfi.vasm.empty())
			return true;
		if(!zbin::cp_vword_to_vasm(sh,tfi,env))
		{
			return false;
		}
		int size=tfi.vasm.count()*6;//todo 估算法并不是最好的方法
		size=r_ceil_div(size,4096)*4096;
		if(tfi.code==null)
		{
			tfi.code=talloc::alloc_v(size);
			if(tfi.code==null)
			{
				return false;
			}
			sh.m_addr.insert(taddr(
				(uint)tfi.code,(uint)(tfi.code+size),&tfi));
		}
		int cur=0;
		for(int i=0;i<tfi.vasm.count();i++)
		{
			tfi.vasm[i].ptfi=&tfi;
			ifn(a_asm(sh,tfi.vasm[i]))
			{
				return false;
			}
			tfi.vasm[i].start=tfi.code+cur;
			rstr s=asm_to_x86(sh,tfi.vasm[i],tfi.code+cur);
			if(s.empty())
			{
				sh.error(tfi.vasm[i],"can't build jit ins");
				return false;
			}
			if(cur+s.count()>size)
			{
				sh.error(tfi,"func too big");
				return false;
			}
			memcpy(tfi.code+cur,s.begin(),s.count());
			cur+=s.count();
		}
		for(int i=0;i<tfi.vasm.count();i++)
		{
			ifn(fix_addr(sh,tfi.vasm[i],tfi.vasm))
			{
				return false;
			}
		}
		return true;
#else
		return false;
#endif
	}

	static rbool fix_addr(tsh& sh,tasm& oasm,rbuf<tasm>& vasm)
	{
		ifn(oasm.vstr.count()==2&&
			zbin::is_jmp_ins(oasm.ins.type)&&
			oasm.vstr[1].is_number())
		{
			return true;
		}
		int line=oasm.vstr[1].toint();
		int i;
		for(i=0;i<vasm.count();i++)
		{
			if(vasm[i].pos.line>=line)
			{
				break;
			}
		}
		if(i>=vasm.count())
		{
			sh.error(oasm);
			return false;
		}
		ifn(a_asm(sh,oasm))
		{
			return false;
		}
		oasm.ins.first.type=topnd::c_imme;
		oasm.ins.first.val=r_to_uint(vasm[i].start);
		uchar* real=oasm.start;
		rstr s=asm_to_x86(sh,oasm,real);
		if(s.empty())
		{
			return false;
		}
		memcpy(real,s.begin(),s.count());
		return true;
	}

	static rstr asm_to_x86(tsh& sh,tasm& item,uchar* start)
	{
		tins& ins=item.ins;
		rbuf<rstr>& vstr=item.vstr;
		rstr s;
		switch(ins.type)
		{
		case tkey::c_calle:
			return p_calle(sh,ins,start);
		case tkey::c_call:
			return zjiti::b_call(ins,start);
		case tkey::c_retn:
			return zjiti::b_retn();
		case tkey::c_reti:
			return zjiti::b_reti(ins);
		case tkey::c_push:
			return zjiti::b_push(ins);
		case tkey::c_pop:
			return zjiti::b_pop(ins);
		case tkey::c_jmp:
			return zjiti::b_jmp(ins,start);
		case tkey::c_jebxz:
			return rppj4(b_cmp,rppkey(c_ebx),rppoptr(c_comma),"0")+
				zjiti::b_jz(ins,start+6);
		case tkey::c_jebxnz:
			return rppj4(b_cmp,rppkey(c_ebx),rppoptr(c_comma),"0")+
				zjiti::b_jnz(ins,start+6);
		case tkey::c_nop:
			return zjiti::b_nop();
		case tkey::c_lea:
			if(znasm::count_mbk_l(vstr)!=2)
				return zjiti::b_lea(ins);
			//应该检查b_lea是否返回空串
			return rppj4(b_lea,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_mov,rppja,rppoptr(c_comma),rppkey(c_ecx));
		case tkey::c_mov:
			if(znasm::count_mbk_l(vstr)!=2)
				return zjiti::b_mov(ins);
			return rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_mov,rppja,rppoptr(c_comma),rppkey(c_ecx));
		case tkey::c_mov1:
			return rppj4(b_mov1_cl_addr,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_mov1_addr_cl,rppja,rppoptr(c_comma),rppkey(c_ecx));
		case tkey::c_mov8:
			s+=rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb);
			s+=rppj4(b_mov,rppja,rppoptr(c_comma),rppkey(c_ecx));
			ins.first.val+=4;
			ins.second.val+=4;
			s+=rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb);
			s+=rppj4(b_mov,rppja,rppoptr(c_comma),rppkey(c_ecx));
			return r_move(s);
		case tkey::c_add:
			if(znasm::count_mbk_l(vstr)!=2)
				return zjiti::b_add(ins);
			return rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_add,rppja,rppoptr(c_comma),rppkey(c_ecx));
		case tkey::c_sub:
			if(znasm::count_mbk_l(vstr)!=2)
				return zjiti::b_sub(ins);
			return rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_sub,rppja,rppoptr(c_comma),rppkey(c_ecx));
		case tkey::c_imul:
			return rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppja)+
				rppj4(b_imul,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_mov,rppja,rppoptr(c_comma),rppkey(c_ecx));
		case tkey::c_idiv:
			return rppj4(b_mov,rppkey(c_eax),rppoptr(c_comma),rppja)+
				zjiti::b_cdq()+
				rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_idiv,rppkey(c_ecx),rppoptr(c_comma),"0")+
				rppj4(b_mov,rppja,rppoptr(c_comma),rppkey(c_eax));
		case tkey::c_imod:
			return rppj4(b_mov,rppkey(c_eax),rppoptr(c_comma),rppja)+
				zjiti::b_cdq()+
				rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_idiv,rppkey(c_ecx),rppoptr(c_comma),"0")+
				rppj4(b_mov,rppja,rppoptr(c_comma),rppkey(c_edx));
		case tkey::c_udiv:
			return rppj4(b_mov,rppkey(c_eax),rppoptr(c_comma),rppja)+
				rppj4(b_xor,rppkey(c_edx),rppoptr(c_comma),rppkey(c_edx))+
				rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_udiv,rppkey(c_ecx),rppoptr(c_comma),"0")+
				rppj4(b_mov,rppja,rppoptr(c_comma),rppkey(c_eax));
		case tkey::c_umod:
			return rppj4(b_mov,rppkey(c_eax),rppoptr(c_comma),rppja)+
				rppj4(b_xor,rppkey(c_edx),rppoptr(c_comma),rppkey(c_edx))+
				rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_udiv,rppkey(c_ecx),rppoptr(c_comma),"0")+
				rppj4(b_mov,rppja,rppoptr(c_comma),rppkey(c_edx));
		case tkey::c_cesb:
			rppjcmp(b_sete_bl);
		case tkey::c_cnesb:
			rppjcmp(b_setne_bl);
		case tkey::c_cgsb:
			rppjcmp(b_setg_bl);
		case tkey::c_cgesb:
			rppjcmp(b_setge_bl);
		case tkey::c_clsb:
			rppjcmp(b_setl_bl);
		case tkey::c_clesb:
			rppjcmp(b_setle_bl);
		case tkey::c_ucgsb:
			rppjcmp(b_seta_bl);
		case tkey::c_ucgesb:
			rppjcmp(b_setae_bl);
		case tkey::c_uclsb:
			rppjcmp(b_setb_bl);
		case tkey::c_uclesb:
			rppjcmp(b_setbe_bl);
		case tkey::c_band:
			if(znasm::count_mbk_l(vstr)!=2)
				return zjiti::b_and(ins);
			return rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_and,rppja,rppoptr(c_comma),rppkey(c_ecx));
		case tkey::c_bor:
			if(znasm::count_mbk_l(vstr)!=2)
				return zjiti::b_or(ins);
			return rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_or,rppja,rppoptr(c_comma),rppkey(c_ecx));
		case tkey::c_bxor:
			if(znasm::count_mbk_l(vstr)!=2)
				return zjiti::b_xor(ins);
			return rppj4(b_mov,rppkey(c_ecx),rppoptr(c_comma),rppjb)+
				rppj4(b_xor,rppja,rppoptr(c_comma),rppkey(c_ecx));
		case tkey::c_bnot:
			return zjiti::b_not(ins);
		}
		return rstr();
	}

	static rstr p_calle(tsh& sh,tins& ins,uchar* start)
	{
		char* name=(char*)ins.first.val;
		void* temp=null;
		if(sh.m_jit_f.find(name)!=null)
		{
			temp=sh.m_jit_f[name];
		}
		if(temp==null)
		{
			temp=zjitf::find_dll_full(name);
		}
		if(temp==null)
		{
			return rstr();
		}
		ins.first.val=(int)temp;
		return zjiti::b_call(ins,start);
	}

	static rbool a_asm(tsh& sh,tasm& item)
	{
		int i=zbin::find_comma(sh,item.vstr);
		if(!a_opnd(sh,item,i-1,item.vstr.sub(1,i),item.ins.first))
			return false;
		if(!a_opnd(sh,item,i+1,item.vstr.sub(i+1),item.ins.second))
			return false;
		item.ins.type=sh.m_key.get_key_index(item.vstr.get_bottom());
		return true;
	}

	static rbool a_opnd(tsh& sh,tasm& item,int index,const rbuf<rstr>& v,topnd& o)
	{
		if(v.count()==7&&v[1]==rppoptr(c_addr))
		{
			tfunc* ptfi=znasm::call_find(sh,item);
			if(ptfi==null)
			{
				return false;
			}
			ifn(func_to_x86(sh,*ptfi,tenv()))
			{
				return false;
			}
			o.type=topnd::c_imme;
			o.val=r_to_uint(ptfi->code);
			return true;
		}
		return zbin::a_opnd(sh,item,index,v,o);
	}

	static tins build_ins(tsh& sh,rstr s1,rstr s2,rstr s3,rstr s4)
	{
		tasm item;
		item.vstr+=s1;
		item.vstr+=s2;
		item.vstr+=s3;
		item.vstr+=s4;
		a_asm(sh,item);
		return item.ins;
	}

	static tins build_ins(tsh& sh,rstr s1,rstr s2,rstr s3,rbuf<rstr> s4)
	{
		tasm item;
		item.vstr+=s1;
		item.vstr+=s2;
		item.vstr+=s3;
		item.vstr+=s4;
		a_asm(sh,item);
		return item.ins;
	}

	static tins build_ins(tsh& sh,rstr s1,rbuf<rstr> s2,rstr s3,rstr s4)
	{
		tasm item;
		item.vstr+=s1;
		item.vstr+=s2;
		item.vstr+=s3;
		item.vstr+=s4;
		a_asm(sh,item);
		return item.ins;
	}

	//非多线程安全
	static void* find_func(char* name)
	{
		tfunc* ptfi=zbin::find_func(*zjitf::get_psh(),name);
		if(ptfi==null)
		{
			return null;
		}
		ifn(func_to_x86(*zjitf::get_psh(),*ptfi,tenv()))
		{
			return null;
		}
		return ptfi->code;
	}

	static void init_addr_list(tsh& sh)
	{
		rppjf("add8",zjitf::add8);
		rppjf("sub8",zjitf::sub8);
		rppjf("imul8",zjitf::imul8);
		rppjf("idiv8",zjitf::idiv8);
		rppjf("imod8",zjitf::imod8);
		rppjf("cgsb8",zjitf::cgsb8);
		rppjf("clsb8",zjitf::clsb8);
		rppjf("fadd8",zjitf::fadd8);
		rppjf("fsub8",zjitf::fsub8);
		rppjf("fmul8",zjitf::fmul8);
		rppjf("fdiv8",zjitf::fdiv8);
		rppjf("fcgsb8",zjitf::fcgsb8);
		rppjf("fclsb8",zjitf::fclsb8);
		rppjf("bshl",zjitf::bshl);
		rppjf("bshr",zjitf::bshr);
		rppjf("bsar",zjitf::bsar);
		rppjf("get_argc",zjitf::get_argc);
		rppjf("get_argv",zjitf::get_argv);
		rppjf("get_hins",zjitf::get_hins);
		rppjf("set_main_ret",zjitf::set_main_ret);
		rppjf("find_func",find_func);
		rppjf("find_dll",zjitf::find_dll_q);
		rppjf("utf8_to_gbk",rcode::utf8_to_gbk_c);
		rppjf("get_cur_func",zjitf::get_cur_func);
		rppjf("get_vclass",zjitf::get_vclass);
		rppjf("eval",eval);
		rppjf("eval_vstr",eval_vstr);
		rppjf("_float_to_double",zjitf::_float_to_double);
		rppjf("sin",zjitf::sin);
		rppjf("cos",zjitf::cos);
	}

	static rbool eval_vstr(rbuf<rstr>& vstr,tenv env)
	{
		rbuf<tword> v;
		for(int i=0;i<vstr.count();i++)
		{
			v+=tword(vstr[i]);
			v[i].pos.line=1;
		}
		return eval_v(v,env);
	}

	static rbool eval_v(rbuf<tword>& v,tenv env)
	{
		tsh& sh=*zjitf::get_psh();
		zpre::def_replace(sh,sh.m_vdefine,v);
		if(!zctl::type_replace(sh,v))
		{
			sh.error("type_replace");
			return false;
		}
		tfunc tfi;
		tfi.ptci=sh.m_main;
		tfi.vword=v;
		tfi.first_pos.line=1;
		tfi.last_pos.line=tfi.vword.get_top().pos.line+10;
		tfi.retval.type=rppkey(c_int);
		tfi.retval.name=rppkey(c_s_ret);
		tfi.is_cfunc=true;
		if(!func_to_x86(sh,tfi,env))
		{
			sh.error("func_to_x86");
			return false;
		}
		uchar* temp=tfi.code;
		__asm sub esp,4
		__asm call temp
		__asm mov esi,[esp]
		__asm mov temp,esi
		__asm add esp,4
		return (int)temp;
	}

	static rbool eval(uchar* s,tenv env)
	{
		tsh& sh=*zjitf::get_psh();
		rbuf<tword> v;
		if(!zpre::str_analyse(sh,rstr(s),v,null))
		{
			return false;
		}
		return eval_v(v,env);
	}
};

#endif
