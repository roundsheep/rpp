#ifndef tconf_h__
#define tconf_h__

#include "../rlib/rstr.h"
#include "../rlib/ralgo.h"
#include "../rlib/rdic.h"

struct tconf
{
	rbuf<int> m_conf;

	int& operator[](int n) const
	{
		return m_conf[n];
	}

	enum
	{
		c_export_rbin,
		c_export_rasm,
		c_export_rp,
		c_display_inf,
		c_warning,
		c_auto_addinit,
		c_auto_addfunc,//自动增加构造析构等函数
		c_auto_import,
		c_auto_bbk,//是否控制结构自动加大括号
		c_op_empty_func,//优化基本数据类型空构造析构函数开关
		c_op_nop,
		c_op_zero,//优化add esp,0
		c_op_merge,//同一行连续加减同一个寄存器合并为一条指令
		c_op_ebp,//如果本函数未使用ebp可以不用push ebp节省3条指令
		c_op_base_calc,//优化基础运算，如调用int.opreator+可变成一条add指令
		c_op_pass,//优化参数传递，如传递int不需要调用拷贝构造函数
		c_op_const_eval,//常量表达式求值
		c_op_match,//汇编模式匹配优化
		c_run,
		c_pause,
		c_stack_size
	};
};

struct toptr
{
	rbuf<rstr> m_optr;//编译器使用
	rbuf<rstr> m_optr_s;//排好序的运算符，因为词法分析是贪心的
	rdic<int> m_dic;//字典中存放的是优先级，值小的优先
	int m_optr_max;

	toptr()
	{
		m_optr_max=0;
	}

	rstr& operator[](int n) const
	{
		return m_optr[n];
	}

	void sort_optr()
	{
		m_optr_s=m_optr;
		r_qsort(m_optr_s);
		m_optr_max=0;
		for (int i=0;i<m_optr_s.count();i++)
			if (m_optr_s[i].count()>m_optr_max)
				m_optr_max=m_optr_s[i].count();
	}

	int get_grade(const rstr& s) const
	{
		int* p=m_dic.find(s);
		if (p==null)
		{
			return 0;
		}
		return *p;
	}
	
	//比较两个运算符的优先级，true表示第一个优先于第二个，相同优先级总是从左到右计算
	rbool precede(const rstr& first,const rstr& second)
	{
		return get_grade(first)<=get_grade(second);
	}

	rbool is_optr(const rstr& s) const
	{
		return r_find_pos_b(m_optr_s,s)<m_optr_s.count();
	}

	enum
	{
		c_sbk_l,
		c_sbk_r,//()小括号
		c_mbk_l,
		c_mbk_r,//[]中括号
		c_bbk_l,
		c_bbk_r,//{}大括号
		
		c_arrow_r,//->右箭头
		c_dot,//.
		c_domain,//::作用域运算符
		c_semi,//;分号
		c_colon,//:冒号
		c_comma,//,
		c_pre,//预处理符号#

		c_addr,//&
		c_not,//!
		c_destruct,//~
		c_plusplus,//++
		c_minusminus,//--
		c_negative,
		
		c_star,//*
		c_divide,//除
		c_modulo,
		
		c_plus,//+
		c_minus,//-
		
		c_lessequal,
		c_greatequal,
		c_equalequal,
		c_notequal,

		c_tbk_l,
		c_tbk_r,//<>尖括号

		c_temp_51,
		c_temp_52,

		c_and3,
		c_or3,
		
		c_equal,//=
		c_arrow_l,//<-
		c_exchange,//<->
		c_temp_94,//+=
		c_temp_95,//-=
		c_temp_96,//*=
		c_temp_97,// /=
		c_temp_98,// %=
		c_temp_99,// ^
		c_temp_910,// |
		c_temp_911,// &=
		c_temp_912,// ^=
		c_temp_913,// |=

		c_exp_part,//表达式起止符号,使用一个根本不会出现的符号	
		c_at
	};
};

struct tkey
{
	rbuf<rstr> m_key;
	rdic<int> m_dic;

	rstr& operator[](int n) const
	{
		return m_key[n];
	}

	int count() const
	{
		return m_key.count();
	}

	void sort_asm()
	{
		m_dic.clear();
		for (int i=0;i<m_key.count();i++)
		{
			m_dic[m_key[i]]=i;
		}
	}

	int get_key_index(const rstr& s)
	{
		int* p=m_dic.find(s);
		if (p==null)
		{
			return m_key.count();
		}
		return *p;
	}

	rbool is_asm_ins(const rstr& s) const
	{
		int* p=m_dic.find(s);
		if (p==null)
		{
			return false;
		}
		int index=*p;
		return index>=0&&index<=c_rn;
	}

	rbool is_asm_reg(const rstr& s) const
	{
		for (int i=c_eip;i<=c_edi;++i)
			if (s==m_key[i])
				return true;
		return false;
	}

	enum
	{
		c_rjit,

		c_calle,
		c_call,
		c_retn,
		c_reti,

		c_push,
		c_pop,

		c_jmp,
		c_jebxz,//如果ebx等于0则跳转
		c_jebxnz,
		
		c_halt,
		c_nop,
		
		c_lea,
		c_mov,
		c_mov1,
		c_mov8,

		c_add,
		c_sub,
		c_imul,
		c_idiv,
		c_imod,
		
		c_cesb,
		c_cnesb,
		c_cgsb,//cmp great set ebx带符号设置指令，大于则置1，否则置0
		c_cgesb,
		c_clsb,
		c_clesb,

		c_add8,
		c_sub8,
		c_imul8,
		c_idiv8,
		c_imod8,
		c_cgsb8,
		c_clsb8,

		c_fadd8,
		c_fsub8,
		c_fmul8,
		c_fdiv8,
		c_fcgsb8,
		c_fclsb8,

		c_band,
		c_bor,
		c_bnot,
		c_bxor,
		c_bshl,
		c_bshr,
		c_bsar,

		c_udiv,
		c_umod,
		c_ucgsb,
		c_ucgesb,
		c_uclsb,
		c_uclesb,

		c_rn,

		c_rd1,
		c_rd2,
		c_rd4,
		c_rd8,
		c_rdp,

		c_eip,
		c_esp,
		c_ebp,
		c_eax,
		c_ebx,
		c_ecx,
		c_edx,
		c_esi,
		c_edi,
		c_eflags,

		c_void,
		c_char,
		c_int,
		c_uint,
		c_int8,
		c_double,
		c_rp,
		c_var,
		c_rbuf,
		c_rstr,
		c_rcs,//RPP代码类型
		
		c_js_func,//JS函数
		c_js_obj,
		c_js_findf,

		c_if,
		c_ifn,//与unless功能一样,感觉ifnot或者ifn更清晰
		c_else,
		c_break,
		c_continue,
		c_continued,
		c_return,
		c_class,
		c_template,
		c_this,
		c_sizeof,
		c_main,
		c_include,
		c_import,
		c_endif,
		c_ifdef,
		c_ifndef,
		c_define,
		c_enum,
		c_for,
		c_to,
		c_in,
		c_case,
		c_friend,
		c_private,
		c_protected,
		c_public,
		c_switch,
		c_typename,
		c_typeof,
		c_s_ret,
		c_s_off,
		c_s_param,
		c_s_local,
		c_mac,//宏
		c_export,
		c_lambda,
		c_self,
		c_btemp,//构造临时对象
		c_pcall,//函数指针调用
		c_LAMBDA,
		c_SELF,
		c_rvm,
		c_rvara,
		c_rvarb,
		c_temp,
		c_plambda,
		c_pmain,//全局数据的引用
	};
};

#endif
