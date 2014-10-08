#ifndef tvm_struct_h__
#define tvm_struct_h__

#include "../rlib/rstr.h"
#include "rpp_def.h"

#pragma pack(4)

struct treg
{
	union
	{
		//int64 rip;
		uint eip;
	};
	union
	{
		//int64 rsp;
		uint esp;
	};
	union
	{
		//int64 rbp;
		uint ebp;
	};
	union
	{
		//int64 rsi;
		uint esi;
	};
	union
	{
		//int64 rdi;
		uint edi;
	};
	union
	{
		//int64 rax;
		uint eax;
	};
	union
	{
		//int64 rbx;
		uint ebx;
	};
	union
	{
		//int64 rcx;
		uint ecx;
	};
	union
	{
		//int64 rdx;
		uint edx;
	};

	enum 
	{
		c_esp=4,
		c_ebp=8,
		c_esi=12,
		c_edi=16,
		c_eax=20,
		c_ebx=24,
		c_ecx=28,
		c_edx=32
	};
};

struct topnd
{
	int type;
	int off;
	int val;

	enum
	{
		c_null,
		c_reg,//ebp
		c_imme,//4
		c_addr,//[ebp+4]
	};

	topnd()
	{
		clear();
	}

	void clear()
	{
		type=c_null;
		off=0;
		val=0;
	}

	rbool empty()
	{
		return type==c_null;
	}
};

struct tins
{
	int type;
	topnd first;
	topnd second;

	tins()
	{
		clear();
	}

	void clear()
	{
		type=c_rjit_n;
		first.clear();
		second.clear();
	}

	rbool empty()
	{
		return type==c_rjit_n;
	}

	enum
	{
		c_rjit_n,

		c_calle_i=c_rjit_n+6,

		c_call_i=c_calle_i+6,
		c_call_r,
		c_call_a,

		c_ret_n=c_call_i+6,

		c_ret_i=c_ret_n+6,
		c_ret_r,
		c_ret_a,

		c_push_i=c_ret_i+6,
		c_push_r,
		c_push_a,

		c_pop_i=c_push_i+6,
		c_pop_r,
		c_pop_a,

		c_jmp_i=c_pop_i+6,
		c_jmp_r,
		c_jmp_a,

		c_jebxz_i=c_jmp_i+6,
		c_jebxz_r,
		c_jebxz_a,

		c_jebxnz_i=c_jebxz_i+6,
		c_jebxnz_r,
		c_jebxnz_a,

		c_halt_n=c_jebxnz_i+6,

		c_nop_n=c_halt_n+6,

		c_lea_ri=c_nop_n+6,
		c_lea_ai,
		c_lea_rr,
		c_lea_ar,
		c_lea_ra,
		c_lea_aa,

		c_mov_ri,
		c_mov_ai,
		c_mov_rr,
		c_mov_ar,
		c_mov_ra,
		c_mov_aa,

		c_mov1_ri,
		c_mov1_ai,
		c_mov1_rr,
		c_mov1_ar,
		c_mov1_ra,
		c_mov1_aa,

		c_mov8_ri,
		c_mov8_ai,
		c_mov8_rr,
		c_mov8_ar,
		c_mov8_ra,
		c_mov8_aa,

		c_add_ri,
		c_add_ai,
		c_add_rr,
		c_add_ar,
		c_add_ra,
		c_add_aa,

		c_sub_ri,
		c_sub_ai,
		c_sub_rr,
		c_sub_ar,
		c_sub_ra,
		c_sub_aa,
		
		c_imul_ri,
		c_imul_ai,
		c_imul_rr,
		c_imul_ar,
		c_imul_ra,
		c_imul_aa,

		c_idiv_ri,
		c_idiv_ai,
		c_idiv_rr,
		c_idiv_ar,
		c_idiv_ra,
		c_idiv_aa,

		c_imod_ri,
		c_imod_ai,
		c_imod_rr,
		c_imod_ar,
		c_imod_ra,
		c_imod_aa,

		c_cesb_ri,
		c_cesb_ai,
		c_cesb_rr,
		c_cesb_ar,
		c_cesb_ra,
		c_cesb_aa,

		c_cnesb_ri,
		c_cnesb_ai,
		c_cnesb_rr,
		c_cnesb_ar,
		c_cnesb_ra,
		c_cnesb_aa,

		c_cgsb_ri,
		c_cgsb_ai,
		c_cgsb_rr,
		c_cgsb_ar,
		c_cgsb_ra,
		c_cgsb_aa,

		c_cgesb_ri,
		c_cgesb_ai,
		c_cgesb_rr,
		c_cgesb_ar,
		c_cgesb_ra,
		c_cgesb_aa,

		c_clsb_ri,
		c_clsb_ai,
		c_clsb_rr,
		c_clsb_ar,
		c_clsb_ra,
		c_clsb_aa,

		c_clesb_ri,
		c_clesb_ai,
		c_clesb_rr,
		c_clesb_ar,
		c_clesb_ra,
		c_clesb_aa,

		c_add8_aa=c_clesb_ri+6+5,
		c_sub8_aa=c_add8_aa+6,
		c_imul8_aa=c_sub8_aa+6,
		c_idiv8_aa=c_imul8_aa+6,
		c_imod8_aa=c_idiv8_aa+6,
		c_cgsb8_aa=c_imod8_aa+6,
		c_clsb8_aa=c_cgsb8_aa+6,

		c_fadd8_aa=c_clsb8_aa+6,
		c_fsub8_aa=c_fadd8_aa+6,
		c_fmul8_aa=c_fsub8_aa+6,
		c_fdiv8_aa=c_fmul8_aa+6,
		c_fcgsb8_aa=c_fdiv8_aa+6,
		c_fclsb8_aa=c_fcgsb8_aa+6,

		c_band_ri,
		c_band_ai,
		c_band_rr,
		c_band_ar,
		c_band_ra,
		c_band_aa,

		c_bor_ri,
		c_bor_ai,
		c_bor_rr,
		c_bor_ar,
		c_bor_ra,
		c_bor_aa,

		c_bnot_i,
		c_bnot_r,
		c_bnot_a,

		c_bxor_ri=c_bnot_i+6,
		c_bxor_ai,
		c_bxor_rr,
		c_bxor_ar,
		c_bxor_ra,
		c_bxor_aa,

		c_bshl_ri,
		c_bshl_ai,
		c_bshl_rr,
		c_bshl_ar,
		c_bshl_ra,
		c_bshl_aa,

		c_bshr_ri,
		c_bshr_ai,
		c_bshr_rr,
		c_bshr_ar,
		c_bshr_ra,
		c_bshr_aa,

		c_bsar_ri,
		c_bsar_ai,
		c_bsar_rr,
		c_bsar_ar,
		c_bsar_ra,
		c_bsar_aa,

		c_udiv_ri,
		c_udiv_ai,
		c_udiv_rr,
		c_udiv_ar,
		c_udiv_ra,
		c_udiv_aa,

		c_umod_ri,
		c_umod_ai,
		c_umod_rr,
		c_umod_ar,
		c_umod_ra,
		c_umod_aa,
	
		c_ucgsb_ri,
		c_ucgsb_ai,
		c_ucgsb_rr,
		c_ucgsb_ar,
		c_ucgsb_ra,
		c_ucgsb_aa,

		c_ucgesb_ri,
		c_ucgesb_ai,
		c_ucgesb_rr,
		c_ucgesb_ar,
		c_ucgesb_ra,
		c_ucgesb_aa,

		c_uclsb_ri,
		c_uclsb_ai,
		c_uclsb_rr,
		c_uclsb_ar,
		c_uclsb_ra,
		c_uclsb_aa,

		c_uclesb_ri,
		c_uclesb_ai,
		c_uclesb_rr,
		c_uclesb_ar,
		c_uclesb_ra,
		c_uclesb_aa,
	};

	enum
	{
		c_eval_txt,
		c_cp_txt,
		c_find_meta,
		c_find_func,
		c_find_dll,
		c_stdcall_func,
		c_cdecl_func,
		c_get_vclass,

		c_thr_create,
		c_thr_wait,

		c_mu_init,
		c_mu_del,
		c_mu_enter,
		c_mu_leave,

		c_execmd,
		c_srand,
		c_rand,
		c_gettick,
		c_sleep,

		c_puts,
		c_puts_l,
		c_getch,
		c_getstdin,
		c_getstdout,
		c_get_argc,
		c_get_argv,

		c_sprintf,
		c_sprintf8,
		c_sscanf,

		c_malloc,
		c_mfree,
		c_memset,
		c_memcpy,
		
		c_strcpy,
		c_strcmp,
		c_strlen,
		
		c_fopen_w,
		c_fopen,
		c_fclose,
		c_fread,
		c_fwrite,
		c_fseek,
		c_fseek8,
		c_ftell,
		c_ftell8,

		c_s_socket,
		c_s_connect,
		c_s_close,
		c_s_send,
		c_s_recv,
		c_s_bind,
		c_s_listen,
		c_s_accept,
		
		c_gbk_to_utf8,
		c_gbk_to_utf16,
		c_utf8_to_gbk,
		c_utf8_to_utf16,
		c_utf16_to_gbk,
		c_utf16_to_utf8,
		
		c_CloseHandle,
		c_MessageBoxA,
		c_itof8,
	};

	enum
	{
		c_cc_set_effect_volume=1000,
		c_cc_set_music_volume,
		c_cc_play_effect,
		c_cc_play_music,

		c_cc_add_animations,
		c_cc_get_animation,
		c_cc_animate_create,

		c_cc_get_key,
		c_cc_set_key,
		c_cc_add_cache,
		c_cc_rand,
		c_cc_set_design_size,
		c_cc_set_display_state,
		c_cc_jump_scene,
		c_cc_touch_call,

		c_cc_sprite_create,
		c_cc_bmf_create,
		c_cc_bmf_set,
		c_cc_get_size,
		c_cc_set_tag,
		c_cc_get_tag,
		c_cc_add_child,
		c_cc_remove_from_parent,
		c_cc_set_pos,
		c_cc_get_pos,
		c_cc_set_scale,
		c_cc_run_ac,
		c_cc_stop_ac,
		c_cc_schedule,

		c_cc_ac_move_to,
		c_cc_ac_move_by,
		c_cc_ac_rotate_to,
		c_cc_ac_rotate_by,
		c_cc_ac_scale_to,
		c_cc_ac_scale_by,
		c_cc_ac_delay,
		c_cc_ac_repeat,
		c_cc_ac_repeat_times,
		c_cc_ac_sequence,
		c_cc_ac_spawn,
		c_cc_ac_call,
		c_cc_ac_ease_in,
		c_cc_ac_ease_out,
		c_cc_ac_elastic_in,
		c_cc_ac_elastic_out,

		c_cc_phy_apply_impulse,
		c_cc_phy_get_body,
		c_cc_phy_set_body,
		c_cc_phy_box,
		c_cc_phy_box_edge,
		c_cc_phy_call,

		c_cc_label_create,
		c_cc_set_color,
	};
};

struct tfile;

struct tpos
{
	tfile* file;
	int line;

	tpos()
	{
		clear();
	}

	void clear()
	{
		file=null;
		line=0;
	}

	rbool empty()
	{
		return file==null;
	}

	friend rbool operator==(const tpos& a,const tpos& b)
	{
		return a.file==b.file&&a.line==b.line;
	}

	friend rbool operator!=(const tpos& a,const tpos& b)
	{
		return a.file!=b.file||a.line!=b.line;
	}

	void printl()
	{
		rstr((uint)file).printl();
		rstr(line).printl();
	}
};

struct tfunc;

struct tasm
{
	tins ins;//tins放在最前面便于把指针转换为tasm*
	rbuf<rstr> vstr;
	tpos pos;
	tfunc* ptfi;//反射到tfunc，不用初始化
	uchar* start;//jit地址

	tasm()
	{
		clear();
	}

	tasm(tasm&& a)
	{
		ins=a.ins;
		vstr=a.vstr;
		pos=a.pos;
		ptfi=a.ptfi;
		start=a.start;
	}

	void operator=(tasm&& a)
	{
		ins=a.ins;
		vstr=a.vstr;
		pos=a.pos;
		ptfi=a.ptfi;
		start=a.start;
	}

	void clear()
	{
		ins.clear();
		vstr.clear();
		pos.clear();
		ptfi=null;
		start=null;
	}

	void print()
	{
		vstr.print();
	}
};

//优化节点
struct top_node
{
	rbuf<tasm> src;
	rbuf<tasm> dst;

	void clear()
	{
		src.clear();
		dst.clear();
	}
};

#endif
