#ifndef tvm_h__
#define tvm_h__

#include "zjit.h"
#include "../rlib/rthread.h"
#include "../rlib/rsock.h"
#include "tanalyse.h"
#include "../rlib/rdic.h"
#include "../rlib/rlist.h"
#include "tvar.h"

struct tvm;

//虚拟机的一个线程
struct tvm_t
{
	treg m_reg;
	rbuf<uchar> m_stack;
	int m_thread;
	rbuf<tasm> m_vasm;//一组初始化指令
	tfunc m_meta;//元函数
	rdic<void*> m_func;//外部函数字典，每个线程一个则不需要使用互斥体
	tvm* m_pvm;
};

struct tvm
{
	rlist<tvm_t> m_list;
	tsh m_sh;

	static rbool other_call(int index,tsh* sh,tvm_t* pvmt,treg& reg)
	{
		if(index<2000)
		{
			return false;//cocos_call
		}
		return false;
	}

	//为了提高效率将1条指令分解为6条
	static rbool qrun(tsh& sh,tvm_t* pvmt,treg reg,treg* poldreg=null)
	{
		tins* cur;
next:
		cur=(tins*)(reg.eip);
		//m_bin.print_reg(reg);
		//((tasm*)cur)->vstr.print();
		//rf::printl();
		//rf::getch();
		
		switch(cur->type)
		{
		case tins::c_rjit_n:
			if(!zbin::cp_call_asm(sh,*(tasm*)cur))
			{
				sh.error(((tasm*)cur)->ptfi);
				return false;
			}
			goto next;
		case tins::c_rjit_n+1:
		case tins::c_rjit_n+2:
		case tins::c_rjit_n+3:
		case tins::c_rjit_n+4:
		case tins::c_rjit_n+5:
			return false;

		case tins::c_calle_i:
			switch(v_get_imme(cur->first))
			{
			case tins::c_eval_txt:
				return false;
			case tins::c_cp_txt:
				pvmt->m_meta.clear();
				if(!zbin::cp_func_txt(sh,
					pvmt->m_meta,(char*)v_pto_uint(reg.esp)))
				{
					sh.error(((tasm*)cur)->ptfi);
					return false;
				}
				reg.esp+=4;
				v_next_ins;
			case tins::c_find_meta:
				v_pto_uint(reg.esp)=(uint)pvmt->m_meta.vasm.begin();
				v_next_ins;
			case tins::c_find_func:
				v_pto_uint(reg.esp+4)=zbin::find_func_bin(sh,
					(char*)v_pto_uint(reg.esp));
				reg.esp+=4;
				v_next_ins;
			case tins::c_find_dll:
				v_pto_uint(reg.esp+4)=(uint)find_dll(
					pvmt,(char*)v_pto_uint(reg.esp));
				reg.esp+=4;
				v_next_ins;
			case tins::c_stdcall_func:
				stdcall_func(reg);
				v_next_ins;
			case tins::c_cdecl_func:
				cdecl_func(reg);
				v_next_ins;
			case tins::c_get_vclass:
				v_pto_uint(reg.esp)=(uint)&sh.m_class;
				v_next_ins;

			case tins::c_thr_create:
				v_pto_uint(reg.esp+8)=pvmt->m_pvm->create_thr(sh,
					(void*)v_pto_uint(reg.esp),
					(void*)v_pto_uint(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			case tins::c_thr_wait:
				if(v_pto_uint(reg.esp)!=0)
					rthread::wait_exit(v_pto_uint(reg.esp));
				reg.esp+=4;
				v_next_ins;

			case tins::c_mu_init:
				v_pto_int(reg.esp)=(int)(new rmutex);
				v_next_ins;
			case tins::c_mu_del:
				delete ((rmutex*)v_pto_int(reg.esp));
				reg.esp+=4;
				v_next_ins;
			case tins::c_mu_enter:
				((rmutex*)v_pto_int(reg.esp))->enter();
				reg.esp+=4;
				v_next_ins;
			case tins::c_mu_leave:
				((rmutex*)v_pto_int(reg.esp))->leave();
				reg.esp+=4;
				v_next_ins;

			case tins::c_execmd:
				v_pto_uint(reg.esp+4)=rf::execmd(
					(char*)v_pto_uint(reg.esp));
				reg.esp+=4;
				v_next_ins;
			case tins::c_srand:
				rf::srand();
				v_next_ins;
			case tins::c_rand:
				v_pto_int(reg.esp)=rf::rand();
				v_next_ins;
			case tins::c_gettick:
				v_pto_int(reg.esp)=rf::tick();
				v_next_ins;
			case tins::c_sleep:
				rf::sleep(v_pto_int(reg.esp));
				reg.esp+=4;
				v_next_ins;

			case tins::c_puts:
				printf("%s",r_to_pchar(v_pto_int(reg.esp)));
				reg.esp+=4;
				v_next_ins;
			case tins::c_puts_l:
				printf("%s",rstr(r_to_pchar(v_pto_int(reg.esp)),
					v_pto_int(reg.esp+4)).cstr());
				reg.esp+=8;
				v_next_ins;
			case tins::c_getch:
				v_pto_int(reg.esp)=rf::getch();
				v_next_ins;
			case tins::c_getstdin:
				v_pto_int(reg.esp)=(int)stdin;
				v_next_ins;
			case tins::c_getstdout:
				v_pto_int(reg.esp)=(int)stdout;
				v_next_ins;
			case tins::c_get_argc:
				v_pto_int(reg.esp)=sh.m_argc;
				v_next_ins;
			case tins::c_get_argv:
				v_pto_int(reg.esp)=(int)sh.m_argv;
				v_next_ins;

			case tins::c_sprintf:
				sprintf(r_to_pchar(v_pto_int(reg.esp)),
					r_to_pchar(v_pto_int(reg.esp+4)),
					v_pto_int(reg.esp+8));
				reg.esp+=12;
				v_next_ins;
			case tins::c_sprintf8:
				sprintf((char*)v_pto_int(reg.esp),
					(char*)v_pto_int(reg.esp+4),
					v_pto_int8(reg.esp+8));
				reg.esp+=16;
				v_next_ins;
			case tins::c_sscanf:
				sscanf((char*)v_pto_int(reg.esp),
					(char*)v_pto_int(reg.esp+4),
					v_pto_int(reg.esp+8));
				reg.esp+=12;
				v_next_ins;

			case tins::c_malloc:
				v_pto_uint(reg.esp+4)=r_to_uint(
					malloc(v_pto_uint(reg.esp)));
				reg.esp+=4;
				v_next_ins;
			case tins::c_mfree:
				free(r_to_pchar(v_pto_uint(reg.esp)));
				reg.esp+=4;
				v_next_ins;
			case tins::c_memset:
				memset((char*)v_pto_uint(reg.esp),
					v_pto_int(reg.esp+4),
					v_pto_uint(reg.esp+8));
				reg.esp+=12;
				v_next_ins;
			case tins::c_memcpy:
				memcpy((char*)v_pto_uint(reg.esp),
					(char*)v_pto_uint(reg.esp+4),
					v_pto_uint(reg.esp+8));
				reg.esp+=12;
				v_next_ins;
			
			case tins::c_strcpy:
				strcpy((char*)v_pto_uint(reg.esp),
					(char*)v_pto_uint(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			case tins::c_strcmp:
				v_pto_int(reg.esp+8)=strcmp(
					(char*)v_pto_uint(reg.esp),
					(char*)v_pto_uint(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			case tins::c_strlen:
				v_pto_uint(reg.esp+4)=
					strlen((char*)v_pto_uint(reg.esp));
				reg.esp+=4;
				v_next_ins;
			
			case tins::c_fopen_w:
				v_pto_uint(reg.esp+8)=(int)rfile::fopen_w(
					(wchar*)v_pto_uint(reg.esp),
					(wchar*)v_pto_uint(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			case tins::c_fopen:
				v_pto_uint(reg.esp+8)=(uint)fopen(
					(char*)v_pto_uint(reg.esp),
					(char*)v_pto_uint(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			case tins::c_fclose:
				v_pto_int(reg.esp+4)=fclose(
					(FILE*)v_pto_uint(reg.esp));
				reg.esp+=4;
				v_next_ins;
			case tins::c_fread:
				v_pto_uint(reg.esp+16)=fread(
					(void*)v_pto_uint(reg.esp),
					v_pto_uint(reg.esp+4),
					v_pto_uint(reg.esp+8),
					(FILE*)v_pto_uint(reg.esp+12));
				reg.esp+=16;
				v_next_ins;
			case tins::c_fwrite:
				v_pto_uint(reg.esp+16)=fwrite(
					(void*)v_pto_uint(reg.esp),
					v_pto_uint(reg.esp+4),
					v_pto_uint(reg.esp+8),
					(FILE*)v_pto_uint(reg.esp+12));
				reg.esp+=16;
				v_next_ins;
			case tins::c_fseek:
				fseek((FILE*)v_pto_uint(reg.esp),
					v_pto_int(reg.esp+4),
					v_pto_int(reg.esp+8));
				reg.esp+=12;
				v_next_ins;
			case tins::c_fseek8:
				rfile::fseek8((FILE*)v_pto_uint(reg.esp),
					v_pto_int8(reg.esp+4),
					v_pto_int(reg.esp+12));
				reg.esp+=16;
				v_next_ins;
			case tins::c_ftell:
				v_pto_uint(reg.esp+4)=ftell(
					(FILE*)v_pto_uint(reg.esp));
				reg.esp+=4;
				v_next_ins;
			case tins::c_ftell8:
				v_pto_int8(reg.esp+4)=rfile::ftell8(
					(FILE*)v_pto_uint(reg.esp));
				reg.esp+=4;
				v_next_ins;

			case tins::c_s_socket:
				v_pto_int(reg.esp+12)=rsock::s_socket(
					v_pto_int(reg.esp),
					v_pto_int(reg.esp+4),
					v_pto_int(reg.esp+8));
				reg.esp+=12;
				v_next_ins;
			case tins::c_s_connect:
				v_pto_int(reg.esp+12)=rsock::s_connect(
					v_pto_int(reg.esp),
					(void*)v_pto_int(reg.esp+4),
					v_pto_int(reg.esp+8));
				reg.esp+=12;
				v_next_ins;
			case tins::c_s_close:
				v_pto_int(reg.esp+4)=rsock::s_close(
					v_pto_int(reg.esp));
				reg.esp+=4;
				v_next_ins;
			case tins::c_s_send:
				v_pto_int(reg.esp+16)=rsock::s_send(
					v_pto_int(reg.esp),
					(void*)v_pto_int(reg.esp+4),
					v_pto_int(reg.esp+8),
					v_pto_int(reg.esp+12));
				reg.esp+=16;
				v_next_ins;
			case tins::c_s_recv:
				v_pto_int(reg.esp+16)=rsock::s_recv(
					v_pto_int(reg.esp),
					(void*)v_pto_int(reg.esp+4),
					v_pto_int(reg.esp+8),
					v_pto_int(reg.esp+12));
				reg.esp+=16;
				v_next_ins;
			case tins::c_s_bind:
				v_pto_int(reg.esp+12)=rsock::s_bind(
					v_pto_int(reg.esp),
					(void*)v_pto_int(reg.esp+4),
					v_pto_int(reg.esp+8));
				reg.esp+=12;
				v_next_ins;
			case tins::c_s_listen:
				v_pto_int(reg.esp+8)=rsock::s_listen(
					v_pto_int(reg.esp),
					v_pto_int(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			case tins::c_s_accept:
				v_pto_int(reg.esp+12)=rsock::s_accept(
					v_pto_int(reg.esp),
					(void*)v_pto_int(reg.esp+4),
					(int*)v_pto_int(reg.esp+8));
				reg.esp+=12;
				v_next_ins;

			case tins::c_gbk_to_utf8:
				v_pto_int(reg.esp+8)=rcode::gbk_to_utf8_c(
					(uchar*)v_pto_uint(reg.esp),
					(uchar*)v_pto_uint(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			case tins::c_gbk_to_utf16:
				v_pto_int(reg.esp+8)=rcode::gbk_to_utf16_c(
					(uchar*)v_pto_uint(reg.esp),
					(uchar*)v_pto_uint(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			case tins::c_utf8_to_gbk:
				v_pto_int(reg.esp+8)=rcode::utf8_to_gbk_c(
					(uchar*)v_pto_uint(reg.esp),
					(uchar*)v_pto_uint(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			case tins::c_utf8_to_utf16:
				v_pto_int(reg.esp+8)=rcode::utf8_to_utf16_c(
					(uchar*)v_pto_uint(reg.esp),
					(uchar*)v_pto_uint(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			case tins::c_utf16_to_gbk:
				v_pto_int(reg.esp+8)=rcode::utf16_to_gbk_c(
					(uchar*)v_pto_uint(reg.esp),
					(uchar*)v_pto_uint(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			case tins::c_utf16_to_utf8:
				v_pto_int(reg.esp+8)=rcode::utf16_to_utf8_c(
					(uchar*)v_pto_uint(reg.esp),
					(uchar*)v_pto_uint(reg.esp+4));
				reg.esp+=8;
				v_next_ins;
			
			case tins::c_CloseHandle:
#ifdef _MSC_VER
				v_pto_int(reg.esp+4)=CloseHandle(
					(HANDLE)v_pto_int(reg.esp));
#endif
				reg.esp+=4;
				v_next_ins;
			case tins::c_MessageBoxA:
#ifdef _MSC_VER
				MessageBoxA(0,"","",0);
#endif
				v_next_ins;
			case tins::c_itof8:
				v_pto_f8(reg.esp+4)=(double)v_pto_int(reg.esp);
				reg.esp+=4;
				v_next_ins;

			default:
				ifn(other_call(v_get_imme(cur->first),&sh,pvmt,reg))
				{
					return false;
				}
				v_next_ins;
			}
		case tins::c_calle_i+1:
		case tins::c_calle_i+2:
		case tins::c_calle_i+3:
		case tins::c_calle_i+4:
		case tins::c_calle_i+5:
			return false;

		case tins::c_call_i:
#ifdef RANALYSE
			tanalyse::analyse_add(v_get_imme(cur->first));
#endif
			reg.esp-=4;
			v_pto_uint(reg.esp)=reg.eip+r_size(tasm);
			reg.eip=v_get_imme(cur->first);
			goto next;
		case tins::c_call_r:
			reg.esp-=4;
			v_pto_uint(reg.esp)=reg.eip+r_size(tasm);
			reg.eip=v_get_reg(cur->first);
			goto next;
		case tins::c_call_a:
			reg.esp-=4;
			v_pto_uint(reg.esp)=reg.eip+r_size(tasm);
			reg.eip=v_get_addr(cur->first);
			goto next;
		case tins::c_call_a+1:
		case tins::c_call_a+2:
		case tins::c_call_a+3:
			return false;

		case tins::c_ret_n:
			reg.eip=v_pto_uint(reg.esp);
			reg.esp+=4;
			goto next;
		case tins::c_ret_n+1:
		case tins::c_ret_n+2:
		case tins::c_ret_n+3:
		case tins::c_ret_n+4:
		case tins::c_ret_n+5:
			return false;

		case tins::c_ret_i:
			reg.eip=v_pto_uint(reg.esp);
			reg.esp+=4+v_get_imme(cur->first);
			goto next;
		case tins::c_ret_r:
			reg.eip=v_pto_uint(reg.esp);
			reg.esp+=4+v_get_reg(cur->first);
			goto next;
		case tins::c_ret_a:
			reg.eip=v_pto_uint(reg.esp);
			reg.esp+=4;
			reg.esp+=v_get_addr(cur->first);
			goto next;
		case tins::c_ret_a+1:
		case tins::c_ret_a+2:
		case tins::c_ret_a+3:
			return false;

		case tins::c_push_i:
			reg.esp-=4;
			v_pto_uint(reg.esp)=v_get_imme(cur->first);
			v_next_ins;
		case tins::c_push_r:
			reg.esp-=4;
			v_pto_uint(reg.esp)=v_get_reg(cur->first);
			v_next_ins;
		case tins::c_push_a:
			reg.esp-=4;
			v_pto_uint(reg.esp)=v_get_addr(cur->first);
			v_next_ins;
		case tins::c_push_a+1:
		case tins::c_push_a+2:
		case tins::c_push_a+3:
			return false;

		case tins::c_pop_i:
			return false;
		case tins::c_pop_r:
			v_get_reg(cur->first)=v_pto_uint(reg.esp);
			reg.esp+=4;
			v_next_ins;
		case tins::c_pop_a:
			v_get_addr(cur->first)=v_pto_uint(reg.esp);
			reg.esp+=4;
			v_next_ins;
		case tins::c_pop_a+1:
		case tins::c_pop_a+2:
		case tins::c_pop_a+3:
			return false;

		case tins::c_jmp_i:
			reg.eip=v_get_imme(cur->first);
			goto next;
		case tins::c_jmp_r:
			reg.eip=v_get_reg(cur->first);
			goto next;
		case tins::c_jmp_a:
			reg.eip=v_get_addr(cur->first);
			goto next;
		case tins::c_jmp_a+1:
		case tins::c_jmp_a+2:
		case tins::c_jmp_a+3:
			return false;

		case tins::c_jebxz_i:
			if(reg.ebx)
				reg.eip+=r_size(tasm);
			else
				reg.eip=v_get_imme(cur->first);
			goto next;
		case tins::c_jebxz_r:
			if(reg.ebx)
				reg.eip+=r_size(tasm);
			else
				reg.eip=v_get_reg(cur->first);
			goto next;
		case tins::c_jebxz_a:
			if(reg.ebx)
				reg.eip+=r_size(tasm);
			else
				reg.eip=v_get_addr(cur->first);
			goto next;
		case tins::c_jebxz_a+1:
		case tins::c_jebxz_a+2:
		case tins::c_jebxz_a+3:
			return false;

		case tins::c_jebxnz_i:
			if(reg.ebx)
				reg.eip=v_get_imme(cur->first);
			else
				reg.eip+=r_size(tasm);
			goto next;
		case tins::c_jebxnz_r:
			if(reg.ebx)
				reg.eip=v_get_reg(cur->first);
			else
				reg.eip+=r_size(tasm);
			goto next;
		case tins::c_jebxnz_a:
			if(reg.ebx)
				reg.eip=v_get_addr(cur->first);
			else
				reg.eip+=r_size(tasm);
			goto next;
		case tins::c_jebxnz_a+1:
		case tins::c_jebxnz_a+2:
		case tins::c_jebxnz_a+3:
			return false;

		case tins::c_halt_n:
			break;
		case tins::c_halt_n+1:
		case tins::c_halt_n+2:
		case tins::c_halt_n+3:
		case tins::c_halt_n+4:
		case tins::c_halt_n+5:
			return false;

		case tins::c_nop_n:
			v_next_ins;
		case tins::c_nop_n+1:
		case tins::c_nop_n+2:
		case tins::c_nop_n+3:
		case tins::c_nop_n+4:
		case tins::c_nop_n+5:
			return false;

		case tins::c_lea_ri:
			return false;
		case tins::c_lea_ai:
			return false;
		case tins::c_lea_rr:
			return false;
		case tins::c_lea_ar:
			return false;
		case tins::c_lea_ra:
			v_get_reg(cur->first)=v_get_lea(cur->second);
			v_next_ins;
		case tins::c_lea_aa:
			v_get_addr(cur->first)=v_get_lea(cur->second);
			v_next_ins;

		case tins::c_mov_ri:
			v_get_reg(cur->first)=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_mov_ai:
			v_get_addr(cur->first)=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_mov_rr:
			v_get_reg(cur->first)=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_mov_ar:
			v_get_addr(cur->first)=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_mov_ra:
			v_get_reg(cur->first)=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_mov_aa:
			v_get_addr(cur->first)=v_get_addr(cur->second);
			v_next_ins;

		case tins::c_mov1_ri:
		case tins::c_mov1_ai:
		case tins::c_mov1_rr:
		case tins::c_mov1_ar:
		case tins::c_mov1_ra:
			return false;
		case tins::c_mov1_aa:
			v_get_addr_1(cur->first)=v_get_addr_1(cur->second);
			v_next_ins;

		case tins::c_mov8_ri:
		case tins::c_mov8_ai:
		case tins::c_mov8_rr:
		case tins::c_mov8_ar:
		case tins::c_mov8_ra:
			return false;
		case tins::c_mov8_aa:
			v_get_addr_8(cur->first)=v_get_addr_8(cur->second);
			v_next_ins;

		case tins::c_add_ri:
			v_get_reg(cur->first)+=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_add_ai:
			v_get_addr(cur->first)+=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_add_rr:
			v_get_reg(cur->first)+=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_add_ar:
			v_get_addr(cur->first)+=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_add_ra:
			v_get_reg(cur->first)+=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_add_aa:
			v_get_addr(cur->first)+=v_get_addr(cur->second);
			v_next_ins;

		case tins::c_sub_ri:
			v_get_reg(cur->first)-=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_sub_ai:
			v_get_addr(cur->first)-=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_sub_rr:
			v_get_reg(cur->first)-=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_sub_ar:
			v_get_addr(cur->first)-=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_sub_ra:
			v_get_reg(cur->first)-=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_sub_aa:
			v_get_addr(cur->first)-=v_get_addr(cur->second);
			v_next_ins;
		
		case tins::c_imul_ri:
			v_get_reg(cur->first)*=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_imul_ai:
			v_get_addr(cur->first)*=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_imul_rr:
			v_get_reg(cur->first)*=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_imul_ar:
			v_get_addr(cur->first)*=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_imul_ra:
			v_get_reg(cur->first)*=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_imul_aa:
			v_get_addr(cur->first)*=v_get_addr(cur->second);
			v_next_ins;

		case tins::c_idiv_ri:
			v_get_reg(cur->first)/=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_idiv_ai:
			v_get_addr(cur->first)/=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_idiv_rr:
			v_get_reg(cur->first)/=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_idiv_ar:
			v_get_addr(cur->first)/=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_idiv_ra:
			v_get_reg(cur->first)/=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_idiv_aa:
			v_get_addr(cur->first)/=v_get_addr(cur->second);
			v_next_ins;

		case tins::c_imod_ri:
			v_get_reg(cur->first)%=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_imod_ai:
			v_get_addr(cur->first)%=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_imod_rr:
			v_get_reg(cur->first)%=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_imod_ar:
			v_get_addr(cur->first)%=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_imod_ra:
			v_get_reg(cur->first)%=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_imod_aa:
			v_get_addr(cur->first)%=v_get_addr(cur->second);
			v_next_ins;	
		
		case tins::c_cesb_ri:
			reg.ebx=v_get_reg(cur->first)==v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cesb_ai:
			reg.ebx=v_get_addr(cur->first)==v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cesb_rr:
			reg.ebx=v_get_reg(cur->first)==v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cesb_ar:
			reg.ebx=v_get_addr(cur->first)==v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cesb_ra:
			reg.ebx=v_get_reg(cur->first)==v_get_addr(cur->second);
			v_next_ins;
		case tins::c_cesb_aa:
			reg.ebx=v_get_addr(cur->first)==v_get_addr(cur->second);
			v_next_ins;

		case tins::c_cnesb_ri:
			reg.ebx=v_get_reg(cur->first)!=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cnesb_ai:
			reg.ebx=v_get_addr(cur->first)!=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cnesb_rr:
			reg.ebx=v_get_reg(cur->first)!=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cnesb_ar:
			reg.ebx=v_get_addr(cur->first)!=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cnesb_ra:
			reg.ebx=v_get_reg(cur->first)!=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_cnesb_aa:
			reg.ebx=v_get_addr(cur->first)!=v_get_addr(cur->second);
			v_next_ins;
		
		case tins::c_cgsb_ri:
			reg.ebx=v_get_reg(cur->first)>v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cgsb_ai:
			reg.ebx=v_get_addr(cur->first)>v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cgsb_rr:
			reg.ebx=v_get_reg(cur->first)>v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cgsb_ar:
			reg.ebx=v_get_addr(cur->first)>v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cgsb_ra:
			reg.ebx=v_get_reg(cur->first)>v_get_addr(cur->second);
			v_next_ins;
		case tins::c_cgsb_aa:
			reg.ebx=v_get_addr(cur->first)>v_get_addr(cur->second);
			v_next_ins;

		case tins::c_cgesb_ri:
			reg.ebx=v_get_reg(cur->first)>=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cgesb_ai:
			reg.ebx=v_get_addr(cur->first)>=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cgesb_rr:
			reg.ebx=v_get_reg(cur->first)>=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cgesb_ar:
			reg.ebx=v_get_addr(cur->first)>=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cgesb_ra:
			reg.ebx=v_get_reg(cur->first)>=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_cgesb_aa:
			reg.ebx=v_get_addr(cur->first)>=v_get_addr(cur->second);
			v_next_ins;

		case tins::c_clsb_ri:
			reg.ebx=v_get_reg(cur->first)<v_get_imme(cur->second);
			v_next_ins;
		case tins::c_clsb_ai:
			reg.ebx=v_get_addr(cur->first)<v_get_imme(cur->second);
			v_next_ins;
		case tins::c_clsb_rr:
			reg.ebx=v_get_reg(cur->first)<v_get_reg(cur->second);
			v_next_ins;
		case tins::c_clsb_ar:
			reg.ebx=v_get_addr(cur->first)<v_get_reg(cur->second);
			v_next_ins;
		case tins::c_clsb_ra:
			reg.ebx=v_get_reg(cur->first)<v_get_addr(cur->second);
			v_next_ins;
		case tins::c_clsb_aa:
			reg.ebx=v_get_addr(cur->first)<v_get_addr(cur->second);
			v_next_ins;

		case tins::c_clesb_ri:
			reg.ebx=v_get_reg(cur->first)<=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_clesb_ai:
			reg.ebx=v_get_addr(cur->first)<=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_clesb_rr:
			reg.ebx=v_get_reg(cur->first)<=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_clesb_ar:
			reg.ebx=v_get_addr(cur->first)<=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_clesb_ra:
			reg.ebx=v_get_reg(cur->first)<=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_clesb_aa:
			reg.ebx=v_get_addr(cur->first)<=v_get_addr(cur->second);
			v_next_ins;
		
		case tins::c_clesb_aa+1:
		case tins::c_clesb_aa+2:
		case tins::c_clesb_aa+3:
		case tins::c_clesb_aa+4:
		case tins::c_clesb_aa+5:
			return false;
		case tins::c_add8_aa:
			v_get_addr_8(cur->first)+=v_get_addr_8(cur->second);
			v_next_ins;	
		
		case tins::c_add8_aa+1:
		case tins::c_add8_aa+2:
		case tins::c_add8_aa+3:
		case tins::c_add8_aa+4:
		case tins::c_add8_aa+5:
			return false;
		case tins::c_sub8_aa:
			v_get_addr_8(cur->first)-=v_get_addr_8(cur->second);
			v_next_ins;
		
		case tins::c_sub8_aa+1:
		case tins::c_sub8_aa+2:
		case tins::c_sub8_aa+3:
		case tins::c_sub8_aa+4:
		case tins::c_sub8_aa+5:
			return false;
		case tins::c_imul8_aa:
			v_get_addr_8(cur->first)*=v_get_addr_8(cur->second);
			v_next_ins;

		case tins::c_imul8_aa+1:
		case tins::c_imul8_aa+2:
		case tins::c_imul8_aa+3:
		case tins::c_imul8_aa+4:
		case tins::c_imul8_aa+5:
			return false;
		case tins::c_idiv8_aa:
			v_get_addr_8(cur->first)/=v_get_addr_8(cur->second);
			v_next_ins;

		case tins::c_idiv8_aa+1:
		case tins::c_idiv8_aa+2:
		case tins::c_idiv8_aa+3:
		case tins::c_idiv8_aa+4:
		case tins::c_idiv8_aa+5:
			return false;
		case tins::c_imod8_aa:
			v_get_addr_8(cur->first)%=v_get_addr_8(cur->second);
			v_next_ins;

		case tins::c_imod8_aa+1:
		case tins::c_imod8_aa+2:
		case tins::c_imod8_aa+3:
		case tins::c_imod8_aa+4:
		case tins::c_imod8_aa+5:
			return false;
		case tins::c_cgsb8_aa:
			reg.ebx=v_get_addr_8(cur->first)>v_get_addr_8(cur->second);
			v_next_ins;

		case tins::c_cgsb8_aa+1:
		case tins::c_cgsb8_aa+2:
		case tins::c_cgsb8_aa+3:
		case tins::c_cgsb8_aa+4:
		case tins::c_cgsb8_aa+5:
			return false;
		case tins::c_clsb8_aa:
			reg.ebx=v_get_addr_8(cur->first)<v_get_addr_8(cur->second);
			v_next_ins;
		
		case tins::c_clsb8_aa+1:
		case tins::c_clsb8_aa+2:
		case tins::c_clsb8_aa+3:
		case tins::c_clsb8_aa+4:
		case tins::c_clsb8_aa+5:
			return false;
		case tins::c_fadd8_aa:
			v_get_addr_f8(cur->first)+=v_get_addr_f8(cur->second);
			v_next_ins;	

		case tins::c_fadd8_aa+1:
		case tins::c_fadd8_aa+2:
		case tins::c_fadd8_aa+3:
		case tins::c_fadd8_aa+4:
		case tins::c_fadd8_aa+5:
			return false;
		case tins::c_fsub8_aa:
			v_get_addr_f8(cur->first)-=v_get_addr_f8(cur->second);
			v_next_ins;

		case tins::c_fsub8_aa+1:
		case tins::c_fsub8_aa+2:
		case tins::c_fsub8_aa+3:
		case tins::c_fsub8_aa+4:
		case tins::c_fsub8_aa+5:
			return false;
		case tins::c_fmul8_aa:
			v_get_addr_f8(cur->first)*=v_get_addr_f8(cur->second);
			v_next_ins;

		case tins::c_fmul8_aa+1:
		case tins::c_fmul8_aa+2:
		case tins::c_fmul8_aa+3:
		case tins::c_fmul8_aa+4:
		case tins::c_fmul8_aa+5:
			return false;
		case tins::c_fdiv8_aa:
			v_get_addr_f8(cur->first)/=v_get_addr_f8(cur->second);
			v_next_ins;

		case tins::c_fdiv8_aa+1:
		case tins::c_fdiv8_aa+2:
		case tins::c_fdiv8_aa+3:
		case tins::c_fdiv8_aa+4:
		case tins::c_fdiv8_aa+5:
			return false;
		case tins::c_fcgsb8_aa:
			reg.ebx=v_get_addr_f8(cur->first)>v_get_addr_f8(cur->second);
			v_next_ins;

		case tins::c_fcgsb8_aa+1:
		case tins::c_fcgsb8_aa+2:
		case tins::c_fcgsb8_aa+3:
		case tins::c_fcgsb8_aa+4:
		case tins::c_fcgsb8_aa+5:
			return false;
		case tins::c_fclsb8_aa:
			reg.ebx=v_get_addr_f8(cur->first)<v_get_addr_f8(cur->second);
			v_next_ins;
			
		case tins::c_band_ri:
			v_get_reg_u(cur->first)&=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_band_ai:
			v_get_addr_u(cur->first)&=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_band_rr:
			v_get_reg_u(cur->first)&=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_band_ar:
			v_get_addr_u(cur->first)&=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_band_ra:
			v_get_reg_u(cur->first)&=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_band_aa:
			v_get_addr_u(cur->first)&=v_get_addr_u(cur->second);
			v_next_ins;	

		case tins::c_bor_ri:
			v_get_reg_u(cur->first)|=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bor_ai:
			v_get_addr_u(cur->first)|=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bor_rr:
			v_get_reg_u(cur->first)|=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bor_ar:
			v_get_addr_u(cur->first)|=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bor_ra:
			v_get_reg_u(cur->first)|=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_bor_aa:
			v_get_addr_u(cur->first)|=v_get_addr_u(cur->second);
			v_next_ins;	

		case tins::c_bnot_i:
			return false;
		case tins::c_bnot_r:
			v_get_reg(cur->first)=~v_get_reg(cur->first);
			v_next_ins;	
		case tins::c_bnot_a:
			v_get_addr(cur->first)=~v_get_addr(cur->first);
			v_next_ins;
		case tins::c_bnot_a+1:
		case tins::c_bnot_a+2:
		case tins::c_bnot_a+3:
			return false;

		case tins::c_bxor_ri:
			v_get_reg_u(cur->first)^=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bxor_ai:
			v_get_addr_u(cur->first)^=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bxor_rr:
			v_get_reg_u(cur->first)^=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bxor_ar:
			v_get_addr_u(cur->first)^=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bxor_ra:
			v_get_reg_u(cur->first)^=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_bxor_aa:
			v_get_addr_u(cur->first)^=v_get_addr_u(cur->second);
			v_next_ins;	

		case tins::c_bshl_ri:
			v_get_reg_u(cur->first)<<=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bshl_ai:
			v_get_addr_u(cur->first)<<=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bshl_rr:
			v_get_reg_u(cur->first)<<=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bshl_ar:
			v_get_addr_u(cur->first)<<=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bshl_ra:
			v_get_reg_u(cur->first)<<=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_bshl_aa:
			v_get_addr_u(cur->first)<<=v_get_addr_u(cur->second);
			v_next_ins;	

		case tins::c_bshr_ri:
			v_get_reg_u(cur->first)>>=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bshr_ai:
			v_get_addr_u(cur->first)>>=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bshr_rr:
			v_get_reg_u(cur->first)>>=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bshr_ar:
			v_get_addr_u(cur->first)>>=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bshr_ra:
			v_get_reg_u(cur->first)>>=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_bshr_aa:
			v_get_addr_u(cur->first)>>=v_get_addr_u(cur->second);
			v_next_ins;	

		case tins::c_bsar_ri:
			v_get_reg(cur->first)>>=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_bsar_ai:
			v_get_addr(cur->first)>>=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_bsar_rr:
			v_get_reg(cur->first)>>=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_bsar_ar:
			v_get_addr(cur->first)>>=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_bsar_ra:
			v_get_reg(cur->first)>>=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_bsar_aa:
			v_get_addr(cur->first)>>=v_get_addr(cur->second);
			v_next_ins;	

		case tins::c_udiv_ri:
			v_get_reg_u(cur->first)/=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_udiv_ai:
			v_get_addr_u(cur->first)/=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_udiv_rr:
			v_get_reg_u(cur->first)/=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_udiv_ar:
			v_get_addr_u(cur->first)/=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_udiv_ra:
			v_get_reg_u(cur->first)/=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_udiv_aa:
			v_get_addr_u(cur->first)/=v_get_addr_u(cur->second);
			v_next_ins;

		case tins::c_umod_ri:
			v_get_reg_u(cur->first)%=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_umod_ai:
			v_get_addr_u(cur->first)%=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_umod_rr:
			v_get_reg_u(cur->first)%=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_umod_ar:
			v_get_addr_u(cur->first)%=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_umod_ra:
			v_get_reg_u(cur->first)%=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_umod_aa:
			v_get_addr_u(cur->first)%=v_get_addr_u(cur->second);
			v_next_ins;		

		case tins::c_ucgsb_ri:
			reg.ebx=v_get_reg_u(cur->first)>v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_ucgsb_ai:
			reg.ebx=v_get_addr_u(cur->first)>v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_ucgsb_rr:
			reg.ebx=v_get_reg_u(cur->first)>v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_ucgsb_ar:
			reg.ebx=v_get_addr_u(cur->first)>v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_ucgsb_ra:
			reg.ebx=v_get_reg_u(cur->first)>v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_ucgsb_aa:
			reg.ebx=v_get_addr_u(cur->first)>v_get_addr_u(cur->second);
			v_next_ins;

		case tins::c_ucgesb_ri:
			reg.ebx=v_get_reg_u(cur->first)>=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_ucgesb_ai:
			reg.ebx=v_get_addr_u(cur->first)>=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_ucgesb_rr:
			reg.ebx=v_get_reg_u(cur->first)>=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_ucgesb_ar:
			reg.ebx=v_get_addr_u(cur->first)>=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_ucgesb_ra:
			reg.ebx=v_get_reg_u(cur->first)>=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_ucgesb_aa:
			reg.ebx=v_get_addr_u(cur->first)>=v_get_addr_u(cur->second);
			v_next_ins;

		case tins::c_uclsb_ri:
			reg.ebx=v_get_reg_u(cur->first)<v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_uclsb_ai:
			reg.ebx=v_get_addr_u(cur->first)<v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_uclsb_rr:
			reg.ebx=v_get_reg_u(cur->first)<v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_uclsb_ar:
			reg.ebx=v_get_addr_u(cur->first)<v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_uclsb_ra:
			reg.ebx=v_get_reg_u(cur->first)<v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_uclsb_aa:
			reg.ebx=v_get_addr_u(cur->first)<v_get_addr_u(cur->second);
			v_next_ins;

		case tins::c_uclesb_ri:
			reg.ebx=v_get_reg_u(cur->first)<=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_uclesb_ai:
			reg.ebx=v_get_addr_u(cur->first)<=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_uclesb_rr:
			reg.ebx=v_get_reg_u(cur->first)<=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_uclesb_ar:
			reg.ebx=v_get_addr_u(cur->first)<=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_uclesb_ra:
			reg.ebx=v_get_reg_u(cur->first)<=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_uclesb_aa:
			reg.ebx=v_get_addr_u(cur->first)<=v_get_addr_u(cur->second);
			v_next_ins;

		default:
			return false;
		}
		if(poldreg!=null)
		{
			*poldreg=reg;
		}
		return true;
	}
	
	//查找DLL中的函数
	static void* find_dll(tvm_t* pvmt,const char* name)
	{
		if(pvmt->m_func.exist(name))
		{
			return pvmt->m_func[name];
		}
		void* ret=zjitf::find_dll_full(name);
		if(ret!=null)
		{
			pvmt->m_func[name]=ret;
		}
		return ret;
	}
	
	//调用外部函数
	static void stdcall_func(treg& reg)
	{
#ifdef _MSC_VER
#ifndef _WIN64
		uint addr=v_pto_uint(reg.esp);
		int count=v_pto_int(reg.esp+4);
		int tmp;
		for(int i=count-1;i>=0;i--)
		{
			tmp=v_pto_int(reg.esp+8+i*4);
			_asm push tmp
		}
		_asm call addr
		_asm mov tmp,eax
		v_pto_int(reg.esp+8+count*4)=tmp;
		reg.esp+=8+count*4;
#endif
#endif
	}
	
	//调用外部函数
	static void cdecl_func(treg& reg)
	{
#ifdef _MSC_VER
#ifndef _WIN64
		uint addr=v_pto_uint(reg.esp);
		int count=v_pto_int(reg.esp+4);
		int tmp;
		for(int i=count-1;i>=0;i--)
		{
			tmp=v_pto_int(reg.esp+8+i*4);
			_asm push tmp
		}
		_asm call addr
		_asm mov tmp,eax
		_asm mov eax,count
		_asm imul eax,4
		_asm add esp,eax
		v_pto_int(reg.esp+8+count*4)=tmp;
		reg.esp+=8+count*4;
#endif
#endif
	}

	rbool run(tsh& sh)
	{
		if(!init(sh))
		{
			rf::printl("main func init failed");
			return false;
		}
		if(!qrun(sh,&m_list[0],m_list[0].m_reg))
		{
			rstr("undefined ins").print();
			return false;
		}
#ifdef RANALYSE
		tanalyse::analyse_print(sh);
#endif
		return true;
	}

	static void init_reg(tsh& sh,tvm_t* pitem)
	{
		pitem->m_stack.realloc_n(rppconf(c_stack_size));
		pitem->m_reg.esp=r_to_uint(pitem->m_stack.end());
		pitem->m_reg.eip=r_to_uint(pitem->m_vasm.begin());
		pitem->m_meta.ptci=sh.m_main;
	}

	rbool init(tsh& sh)
	{
		tvm_t item;
		m_list.push(item);
		tvm_t* pitem=&m_list.top();
		ifn(main_init(sh,pitem->m_vasm))
			return false;
		init_reg(sh,pitem);
		pitem->m_pvm=this;
		pitem->m_thread=0;
		return true;
	}

	static rbool main_init(tsh& sh,rbuf<tasm>& vasm)
	{
		zasm::push_asm(vasm,"sub","esp",",","4");
		zasm::push_asm(vasm,"call",rppoptr(c_mbk_l),
			"&","main","main()",rppoptr(c_mbk_r));
		zasm::push_asm(vasm,"add","esp",",","4");
		zasm::push_asm(vasm,"halt");
		return zbin::proc_vasm(sh,vasm);
	}

	static rbool init_thr_ins(tsh& sh,rbuf<tasm>& vasm,void* start,void* param)
	{
		vasm.clear();
		zasm::push_asm(vasm,"push",rstr((uint)param));
		zasm::push_asm(vasm,"call",rstr((uint)start));
		zasm::push_asm(vasm,"halt");
		return zbin::proc_vasm(sh,vasm);
	}

	int create_thr(tsh& sh,void* start,void* param)
	{
		rmutex_t mutex_t(sh.m_mutex);
		tvm_t item;
		m_list.push(item);
		tvm_t* pitem=&m_list.top();
		ifn(init_thr_ins(sh,pitem->m_vasm,start,param))
		{
			return 0;
		}
		init_reg(sh,pitem);
		pitem->m_pvm=this;
		pitem->m_thread=rthread::create(thread,pitem);
		return pitem->m_thread;
	}

	//这里必须用裸线程
	static rthread_ret rthread_call thread(void* param)
	{
		tvm_t* pvmt=(tvm_t*)param;
		tvm* pvm=pvmt->m_pvm;
		ifn(qrun(pvm->m_sh,pvmt,pvmt->m_reg))
		{
			tsh::error("undefined ins");
		}
		pvm->m_sh.m_mutex.enter();
		pvm->m_list.erase(pvmt);
		pvm->m_sh.m_mutex.leave();
		return 0;
	}
};

#endif
