#ifndef zsent_h__
#define zsent_h__

#include "zlambda.h"

//运算符替换成标准函数调用的S表达式
//即[ class func []]形式，例如[ rf print []]
//从这里开始行号已经放在sent中
struct zsent
{
	static rbool proc_func(tsh& sh,tfunc& tfi)
	{
		ifn(zlambda::function_replace(sh,tfi.vword))
		{
			return false;
		}
		zlambda::lambda_var_replace(sh,tfi);
		ifn(zlambda::lambda_replace(sh,tfi))
		{
			return false;
		}
		zctl::sbk_line_proc(sh,tfi.vword);
		if(!zmac::replace(sh,*tfi.ptci,tfi.vword))
			return false;
		if(!zftl::ftl_replace(sh,*tfi.ptci,tfi.vword,null))
			return false;
		if(!zcontrol::proc_func(sh,tfi))
			return false;
		ifn(zmac::func_mac_replace(sh,tfi.vsent))
			return false;
		zlambda::lambda_proc(sh,tfi);
		//增加全局变量引用
		add_main_quote(sh,tfi);
		zsrep::neg_replace(sh,tfi.vsent);
		//常量dot替换成临时变量
		//如 0.toint -> int(0).toint
		// "abc"+a  ->  rstr("abc")+a
		zsrep::const_replace(sh,tfi.vsent);
		//找到直接定义的局部变量
		zsrep::local_var_replace(sh,tfi);
		int tid=0;
		//函数指针常量替换
		zsrep::fpoint_replace(sh,tfi);
		//sizeof s_off替换成0，稍后处理
		if(!zsrep::size_off_to_zero(sh,tfi))
			return false;
		//进行类型推断
		if(!proc_type_infer(sh,tfi))
			return false;
		if(!zsrep::typeof_replace(sh,tfi))
			return false;
		//变量构造函数替换如a(1) -> int.int(a,1)，这个变量必须是已定义的，不能是类型推断
		if(!zsrep::var_struct_replace(sh,tfi))
			return false;
		//元函数替换
		if(!zself::self_replace(sh,tfi))
			return false;
		//表达式标准化
		if(!zexp::p_exp_all(sh,tfi))
		{
			return false;
		}
		//临时变量替换，替换后有些表达式类型会变空
		if(!replace_temp_var(sh,tfi,tid))
			return false;
		//获取局部变量偏移
		obtain_local_off(sh,tfi.local);
		obtain_param_off(tfi);
		//增加局部变量和成员变量的构造和析构
		zadd::add_local_and_memb(sh,tfi);
		//增加全局变量初始化汇编
		add_main_init_asm(sh,tfi);
		zlambda::lambda_add_init_asm(sh,tfi);
		//sizeof s_off替换成实际值
		if(!zsrep::size_off_to_zero(sh,tfi))
			return false;
		if(!zsrep::size_off_replace(sh,tfi))
			return false;
		//汇编语句常量求值
		if(!asm_const_eval(sh,tfi))
			return false;
		if(!zmac::replace(sh,*tfi.ptci,tfi.vword))
			return false;
		//成员变量里有sizeof s_off的情况需要再次替换
		//注意成员变量初始化的时候不能使用临时变量
		//再处理一次，获取所有表达式的类型
		if(!zexp::p_exp_all(sh,tfi))
			return false;
		return true;
	}

	//增加全局变量引用的初始化汇编语句
	static void add_main_init_asm(tsh& sh,tfunc& tfi)
	{
		if(tfi.name!="main")
		{
			int i;
			for(i=0;i<tfi.local.count();i++)
			{
				if(tfi.local[i].type=="main&"&&tfi.local[i].name==rppkey(c_pmain))
				{
					break;
				}
			}
			if(i>=tfi.local.count())
			{
				return;
			}
		}
		tsent sent;
		sent.pos=tfi.first_pos;
		sent.vword.push(tword("mov"));//tword没有设置pos
		sent.vword.push(tword(rppkey(c_pmain)));
		sent.vword.push(tword(","));
		if(sh.m_mode==tsh::c_rvm||sh.m_mode==tsh::c_jit)
		{
			sent.vword.push(tword(rstr((uint)sh.m_main_data.begin())));
		}
		else
		{
			sent.vword.push(tword("_PMAIN_A"));
		}
		tfi.vsent.push_front(sent);
	}

	//全局变量和成员变量都不能使用类型推断
	//全局变量必须用g_开头
	static void add_main_quote(tsh& sh,tfunc& tfi)
	{
		ifn(tfi.ptci->name==rppkey(c_main)&&tfi.name==rppkey(c_main))
		{
			int i;
			for(i=0;i<tfi.vword.count();i++)
			{
				if(zfind::data_member_search(*sh.m_main,tfi.vword[i].val)!=null)
				{
					break;
				}
			}
			if(i>=tfi.vword.count())
			{
				return;
			}
		}
		tdata tdi;
		tdi.type="main&";
		tdi.name=rppkey(c_pmain);
		tdi.size=4;
		tdi.count=1;
		tfi.local.push(tdi);
	}

	static rbool asm_const_eval(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();++i)
		{
			if(sh.m_key.is_asm_ins(tfi.vsent[i].vword.get_bottom().val))
			{
				if(!zpre::op_const_eval(sh,tfi.vsent[i].vword,true))
					return false;
			}
		}
		return true;
	}

	static void obtain_local_off(tsh& sh,rbuf<tdata>& local)
	{
		int off=c_point_size;
		for(int i=0;i<local.count();++i)
		{
			local[i].size=zfind::get_type_size(sh,local[i].type);
			local[i].off=off;
			off+=zfind::get_ceil_space(local[i]);
		}
	}

	static void obtain_param_off(tfunc& tfi)
	{
		int off=zfind::get_func_local_size(tfi);
		off+=c_point_size;//ebp占用的
		off+=c_point_size;//返回地址占用的
		for(int i=0;i<tfi.param.count();++i)
		{
			tfi.param[i].off=off;
			off+=zfind::get_ceil_space(tfi.param[i]);
		}
		tfi.retval.off=off;
	}

	static rbool proc_type_infer(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();++i)
			if(!proc_type_infer(sh,tfi.vsent[i],tfi))
				return false;
		zcontrol::part_vsent(tfi);
		return true;
	}

	static rbool proc_type_infer(tsh& sh,tsent& sent,tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		if(sent.vword.count()>=3&&
			sent.vword[1].val==sh.m_optr[toptr::c_equal]&&
			sent.vword[0].is_name())
		{
			//处理类型推断
			rstr name=sent.vword[0].val;
			if(null!=zfind::local_search(tfi,name))
				return true;
			if(null!=zfind::data_member_search(tci,name))
				return true;
			if(null!=zfind::data_member_search(*sh.m_main,name))
				return true;
			tsent temp=sent.sub(2,sent.vword.count());
			if(!zsrep::typeof_replace(sh,tfi,temp))
			{
				return false;
			}
			if(!zexp::p_exp(sh,temp,tfi))
				return false;
			tdata tdi;
			tdi.name=name;
			tdi.type=sh.get_tname(temp.type);//引用推断为对象
			
			tfi.local.push(tdi);
			rbuf<tword> vtemp;
			zadd::add_destructor_func(sh,tdi,vtemp);
			zadd::add_structor_func(sh,tdi,vtemp);
			vtemp+=sent.vword;
			sent.vword=vtemp;
		}
		return true;
	}

	static rbool replace_temp_var_v(tsh& sh,tfunc& tfi,tsent& sent,int& tid)
	{
		rbuf<tword>& v=sent.vword;
		rbuf<tword> result;
		for(int i=1;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_btemp))
			{
				continue;
			}
			tclass* ptci=zfind::class_search(sh,v.get(i+1).val);
			if(null==ptci)
				continue;
			int left=i-1;
			if(v[left].val!=rppoptr(c_mbk_l))
			{
				continue;
			}
			int right=sh.find_symm_mbk(v,left);
			if(right>=v.count())
			{
				sh.error(sent,"miss ]");
				return false;
			}
			tdata tdi;
			tdi.type=ptci->name;
			tdi.name=rppkey(c_temp)+rstr(tid++);
			//先析构后构造
			zadd::add_destructor_func(sh,tdi,result);
			result+=rppoptr(c_mbk_l);
			result+=tdi.type;
			result+=tdi.type;
			result+=rppoptr(c_mbk_l);
			result+=tdi.name;
			for(int j=left+4;j<right-1;j++)
			{
				result+=v[j];
			}
			result+=rppoptr(c_mbk_r);
			result+=rppoptr(c_mbk_r);
			result+=rppoptr(c_semi);
			tfi.local.push(tdi);
			for(int j=left;j<=right;j++)
			{
				v[j].clear();
			}
			v[left].val=tdi.name;
			i=right;
		}
		zpre::arrange(v);
		result+=v;
		v=result;
		return true;
	}

	static rbool replace_temp_var(tsh& sh,tfunc& tfi,int& tid)
	{
		for(int i=0;i<tfi.vsent.count();++i)
		{
			if(sh.m_key.is_asm_ins(tfi.vsent[i].vword.get_bottom().val))
				continue;
			for(int j=0;j<c_rpp_deep;j++)//最多150层嵌套
			{
				//刚好是先构造内层的临时变量，再构造外层的临时变量
				//这种循环替换的方式好像比递归更清晰
				int temp=tid;
				if(!replace_temp_var_v(sh,tfi,tfi.vsent[i],tid))
					return false;
				if(temp==tid)
					break;
			}
		}
		zcontrol::part_vsent(tfi);
		return true;
	}
};

#endif