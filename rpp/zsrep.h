#ifndef zsrep_h__
#define zsrep_h__

#include "zadd.h"
#include "zexp.h"

//简单替换类
struct zsrep
{
	static rbool typeof_replace(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();i++)
		{
			if(!typeof_replace(sh,tfi,tfi.vsent[i]))
			{
				return false;
			}
		}
		return true;
	}

	static rbool typeof_replace(tsh& sh,tfunc& tfi,tsent& sent)
	{
		rbuf<tword>& v=sent.vword;
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rppkey(c_typeof))
			{
				continue;
			}
			int left=i+1;
			if(v.get(left)!=rppoptr(c_sbk_l))
			{
				sh.error(sent,"miss (");
				return false;
			}
			int right=sh.find_symm_sbk(v,left);
			if(right>=v.count())
			{
				sh.error(sent,"miss )");
				return false;
			}
			tsent dst;
			dst.pos=sent.pos;
			dst.vword=v.sub(left+1,right);
			if(dst.vword.count()==1&&zfind::is_class(sh,dst.vword[0].val))
			{
				dst.type=dst.vword[0].val;
			}
			else
			{
				if(!zexp::p_exp(sh,dst,tfi))
				{
					return false;
				}
			}
			sh.clear_word_val(v,i,right+1);
			v[i].val=zsuper::add_quote(dst.type);
		}
		zpre::arrange(v);
		return true;
	}

	static void fpoint_replace(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();++i)
			fpoint_replace(sh,*tfi.ptci,tfi.vsent[i].vword);
	}

	static void fpoint_replace(tsh& sh,tclass& tci,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppoptr(c_addr))
				continue;
			tclass* ptci;
			rstr name;
			tfunc* ptfi;
			int left;
			if(v.get(i+2).val==rppoptr(c_dot))
			{
				ptci=zfind::class_search(sh,v.get(i+1).val);
				if(null==ptci)
					continue;
				name=v.get(i+3).val;
				left=i+4;
			}
			else
			{
				name=v.get(i+1).val;
				ptci=&tci;
				ptfi=zfind::func_search(*ptci,name);
				if(null==ptfi)
				{
					ptci=sh.m_main;	
				}
				left=i+2;
			}
			if(v.get(left)!=rppoptr(c_sbk_l))
			{
				ptfi=zfind::func_search(*ptci,name);
				if(null==ptfi)
					continue;
				sh.clear_word_val(v,i,left);
				v[i].multi=sh.get_func_declare_lisp(sh,*ptci,*ptfi);
				i=left-1;
			}
			else
			{
				int right=sh.find_symm_sbk(v,left);
				if(right>=v.count())
				{
					continue;
				}
				rbuf<tsent> vsent;
				sh.split_param(vsent,v.sub(left+1,right));
				rbuf<rstr> vtype;
				for(int j=0;j<vsent.count();j++)
					vtype.push(vsent[j].vword.get(0).val);
				ptfi=zfind::func_search_same(*ptci,name,vtype);
				if(null==ptfi)
					continue;
				sh.clear_word_val(v,i,right+1);
				v[i].multi=sh.get_func_declare_lisp(sh,*ptci,*ptfi);
				i=right;
			}
		}
		zpre::arrange(v);
	}

	static void const_replace(tsh& sh,rbuf<tsent>& vsent)
	{
		for(int i=0;i<vsent.count();++i)
		{
			if(sh.m_key.is_asm_ins(vsent[i].vword.get_bottom().val))
				continue;
			const_replace(sh,vsent[i].vword);
		}
	}

	static void const_replace(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(!v[i].is_const())
				continue;
			if(v[i].is_cint()&&v.get(i+1).val==rppoptr(c_dot))
			{
				v[i].multi.push(rppkey(c_int));
				v[i].multi.push(rppoptr(c_sbk_l));
				v[i].multi.push(v[i].val);
				v[i].multi.push(rppoptr(c_sbk_r));
				v[i].val.clear();
			}
			elif(v[i].is_cdouble()&&v.get(i+1).val==rppoptr(c_dot))
			{
				v[i].multi.push(rstr("double"));
				v[i].multi.push(rppoptr(c_sbk_l));
				v[i].multi.push(v[i].val);
				v[i].multi.push(rppoptr(c_sbk_r));
				v[i].val.clear();
			}
			elif(v[i].is_cstr()&&
				(v.get(i+1).val==rppoptr(c_dot)||
				zfind::is_rstr_optr(sh,v.get(i+1).val)||
				zfind::is_rstr_optr(sh,v.get(i-1).val)))
			{
				v[i].multi.push(rppkey(c_rstr));
				v[i].multi.push(rppoptr(c_sbk_l));
				v[i].multi.push(v[i].val);
				v[i].multi.push(rppoptr(c_sbk_r));
				v[i].val.clear();
			}
		}
		zpre::arrange(v);
	}

	static void neg_replace(tsh& sh,rbuf<tsent>& vsent)
	{
		for(int i=0;i<vsent.count();i++)
		{
			neg_replace(sh,vsent[i].vword);
		}
	}

	static void neg_replace(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count()-1;i++)
		{
			if(v[i]!=rppoptr(c_minus))
			{
				continue;
			}
			if(i==0)
			{
				v[i].val="neg";
				continue;
			}
			if(v[i-1]==rppoptr(c_equal)||
				v[i-1]==rppoptr(c_sbk_l)||
				v[i-1]==rppoptr(c_comma)||
				v[i-1]==rppoptr(c_equalequal)||
				v[i-1]==rppoptr(c_notequal))
			{
				v[i].val="neg";
			}
		}
	}

	static rbool size_off_to_zero(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();++i)
			if(!size_off_to_zero(sh,tfi.vsent[i].vword))
			{
				sh.error(tfi.vsent[i],"size_off_to_zero error");
				return false;
			}
		return true;
	}

	static rbool size_off_to_zero(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_sizeof)&&v[i].val!=rppkey(c_s_off))
			{
				continue;
			}
			if(i+1>=v.count())
			{
				return false;
			}
			if(v.get(i+1).val!=rppoptr(c_sbk_l))
			{
				v[i].multi.push(v[i].val);
				v[i].multi.push(v.get(i+1).val);
				v[i+1].clear();
				v[i].val=rstr("0");
				i++;
			}
			else
			{
				int right=sh.find_symm_sbk(v,i+1);
				if(right>=v.count())
					return false;
				v[i].multi.push(v[i].val);
				v[i].multi+=sh.vword_to_vstr(v.sub(i+2,right));
				sh.clear_word_val(v,i+1,right+1);
				v[i].val=rstr("0");
				i=right;
			}
		}
		zpre::arrange(v);
		return true;
	}

	static rbool size_off_replace(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();++i)
			if(!zsrep::size_off_replace(sh,tfi.vsent[i].vword,tfi))
			{
				sh.error(tfi.vsent[i],"size_off_replace error");
				return false;
			}
		return true;
	}

	static rbool size_off_replace(tsh& sh,rbuf<tword>& v,tfunc& tfi)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].multi.count()!=2)
				continue;
			rstr name=v[i].multi[1];
			if(v[i].multi[0]==rppkey(c_sizeof))
			{
				if(rppkey(c_s_local)==name)
				{
					v[i].val=rstr(zfind::get_func_local_size(tfi));
				}
				elif(rppkey(c_s_param)==name)
				{
					v[i].val=rstr(zfind::get_func_param_size(tfi));
				}
				else
				{
					tclass* ptci=zfind::class_search(sh,name);
					if(null==ptci)
						return false;
					v[i].val=rstr(ptci->size);
				}
			}
			elif(v[i].multi[0]==rppkey(c_s_off))
			{
				tdata* ptdi=zfind::local_search(tfi,name);
				if(null==ptdi)
					return false;
				v[i].val=rstr(ptdi->off);
			}
			else
				continue;
			v[i].multi.clear();
		}
		ifn(sh.m_key.is_asm_ins(v.get_bottom().val))
		{
			return true;
		}
		for(int i=1;i<v.count();i++)
		{
			if(!v[i].is_name())
				continue;
			tdata* ptdi=zfind::local_search(tfi,v[i].val);
			if(ptdi==null)
				continue;
			rbuf<rstr> vdst;
			if(v.get(i-1).val==rppoptr(c_addr))
			{
				vdst.push(rstr("ebp"));
				vdst.push(rstr("+"));
				vdst.push(rstr(ptdi->off));
				v[i-1].clear();
			}
			else
			{
				vdst.push(rstr(rppoptr(c_mbk_l)));
				vdst.push(rstr("ebp"));
				vdst.push(rstr("+"));
				vdst.push(rstr(ptdi->off));
				vdst.push(rstr(rppoptr(c_mbk_r)));
			}
			v[i].clear();
			v[i].multi=r_move(vdst);
		}
		zpre::arrange(v);
		return true;
	}

	static rbool local_var_replace(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();++i)
		{
			if(sh.m_key.is_asm_ins(tfi.vsent[i].vword.get_bottom().val))
				continue;
			if(!local_var_replace(sh,tfi.vsent[i].vword,tfi))
				return false;
		}
		return true;
	}

	static rbool local_var_replace(tsh& sh,rbuf<tword>& v,tfunc& tfi)
	{
		if(v.count()<2||
			!zfind::is_class_t(sh,v.get(0).val)||
			!v.get(1).is_name())
			return true;
		tdata tdi;
		if(!zmemb::a_data_define(sh,tdi,v))
			return false;
		v[0].val.clear();
		if(v.count()==2)
			v[1].val.clear();//清除未初始化的变量定义
		//如int a(1)这样的定义千万不能重复调用构造函数
		rbool bstruct=v.count()>2&&v[2].val==rppoptr(c_sbk_l);
		zpre::arrange(v);
		//排除重复定义的
		tdata* ptdi=zfind::local_search(tfi,tdi.name);
		if(null==ptdi)
			tfi.local.push(tdi);
		else
		{
			if(ptdi->type!=tdi.type)
			{
				sh.error(v.get(0),"diff type local var redefined");
				return false;
			}
		}
		rbuf<tword> vtemp;
		zadd::add_destructor_func(sh,tdi,vtemp);
		if(!bstruct)
			zadd::add_structor_func(sh,tdi,vtemp);
		vtemp+=r_move(v);
		v=r_move(vtemp);
		return true;
	}

	static rbool var_struct_replace(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();++i)
		{
			if(sh.m_key.is_asm_ins(tfi.vsent[i].vword.get_bottom().val))
				continue;
			if(!var_struct_replace(sh,tfi.vsent[i].vword,tfi))
			{
				sh.error(tfi.vsent[i],"var_struct_replace error");
				return false;
			}
		}
		return true;
	}

	static rbool var_struct_replace(tsh& sh,rbuf<tword>& v,tfunc& tfi)
	{
		//暂时只处理a(1)，忽略this.a(1)
		int left=1;
		if(v.get(left).val!=rppoptr(c_sbk_l))
			return true;
		tdata* ptdi=zfind::local_search(tfi,v.get(0).val);
		if(ptdi==null)
			return true;
		if(sh.is_quote(ptdi->type))
			return true;
		int right=sh.find_symm_sbk(v,left);
		if(right>=v.count())
		{
			return false;
		}
		if(right!=v.count()-1)
			return true;
		//动态类型和构造函数有歧义
		rbuf<tword> vtemp;
		vtemp.push(tword(ptdi->type));
		vtemp.push(tword(rppoptr(c_dot)));
		vtemp.push(tword(ptdi->type));
		vtemp.push(tword(rppoptr(c_sbk_l)));
		vtemp+=v.sub(0,left);
		rbuf<tword> vsub=v.sub(left+1,right);
		if(!vsub.empty())
		{
			vtemp.push(tword(rppoptr(c_comma)));
			vtemp+=vsub;
		}
		vtemp.push(tword(rppoptr(c_sbk_r)));
		v=r_move(vtemp);
		return true;
	}
};

#endif