#ifndef zmatch_h__
#define zmatch_h__

#include "zfind.h"

struct zmatch
{
	//可构造转换传递
	static rbool is_construct_pass(tsh& sh,tsent& src,rstr& dst)
	{
		if (dst==rppkey(c_rcs))
		{
			return true;
		}
		tclass* ptci=zfind::class_search_t(sh,dst);
		if (ptci==null)
		{
			return false;
		}
		tfunc* ptfi=zfind::func_search(*ptci,sh.get_tname(dst),
			sh.get_tname(dst)+"&",sh.get_tname(src.type)+"&");
		if (ptfi!=null)
		{
			return true;
		}
		ptfi=zfind::func_search(*ptci,sh.get_tname(dst),
			sh.get_tname(dst)+"&",sh.get_tname(src.type));
		return ptfi!=null;
	}

	//可直接传递，返回对象可传递引用
	static rbool is_direct_pass(tsh& sh,tsent& src,rstr& dst)
	{
		if (sh.is_point(src.type)&&sh.is_point(dst))
			return true;
		if (dst==rppkey(c_rcs))
		{
			return true;
		}
		if (dst==rppkey(c_rd4)&&zfind::get_type_size(sh,src.type)==4)
			return true;
		if (sh.get_tname(src.type)!=sh.get_tname(dst))
			return false;
		return true;
	}

	//是否直接传递或构造转换传递
	static rbool is_direct_construct_pass_m(tsh& sh,rbuf<tsent>& vsent,rbuf<tdata>& vparam)
	{
		if (vsent.count()!=vparam.count())
		{
			return false;
		}
		for (int i=0;i<vsent.count();i++)
		{
			if (!is_direct_pass(sh,vsent[i],vparam[i].type)&&
				!is_construct_pass(sh,vsent[i],vparam[i].type))
			{
				return false;
			}
		}
		return true;
	}

	//查找直接传递或构造转换传递
	static tfunc* find_direct_construct_pass(tsh& sh,tclass& tci,const rstr& fname,
		rbuf<tsent>& vsent)
	{
		for (tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if (fname!=p->name)
				continue;
			if (is_direct_construct_pass_m(sh,vsent,p->param))
				return p;
		}
		return null;
	}

	//可完全传递，临时对象可传递引用，常量和返回对象不可传递引用
	static rbool is_full_pass(tsh& sh,tsent& src,rstr& dst)
	{
		if (!is_direct_pass(sh,src,dst))
			return false;
		if (src.vword.count()==1&&
			(src.vword[0].is_const()||sh.m_key.is_asm_reg(src.vword[0].val))&&
			sh.is_quote(dst))
		{
			return false;
		}
		if (src.vword.count()==5&&sh.is_quote(dst)&&src.vword[1].val==rppoptr(c_addr)&&
			src.vword[0].val==rppoptr(c_mbk_l))
		{
			return false;
		}
		if (is_temp_var(sh,src))
		{
			return true;
		}
		return !sh.is_quote(dst)||!is_ret_obj(sh,src.vword);
	}

	static rbool is_full_pass_m(tsh& sh,rbuf<tsent>& vsent,rbuf<tdata>& vparam)
	{
		if (vsent.count()!=vparam.count())
		{
			return false;
		}
		for (int i=0;i<vsent.count();i++)
		{
			if (!is_full_pass(sh,vsent[i],vparam[i].type))
			{
				return false;
			}
		}
		return true;
	}

	static tfunc* find_full_pass(tsh& sh,tclass& tci,const rstr& fname,
		rbuf<tsent>& vsent)
	{
		for (tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if (fname!=p->name)
				continue;
			if (is_full_pass_m(sh,vsent,p->param))
				return p;
		}
		return null;
	}

	static rbool is_temp_var(tsh& sh,tsent& sent)
	{
		return !tsh::is_quote(sent.type)&&
			sent.vword.count()>=3&&sent.type==sent.vword[2].val&&
			sent.vword[1].val==rppkey(c_btemp);
	}

	//重载的友元属性和返回值类型必须相同
	static rbool is_ret_obj(tsh& sh,rbuf<tword>& v)
	{
		if (v.count()<3)
		{
			return false;
		}
		int start=0;
		while (start+2<v.count()&&
			v[start+1].val==rppoptr(c_dot)&&
			v[start].val==rppoptr(c_mbk_l))
		{
			start+=2;
		}
		if (v[start].val==rppoptr(c_mbk_l))
		{
			start++;
		}
		if (start+1>=v.count())
		{
			return false;
		}
		if (v[start].val==rppkey(c_pcall))
		{
			return !sh.is_quote(v[start+1].val);
		}
		tclass* ptci=zfind::class_search(sh,v[start].val);
		if (ptci==null)
		{
			return false;
		}
		tfunc* ptfi=zfind::func_search_dec(*ptci,v[start+1].val);
		if (ptfi==null)
		{
			return false;
		}
		return !sh.is_quote(ptfi->retval.type);
	}

	static void insert_type(tsh& sh,tsent& sent,const rstr& type)
	{
		rbuf<tword> result;
		result+=rppoptr(c_mbk_l);
		result+=rppkey(c_btemp);
		result+=type;
		result+=rppoptr(c_mbk_l);
		result+=sent.vword;
		result+=rppoptr(c_mbk_r);
		result+=rppoptr(c_mbk_r);
		sent.vword=result;
		sent.type=type;
	}

	//先完全传递，找不到再尝试替换单词
	static tfunc* find_replace(tsh& sh,tclass& tci,const rstr& fname,
		rbuf<tsent>& vsent)
	{
		tfunc* ptfi=find_full_pass(sh,tci,fname,vsent);
		if (ptfi!=null)
		{
			return ptfi;
		}
		//可能找到多个，只取第一个
		ptfi=find_direct_construct_pass(sh,tci,fname,vsent);
		if (ptfi==null)
		{
			return null;
		}
		for (int i=0;i<vsent.count();i++)
		{
			//这里也可以用 if is_direct_construct_pass
			ifn (is_full_pass(sh,vsent[i],ptfi->param[i].type))
			{
				insert_type(sh,vsent[i],sh.get_tname(ptfi->param[i].type));
			}
		}
		return ptfi;
	}
};

#endif
