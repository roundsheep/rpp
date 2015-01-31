#ifndef zfind_h__
#define zfind_h__

#include "tsh.h"

//查找类
struct zfind
{
	//获取类型大小
	static int get_type_size(tsh& sh,const rstr& s)
	{
		if(sh.is_point_quote(s))
			return c_point_size;
		tclass* ptci=class_search_t(sh,s);
		if(ptci!=null)
			return ptci->size;
		return 0;
	}

	static int get_func_local_size(const tfunc& tfi)
	{
		int ret=0;
		for(int i=0;i<tfi.local.count();++i)
			ret+=get_ceil_space(tfi.local[i]);
		return ret;
	}

	static int get_func_param_size(const tfunc& tfi)
	{
		int ret=0;
		for(int i=0;i<tfi.param.count();++i)
			ret+=get_ceil_space(tfi.param[i]);
		return ret;
	}

	//不包括返回值
	static int get_func_stack_size(const tfunc& tfi)
	{
		return get_func_local_size(tfi)+get_func_param_size(tfi)+c_point_size*2;
	}

	//32位栈空间按4字节排列，64位栈空间按8字节排列，因为amd64没有32位push指令
	static int get_ceil_space(const tdata& tdi)
	{
		return r_ceil_div(tdi.size,c_point_size)*c_point_size;
	}

	static rbool is_type_mebx(tsh& sh,const rstr& type)
	{
		//这里不用is_point_quote可略微提高效率
		return type==rppkey(c_int)||sh.is_point(type)||sh.is_quote(type);
	}

	static rbool is_op_pass_type(tsh& sh,const rstr& s)
	{
		return s==rppkey(c_int)||s==rppkey(c_uint)||sh.is_point(s);
	}

	static rbool is_rstr_optr(tsh& sh,const rstr& s)
	{
		return s==rppoptr(c_plus)||s==rppoptr(c_equalequal)||
			s==rppoptr(c_notequal)||s==rppoptr(c_greatequal)||
			s==rppoptr(c_lessequal);
		//大于和小于最好不要加上，防止与尖括号作用冲突
	}

	static rbool is_end_part_optr(tsh& sh,const rstr& s)
	{
		if(!sh.m_optr.is_optr(s))
			return false;
		return s==rppoptr(c_dot)||s==rppoptr(c_sbk_r)||
			s==rppoptr(c_mbk_r)||s==rppoptr(c_plus);
		//还有很多这样的运算符，暂时不写
	}

	static rbool is_empty_struct_type(tsh& sh,const rstr& s)
	{
		return s==rppkey(c_int)||s==rppkey(c_char)||
			s==rppkey(c_uint)||s==rppkey(c_int8)||
			s==rppkey(c_double)||s==rppkey(c_rd4)||
			s=="float"||sh.is_point_quote(s);
	}

	static rbool is_class(tsh& sh,const rstr& s)
	{
		return class_search(sh,s)!=null;
	}

	static rbool is_class_t(tsh& sh,const rstr& s)
	{
		return class_search_t(sh,s)!=null;
	}

	static rbool is_classtl(tsh& sh,const rstr& s)
	{
		return classtl_search(sh,s)!=null;
	}

	static tclass* class_search_t(tsh& sh,const rstr& name)
	{
		return class_search(sh,sh.get_tname(name));
	}

	static tclass* class_search(tsh& sh,const rstr& name)
	{
		tclass item;
		item.name=name;
		return sh.m_class.find(item);
	}

	static tclass* classtl_search(tsh& sh,const rstr& name)
	{
		tclass item;
		item.name=name;
		return sh.m_classtl.find(item);
	}

	//先搜索友元空参，再搜索非友元空参
	static tfunc* func_search_empty_param_m(tclass& tci,
		const rstr& fname,rbool is_friend)
	{
		//友元只能调用友元
		if(is_friend)
			return func_search_empty_param_f(tci,fname);
		tfunc* ptfi=func_search_empty_param_f(tci,fname);
		if(null==ptfi)
			ptfi=func_search_empty_param_n(tci,fname);
		return ptfi;
	}
	
	//先搜索友元多参，再搜索非友元多参
	static tfunc* func_search_multi_param_m(tclass& tci,
		const rstr& fname,rbool is_friend)
	{
		if(is_friend)
			return func_search_multi_param_f(tci,fname);
		tfunc* ptfi=func_search_multi_param_f(tci,fname);
		if(null==ptfi)
			ptfi=func_search_multi_param_n(tci,fname);
		return ptfi;
	}
	
	//搜索非友元参数大于等于1个的函数（实际大于等于2）
	static tfunc* func_search_multi_param_n(tclass& tci,const rstr& fname)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if(fname==p->name&&
				!p->is_friend&&
				p->param.count()>=2&&
				p->param[0].type==tci.name+rstr("&"))
				return p;
		}
		return null;
	}

	//搜索非友元空参函数（类成员空参函数）
	static tfunc* func_search_empty_param_n(tclass& tci,const rstr& fname)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if(fname==p->name&&
				!p->is_friend&&
				p->param.count()==1&&
				p->param[0].type==tci.name+rstr("&"))
				return p;
		}
		return null;
	}

	//搜索友元参数大于等于1个的函数
	static tfunc* func_search_multi_param_f(tclass& tci,const rstr& fname)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if(fname==p->name&&
				p->is_friend&&
				p->param.count()>=1)
				return p;
		}
		return null;
	}

	//搜索友元空参函数
	static tfunc* func_search_empty_param_f(tclass& tci,const rstr& fname)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if(fname==p->name&&
				p->is_friend&&
				p->param.empty())
				return p;
		}
		return null;
	}

	//类型完全相同
	static tfunc* func_search_same(tclass& tci,const rstr& fname,
			const rbuf<rstr>& vtype)
	{
		tfunc tfi;
		tfi.name=fname;
		for(int i=0;i<vtype.count();i++)
		{
			tfi.param.push(vtype[i]);
		}
		tfi.name_dec=tfi.get_dec();
		return tci.vfunc.find(tfi);
	}

	static tfunc* func_search_dec(tclass& tci,const rstr& func_dec)
	{
		tfunc tfi;
		tfi.name_dec=func_dec;
		return tci.vfunc.find(tfi);
	}
	
	//根据函数名搜索函数
	static tfunc* func_search(tclass& tci,const rstr& fname)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if(fname==p->name)
				return p;
		}
		return null;
	}

	static tfunc* func_search(tclass& tci,const rstr& fname,
		const rstr& a,const rstr& b)
	{
		tfunc item;
		item.name=fname;
		item.param.push(a);
		item.param.push(b);
		item.name_dec=item.get_dec();
		return tci.vfunc.find(item);
	}

	static tfunc* func_search(tclass& tci,const rstr& fname,const rstr& a)
	{
		tfunc item;
		item.name=fname;
		item.param.push(a);
		item.name_dec=item.get_dec();
		return tci.vfunc.find(item);
	}

	static tfunc* ftl_search(tclass& tci,rstr& name)
	{
		tfunc item;
		item.name=name;
		item.name_dec=item.get_dec();
		return tci.vfunctl.find(item);
	}

	static tfunc* destruct_search(tclass& tci)
	{
		return func_search(tci,"~"+tci.name,tci.name+"&");
	}

	static rbool is_destruct(tsh& sh,tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		return rppoptr(c_destruct)+tci.name==tfi.name&&
				tfi.param.count()==1&&
				tfi.param[0].type==tci.name+rppoptr(c_addr)&&
				tfi.retval.type==rppkey(c_void);
	}

	static tfunc* emptystruct_search(tclass& tci)
	{
		return func_search(tci,tci.name,tci.name+"&");
	}

	static rbool is_emptystruct(tsh& sh,tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		return tci.name==tfi.name&&
				tfi.param.count()==1&&
				tfi.param[0].type==tci.name+rppoptr(c_addr)&&
				tfi.retval.type==rppkey(c_void);
	}

	static tfunc* copystruct_search(tclass& tci)
	{
		return func_search(tci,tci.name,tci.name+"&",tci.name+"&");
	}

	static tdata* data_member_search(const tclass& tci,const rstr& name)
	{
		for(int i=0;i<tci.vdata.count();i++)
			if(name==tci.vdata[i].name)
				return &tci.vdata[i];
		return null;
	}

	static tdata* local_search(tfunc& tfi,const rstr& name)
	{
		for(int i=0;i<tfi.local.count();++i)
			if(name==tfi.local[i].name)
				return &tfi.local[i];
		for(int i=0;i<tfi.param.count();++i)
			if(name==tfi.param[i].name)
				return &tfi.param[i];
		if(tfi.retval.name==name)
		{
			return &tfi.retval;
		}
		return null;
	}
};

#endif
