#ifndef znasm_h__
#define znasm_h__

#include "zbin.h"

struct znasm
{
	static rbool proc(tsh& sh)
	{
		tfunc* ptfi=zfind::func_search(*sh.m_main,"main");
		if(ptfi==null)
		{
			rf::printl("main not find");
			return false;
		}
		rstr result;
		rbuf<rstr> vconst;
		ifn(proc_func(sh,*ptfi,result,vconst))
		{
			return false;
		}
		rstr head;
		head+="%include '"+rcode::utf8_to_gbk(
			rdir::get_exe_dir().torstr())+"nasm/windemos.inc'\n";
		head+="%include '"+rcode::utf8_to_gbk(
			rdir::get_exe_dir().torstr())+"nasm/dec.inc'\n";
		//head+="IMPORT _getch\n"; nasm的一个bug?

		head+="entry demo11\n";
		head+="\n";
		head+="[section .bss]\n";
		head+="align 32\n";
		head+="_ARGC: resb 4\n";
		head+="_ARGV: resb 4\n";
		head+=rstr("_PMAIN_A: resb ")+sh.m_main_data.size()+"\n";
		add_lambda(sh,head);
		head+="\n";
		head+="[section .data]\n";
		head+="align 32\n";
		head+="db 0,0,0,0\n";//防止数据段为空
		add_str(sh,head,vconst);
		
		head+="\n";
		head+="[section .text]\n";
		head+="proc demo11, ptrdiff_t argcount, ptrdiff_t cmdline\n";
		head+="locals none\n";
		head+="	mov ecx , [ argv(.argcount) ]\n";
		head+="	mov [ _ARGC ] , ecx\n";
		head+="	mov ecx , [ argv(.cmdline) ]\n";
		head+="	mov [ _ARGV ] , ecx\n";
		head+="	sub esp , 4\n";
		head+="	call main2Emain2829\n";
		head+="	add esp , 4\n";
		head+="	xor eax , eax\n";
		head+="endproc\n";
		head+="proc _rpp_main\n";
		head+="locals none\n";
		head+=result;
		head+="\n";
		head+="endproc\n";
		rstrw name=tsh::get_main_name(sh)+".asm";
		rfile file;
		ifn(file.open_n(name,"rw"))
		{
			rf::printl("file open error");
			return false;
		}
		ifn(file.write(head.size(),head.begin()))
		{
			rf::printl("file write error");
			return false;
		}
		return true;
	}

	static rbool proc_grub(tsh& sh)
	{
		tfunc* ptfi=zfind::func_search(*sh.m_main,"main");
		if(ptfi==null)
		{
			rf::printl("main not find");
			return false;
		}
		rstr result;
		rbuf<rstr> vconst;
		ifn(proc_func(sh,*ptfi,result,vconst))
		{
			return false;
		}
		rstr head;
		head+="org 4194304\n";
		head+="MODULEALIGN equ 1<<0\n";
		head+="MEMINFO equ 1<<1\n";
		head+="FLAGS equ MODULEALIGN|MEMINFO|(1<<16)\n";
		head+="MAGIC equ 0x1BADB002\n";
		head+="CHECKSUM equ-(MAGIC+FLAGS)\n";
		head+="bits 32\n";
		head+="\n";
		head+="section .text\n";
		head+="align 4\n";
		head+="MultiBootHeader:\n";
		head+="	dd MAGIC\n";
		head+="	dd FLAGS\n";
		head+="	dd CHECKSUM\n";
		head+="	dd MultiBootHeader\n";
		head+="	dd MultiBootHeader\n";
		head+="	dd MultiBootEnd\n";
		head+="	dd _stack_end\n";
		head+="	dd _loader\n";

		head+="\n";
		head+="STACKSIZE equ 1024*1024\n";
		head+="_loader:\n";
		head+="	mov esp,_stack_start+STACKSIZE\n";
		head+="	finit\n";
		head+="	cli\n";

		head+="	call rf2Einit5Fheap2829\n";

		head+="	sub esp,4\n";
		head+="	call main2Emain2829\n";
		head+="	add esp,4\n";

		head+="_hang:\n";
		head+="	hlt\n";
		head+="	jmp _hang\n";

		head+=result;

		head+="\n";
		head+="MultiBootEnd:\n";

		head+="section .data\n";
		head+="align 32\n";
		head+="db 0,0,0,0\n";//防止数据段为空
		add_str(sh,head,vconst);
		head+="\n";

		head+="section .bss\n";
		head+="align 32\n";
		head+="_stack_start:\n";
		head+="	resb STACKSIZE\n";
		head+="_stack_end:\n";
		head+="\n";

		head+="_ARGC: resb 4\n";
		head+="_ARGV: resb 4\n";
		head+=rstr("_PMAIN_A: resb ")+sh.m_main_data.size()+"\n";
		add_lambda(sh,head);

		rstrw name=tsh::get_main_name(sh)+".asm";
		rfile file;
		ifn(file.open_n(name,"rw"))
		{
			rf::printl("file open error");
			return false;
		}
		ifn(file.write(head.size(),head.begin()))
		{
			rf::printl("file write error");
			return false;
		}
		return true;
	}

	static void add_str(tsh& sh,rstr& result,rbuf<rstr>& vconst)
	{
		for(int i=0;i<vconst.count();i++)
		{
			zbin::trans_cstr(vconst[i]);
			add_str_one(result,i,vconst[i]);
		}
	}

	static void add_str_one(rstr& result,int index,const rstr& s)
	{
		result+="_RC_"+rstr(index)+": db ";
		for(int i=0;i<s.count()-1;i++)
		{
			result+=rstr((int)s[i])+",";
		}
		result+="0\n";
	}

	static void add_lambda(tsh& sh,rstr& result)
	{
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
		{
			add_lambda(result,*p);
		}
	}

	static void add_lambda(rstr& result,tclass& tci)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if(p->lambda_data.empty())
			{
				continue;
			}
			result+=p->name+"_C_P_A: resb "+p->lambda_data.size()+"\n";
		}
	}

	static tfunc* call_find(tsh& sh,tasm& item)
	{
		int i;
		for(i=1;i<item.vstr.count();i++)
		{
			if(item.vstr[i]=="&")
			{
				break;
			}
		}
		//一条指令里最多出现一次&,而且 &操作数 只能是最后一个操作数
		if(i>=item.vstr.count())
		{
			return null;
		}
		i++;
		tclass* ptci=zfind::class_search(sh,item.vstr.get(i));
		if(ptci==null)
		{
			return null;
		}
		i++;
		rstr fname=item.vstr.get(i);
		return zfind::func_search_dec(*ptci,fname);
	}

	static rstr symbol_trans(const rstr& s)
	{
		rstr ret;
		for(int i=0;i<s.count();i++)
		{
			if(rstr::is_alpha(s[i])||rstr::is_number(s[i]))
			{
				ret+=s[i];
			}
			else
			{
				ret+=rstr::format("%02X",(int)s[i]);
			}
		}
		return r_move(ret);
	}

	static rstr get_nasm_symbol(tfunc& tfi)
	{
		return symbol_trans(tfi.ptci->name+"."+tfi.name_dec);
	}

	static rbool is_jmp_ins_nasm(const rstr& s)
	{
		return s=="call"||s=="je"||s=="jne"||s=="jg"||s=="jge"||
			s=="jl"||s=="jle"||s=="ja"||s=="jae"||s=="jb"||s=="jbe";
	}

	static rbool line_update(tsh& sh,rbuf<tasm>& vasm,rbuf<uchar>& vline)
	{
		for(int j=0;j<vasm.count();j++)
		{
			if(vasm[j].vstr.count()!=2&&vasm[j].vstr.count()!=3)
			{
				continue;
			}
			ifn(vasm[j].vstr.top().is_number())
			{
				continue;
			}
			int type=sh.m_key.get_key_index(vasm[j].vstr.get_bottom());
			if(type>tkey::c_rn)
			{
				sh.error(vasm[j]);
				return false;
			}
			ifn(zbin::is_jmp_ins(type)||
				type==tkey::c_rn&&is_jmp_ins_nasm(vasm[j].vstr[1]))
			{
				continue;
			}
			int line=vasm[j].vstr.top().toint();
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
				sh.error(vasm[j]);
				return false;
			}
			vasm[j].vstr.top()=rstr(vasm[i].pos.line);
			vline[i]=1;
		}
		return true;
	}

	//将一个函数翻译成NASM汇编代码
	static rbool proc_func(tsh& sh,tfunc& tfi,rstr& result,
		rbuf<rstr>& vconst,int level=0)
	{
		if(level++>300)
		{
			sh.error(tfi,"nasm level overflow");
			return false;
		}
		if(tfi.count==1)
		{
			return true;
		}
		tfi.count=1;
		if(tfi.vasm.empty())
		{
			if(!zbin::cp_vword_to_vasm(sh,tfi))
			{
				return false;
			}
		}
		
		rbuf<uchar> vline(tfi.vasm.count());
		memset(vline.begin(),0,vline.count());
		ifn(line_update(sh,tfi.vasm,vline))
		{
			sh.error(tfi);
			return false;
		}
		rstr symbol=get_nasm_symbol(tfi);
		result+="\n"+symbol+":\n";
		for(int i=0;i<tfi.vasm.count();i++)
		{
			if(i==0||tfi.vasm[i].pos.line!=tfi.vasm[i-1].pos.line)
			{
				if(vline[i])
				{
					result+=symbol+"_"+tfi.vasm[i].pos.line+":\n";
				}
			}
			rstr temp;
			ifn(proc_asm(sh,tfi,tfi.vasm[i],temp,vconst))
			{
				sh.error(tfi.vasm[i]);
				sh.error(&tfi);
				return false;
			}
			result+=temp;
		}
		
		for(int i=0;i<tfi.vasm.count();i++)
		{
			tfunc* ptfi=call_find(sh,tfi.vasm[i]);
			if(ptfi==null)
			{
				continue;
			}
			ifn(proc_func(sh,*ptfi,result,vconst,level))
			{
				return false;
			}
		}
		return true;
	}

	static void proc_const_str(rbuf<rstr>& vstr,rbuf<rstr>& vconst)
	{
		for(int i=0;i<vstr.count();i++)
		{
			if(vstr[i].get_bottom()=='"'&&vstr[i].count()>=2)
			{
				vconst.push(vstr[i]);
				vstr[i]="_RC_"+rstr(vconst.count()-1);
			}
		}
	}

	static rbool have_single_esp(tsh& sh,tasm& item)
	{
		for(int i=0;i<item.vstr.count();i++)
		{
			if(item.vstr[i]==rppkey(c_esp)||item.vstr[i]==rppkey(c_ebp))
			{
				if(item.vstr.get(i-1)!=rppoptr(c_mbk_l))
				{
					return true;
				}
				if(item.vstr.get(i+2).toint()<4)
				{
					return true;
				}
				if(item.vstr.get(i+1)!=rppoptr(c_plus))
				{
					return true;
				}
			}
			if(item.vstr[i].get_bottom()=='"'&&item.vstr[i].count()>=2)
			{
				return true;
			}
		}
		return false;
	}

	static void fix_esp(tsh& sh,tasm& item)
	{
		for(int i=0;i<item.vstr.count();i++)
		{
			if(item.vstr[i]!=rppkey(c_esp)&&item.vstr[i]!=rppkey(c_ebp))
			{
				continue;
			}
			if(item.vstr.count()>i+2&&item.vstr[i+2].is_number())
			{
				item.vstr[i+2]=item.vstr[i+2].toint()-4;
			}
		}
	}

	static rbool proc_inline(tsh& sh,rstr& result,tfunc* ptfi,rbool& is_inline)
	{
		is_inline=false;
		rbuf<tasm>& vasm=ptfi->vasm;
		if(vasm.empty())
		{
			if(!zbin::cp_vword_to_vasm(sh,*ptfi))
			{
				return false;
			}
		}
		if(vasm.count()>10)
		{
			return true;
		}
		for(int i=0;i<vasm.count()-1;i++)
		{
			int type=sh.m_key.get_key_index(vasm[i].vstr.get_bottom());
			if(type>tkey::c_rn)
			{
				sh.error(vasm[i]);
				return false;
			}
			if(zbin::is_jmp_ins(type)||
				type==tkey::c_rn&&is_jmp_ins_nasm(vasm[i].vstr.get(1)))
			{
				return true;
			}
			//加入call会陷入无限递归
			if(type==tkey::c_reti||tkey::c_retn==type||type==tkey::c_call)
			{
				return true;
			}
			if(have_single_esp(sh,vasm[i]))
			{
				return true;
			}
		}
		rbuf<tasm> temp=vasm;
		rbuf<rstr> vconst;//实际上这个vconst没用
		for(int i=0;i<temp.count()-1;i++)
		{
			fix_esp(sh,temp[i]);
			if(!proc_asm(sh,*ptfi,temp[i],result,vconst))
			{
				return false;
			}
		}
		if(temp.count()>=1&&temp.top().vstr.get(0)==rppkey(c_reti)&&
			temp.top().vstr.get(1).toint()!=0)
		{
			result+="	add esp,"+temp.top().vstr.get(1)+"\n";
		}
		is_inline=true;
		return true;
	}

	static rbool proc_asm(tsh& sh,tfunc& tfi,tasm& item,
		rstr& result,rbuf<rstr>& vconst)
	{
		rbuf<rstr>& vstr=item.vstr;
		proc_const_str(vstr,vconst);
		if(vstr.empty())
		{
			return false;
		}
		int type=sh.m_key.get_key_index(vstr[0]);
		tfunc* ptfi;
		switch(type)
		{
		case tkey::c_call:
			ptfi=call_find(sh,item);
			if(ptfi==null)
			{
				result+="	call dword "+
					link_vstr(vstr.sub(1))+"\n";
				return true;
			}
			result+="	call "+get_nasm_symbol(*ptfi)+"\n";
			return true;
		case tkey::c_retn:
			result+="	ret\n";
			return true;
		case tkey::c_reti:
			result+="	ret "+vstr.get(1)+"\n";
			return true;
		case tkey::c_push:
			ptfi=call_find(sh,item);
			if(ptfi==null)
			{
				result+="	push dword "+link_vstr(vstr.sub(1))+"\n";
			}
			else
			{
				result+="	push "+get_nasm_symbol(*ptfi)+"\n";
			}
			return true;
		case tkey::c_pop:
			result+="	pop dword "+link_vstr(vstr.sub(1))+"\n";
			return true;
		case tkey::c_jmp:
			result+="	jmp "+get_nasm_symbol(tfi)+"_"+vstr.get(1)+"\n";
			return true;
		case tkey::c_jebxz:
			result+="	cmp ebx , 0\n";
			result+="	jz "+get_nasm_symbol(tfi)+"_"+vstr.get(1)+"\n";
			return true;
		case tkey::c_jebxnz:
			result+="	cmp ebx , 0\n";
			result+="	jnz "+get_nasm_symbol(tfi)+"_"+vstr.get(1)+"\n";
			return true;
		case tkey::c_nop:
			result+="	nop\n";
			return true;
		case tkey::c_lea:
			if(count_mbk_l(vstr)==2)
			{
				result+="	lea ecx , "+get_opnd2(vstr)+"\n";
				result+="	mov "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	lea dword "+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_mov:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	mov "+get_opnd1(vstr)+" , ecx\n";
				return true;
			}
			ptfi=call_find(sh,item);
			if(ptfi==null)
			{
				result+="	mov dword "+link_vstr(vstr.sub(1))+"\n";
			}
			else
			{
				result+="	mov dword "+get_opnd1(vstr)+" , "+
					get_nasm_symbol(*ptfi)+"\n";
			}
			return true;
		case tkey::c_mov1:
			result+="	mov cl , "+get_opnd2(vstr)+"\n";
			result+="	mov "+get_opnd1(vstr)+" , cl\n";
			return true;
		case tkey::c_mov8:
			return false;
		case tkey::c_add:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	add "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	add dword "+
					link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_sub:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	sub "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	sub dword "+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_imul:
			result+="	mov ecx , "+get_opnd1(vstr)+"\n";
			result+="	imul ecx , "+get_opnd2(vstr)+"\n";
			result+="	mov "+get_opnd1(vstr)+" , ecx\n";
			return true;
		case tkey::c_idiv:
			result+="	mov eax , "+get_opnd1(vstr)+"\n";
			result+="	cdq\n";
			result+="	mov ecx , "+get_opnd2(vstr)+"\n";
			result+="	idiv ecx\n";
			result+="	mov "+get_opnd1(vstr)+" , eax\n";
			return true;
		case tkey::c_imod:
			result+="	mov eax , "+get_opnd1(vstr)+"\n";
			result+="	cdq\n";
			result+="	mov ecx , "+get_opnd2(vstr)+"\n";
			result+="	idiv ecx\n";
			result+="	mov "+get_opnd1(vstr)+" , edx\n";
			return true;
		case tkey::c_cesb:
			rppncmp("	sete bl\n");
		case tkey::c_cnesb:
			rppncmp("	setne bl\n");
		case tkey::c_cgsb:
			rppncmp("	setg bl\n");
		case tkey::c_cgesb:
			rppncmp("	setge bl\n");
		case tkey::c_clsb:
			rppncmp("	setl bl\n");
		case tkey::c_clesb:
			rppncmp("	setle bl\n");
		case tkey::c_add8:
			return false;
		case tkey::c_sub8:
			return false;
		case tkey::c_imul8:
			return false;
		case tkey::c_idiv8:
			return false;
		case tkey::c_imod8:
			return false;
		case tkey::c_cgsb8:
			return false;
		case tkey::c_clsb8:
			return false;
		case tkey::c_fadd8:
			return false;
		case tkey::c_fsub8:
			return false;
		case tkey::c_fmul8:
			return false;
		case tkey::c_fdiv8:
			return false;
		case tkey::c_fcgsb8:
			return false;
		case tkey::c_fclsb8:
			return false;
		case tkey::c_band:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	and "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	and dword "+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_bor:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	or "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	or dword "+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_bnot:
			result+="	not dword "+link_vstr(vstr.sub(1))+"\n";
			return true;
		case tkey::c_bxor:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	xor "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	xor dword "+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_bshl:
			return false;
		case tkey::c_bshr:
			return false;
		case tkey::c_bsar:
			return false;
		case tkey::c_udiv:
			result+="	mov eax , "+get_opnd1(vstr)+"\n";
			result+="	xor edx , edx\n";
			result+="	mov ecx , "+get_opnd2(vstr)+"\n";
			result+="	div ecx\n";
			result+="	mov "+get_opnd1(vstr)+" , eax\n";
			return true;
		case tkey::c_umod:
			result+="	mov eax , "+get_opnd1(vstr)+"\n";
			result+="	xor edx , edx\n";
			result+="	mov ecx , "+get_opnd2(vstr)+"\n";
			result+="	div ecx\n";
			result+="	mov "+get_opnd1(vstr)+" , edx\n";
			return true;
		case tkey::c_ucgsb:
			rppncmp("	seta bl\n");
		case tkey::c_ucgesb:
			rppncmp("	setae bl\n");
		case tkey::c_uclsb:
			rppncmp("	setb bl\n");
		case tkey::c_uclesb:
			rppncmp("	setbe bl\n");
		case tkey::c_rn:
			if(vstr.count()==3&&vstr.top().is_number()&&
				is_jmp_ins_nasm(vstr[1]))
			{
				result+="	"+vstr[1]+" "+get_nasm_symbol(tfi)+"_"+
					vstr.top()+"\n";
			}
			else
			{
				result+="	"+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		}
		return false;
	}

	static rstr link_vstr(const rbuf<rstr>& v)
	{
		return rstr::join<rstr>(v," ");
	}

	static rstr get_opnd1(rbuf<rstr>& vstr)
	{
		return link_vstr(get_opnd1_v(vstr));
	}

	static rbuf<rstr> get_opnd1_v(rbuf<rstr>& vstr)
	{
		int i;
		int count=0;
		for(i=1;i<vstr.count();i++)
		{
			if("("==vstr[i])
			{
				count++;
			}
			elif(")"==vstr[i])
			{
				count--;
			}
			elif(count==0&&vstr[i]==",")
				break;
		}
		return vstr.sub(1,i);
	}

	static rstr get_opnd2(rbuf<rstr>& vstr)
	{
		return link_vstr(get_opnd2_v(vstr));
	}

	static rbuf<rstr> get_opnd2_v(rbuf<rstr>& vstr)
	{
		int i;
		int count=0;
		for(i=1;i<vstr.count();i++)
		{
			if("("==vstr[i])
			{
				count++;
			}
			elif(")"==vstr[i])
			{
				count--;
			}
			elif(count==0&&vstr[i]==",")
				break;
		}
		return vstr.sub(i+1);
	}

	static int count_mbk_l(rbuf<rstr>& vstr)
	{
		int sum=0;
		for(int i=0;i<vstr.count();i++)
		{
			if(vstr[i]=="[")
			{
				sum++;
			}
		}
		return sum;
	}
};

#endif
