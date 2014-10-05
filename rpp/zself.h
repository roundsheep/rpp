#ifndef zself_h__
#define zself_h__

#include "tsh.h"

//元函数替换
struct zself
{
	static rbool self_replace(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();++i)
		{
			if(!self_replace(sh,tfi.vsent[i].vword,tfi))
			{
				sh.error(tfi.vsent[i],"self_replace error");
				return false;
			}
		}
		return true;
	}

	static rbool self_replace(tsh& sh,rbuf<tword>& v,tfunc& tfi)
	{
		for(int i=0;i<v.count()-1;i++)
		{
			if(v[i]!=rppkey(c_self))
			{
				continue;
			}
			int left=i+1;
			if(v[left]!=rppoptr(c_sbk_l))
			{
				return false;
			}
			int right=sh.find_symm_sbk(v,left);
			if(right>=v.count())
			{
				return false;
			}
			rbuf<rstr> vstr;
			vstr.push(tfi.retval.type);
			vstr.push(rppoptr(c_mbk_l));
			vstr.push(rstr("self_m"));
			vstr.push(rppoptr(c_sbk_l));
			vstr.push(rstr("rstr"));
			vstr.push(rppoptr(c_sbk_l));
			vstr.push("\"friend "+self_get_declare(sh,tfi)+"\"");
			vstr.push(rppoptr(c_sbk_r));
			vstr.push(rstr(","));
			for(int j=left+1;j<right;j++)
			{
				vstr.push(v[j].val);
			}
			vstr.push(rppoptr(c_sbk_r));
			self_push_param(sh,tfi,vstr);
			vstr.push(rppoptr(c_mbk_r));
			sh.clear_word_val(v,i,right+1);
			v[i].multi=vstr;
			i=right;
		}
		zpre::arrange(v);
		return true;
	}

	static rbuf<tdata> get_all_tdata(tsh& sh,tfunc& tfi)
	{
		rbuf<tdata> v;
		for(int i=0;i<tfi.param.count();i++)
		{
			v.push(tfi.param[i]);
		}
		rstr tmpname=rppkey(c_temp);
		for(int i=0;i<tfi.local.count();i++)
		{
			if(tfi.local[i].name.sub(0,tmpname.count())==tmpname)
			{
				continue;
			}
			if(tfi.local[i].name==rppkey(c_pmain))
			{
				continue;
			}
			v.push(tfi.local[i]);
		}
		return r_move(v);
	}

	static void push_func_param(tsh& sh,tfunc& tfi,rbuf<tword>& v)
	{
		rbuf<tdata> vdata=get_all_tdata(sh,tfi);
		for(int i=0;i<vdata.count();i++)
		{
			v.push(rstr(","));
			ifn(tsh::is_point(vdata[i].type))
			{
				v.push(rstr("&"));
			}
			v.push(vdata[i].name);
		}
	}

	static rstr get_func_txt(tsh& sh,tfunc& tfi,tsent& sent,rstr& ret_type)
	{
		rstr s="\"friend "+ret_type+" _SELF(";
		rbuf<tdata> vdata=get_all_tdata(sh,tfi);
		for(int i=0;i<vdata.count();i++)
		{
			if(i!=0)
			{
				s+=",";
			}
			s+=get_func_txt_dec(vdata[i]);
		}
		s+="){return ";
		for(int i=0;i<sent.vword.count();i++)
		{
			s+=sent.vword[i].val;
			s+=" ";
		}
		s+="}\"";
		return r_move(s);
	}

	static rstr get_func_txt_dec(tdata& tdi)
	{
		rstr s=tdi.type;
		if(!tsh::is_point(tdi.type)&&!tsh::is_quote(tdi.type))
		{
			s+="&";
		}
		s+=" "+tdi.name;
		return r_move(s);
	}

	static void self_push_param(tsh& sh,tfunc& tfi,rbuf<rstr>& vstr)
	{
		rbuf<tdata> vdata=get_all_tdata(sh,tfi);
		for(int i=0;i<vdata.count();i++)
		{
			self_push_name(vdata[i],vstr);
		}
	}

	static void self_push_name(tdata& tdi,rbuf<rstr>& vstr)
	{
		vstr.push(rstr(","));
		ifn(tsh::is_point(tdi.type))
		{
			vstr.push(rstr("&"));
		}
		vstr.push(tdi.name);
	}

	static rstr self_get_declare(tsh& sh,tfunc& tfi)
	{
		rstr ret=tfi.retval.type;
		ret+=" _SELF(";
		rbuf<tdata> vdata=get_all_tdata(sh,tfi);
		for(int i=0;i<vdata.count();i++)
		{
			if(i!=0)
			{
				ret+=",";
			}
			ret+=self_get_data_dec(vdata[i]);
		}
		ret+=")";
		return r_move(ret);
	}

	static rstr self_get_data_dec(tdata& tdi)
	{
		return tsh::get_ttype(tdi.type)+" "+tdi.name;
	}
};

#endif
