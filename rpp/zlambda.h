#ifndef zlambda_h__
#define zlambda_h__

#include "zexp.h"
#include "zsrep.h"
#include "zftl.h"
#include "zself.h"

struct zlambda
{
	//闭包处理，todo:闭包暂时不能使用类型推断,不是线程安全
	static void lambda_pack(tsh& sh,const rbuf<tword>& vhead,const rbuf<tword>& vbody,
		tfunc& tfi,tfunc& lambda)
	{
		tclass item;
		for(int i=0;i<vbody.count();i++)
		{
			ifn(vbody[i].is_name())
			{
				continue;
			}
			if(zfind::is_class_t(sh,vbody[i].val))
			{
				continue;
			}
			//todo 未判断重复
			tdata* ptdi=zfind::local_search(tfi,vbody[i].val);
			tdata tdi;
			if(null==ptdi)
			{
				int pos=vhead.find(vbody[i]);
				if(pos>=vhead.count()||pos==0)
				{
					continue;
				}
				ifn(zfind::is_class_t(sh,vhead[pos-1].val))
				{
					continue;
				}
				ptdi=&tdi;
				ptdi->type=vhead[pos-1].val;
			}
			else
			{
				tdi.type=ptdi->type;
				ptdi=&tdi;
			}
			ptdi->name=vbody[i].val;
			ptdi->count=1;
			ptdi->size=zfind::get_type_size(sh,ptdi->type);
			if(ptdi->size==0)
			{
				continue;
			}
			item.vdata.push(*ptdi);
		}
		if(item.vdata.empty())
		{
			return;
		}
		item.is_friend=true;
		item.name=lambda.name+"_C";
		for(int i=0;i<item.vdata.count();i++)
		{
			item.vdata[i].off=item.size;
			item.size+=item.vdata[i].size;
		}
		lambda.lambda_data.set_size(item.size);
		sh.m_class.insert_c(item);
	}

	static rbool lambda_replace(tsh& sh,tclass& tci,tfunc& tfi)
	{
		rbuf<tword>& v=tfi.vword;
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rppkey(c_lambda))
			{
				continue;
			}
			if(v.get(i+1)!=rppoptr(c_sbk_l))
			{
				continue;
			}
			int right=sh.find_symm_sbk(v,i+1);
			if(right>=v.count())
			{
				continue;
			}
			int left=right+1;
			if(v.get(left)!=rppoptr(c_bbk_l))
			{
				continue;
			}
			right=sh.find_symm_bbk(v,left);
			if(right>=v.count())
			{
				continue;
			}
			v[i].val=rstr("_LAMBDA")+tci.vfunc.count();
			ifn(zmemb::func_add(sh,tci,v.sub(i,right+1),false))
			{
				return false;
			}
			tfunc* ptfi=zfind::func_search(tci,v[i].val);
			if(ptfi!=null)
			{
				lambda_pack(sh,v.sub(0,i),v.sub(left+1,right),tfi,*ptfi);
			}
			v[i].multi.push("(");
			v[i].multi.push("&");
			v[i].multi.push(tci.name);
			v[i].multi.push(".");
			v[i].multi.push(v[i].val);
			v[i].multi.push(")");
			sh.clear_word_val(v,i,right+1);
			i=right;
		}
		zpre::arrange(v);
		return true;
	}

	static rstr lambda_get(rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val.sub(0,7)=="_LAMBDA")
			{
				return v[i].val;
			}
		}
		return "";
	}

	static void lambda_push_type(const rstr& name,tfunc& tfi)
	{
		tdata item;
		item.name=name+"_C_P";
		item.type=name+"_C&";
		item.size=4;
		tfi.local.push(item);
	}

	static void lambda_proc(tsh& sh,tclass& tci,tfunc& tfi)
	{
		rbuf<tsent>& v=tfi.vsent;
		for(int i=0;i<v.count();i++)
		{
			rstr name=lambda_get(v[i].vword);
			if(name.empty())
			{
				continue;
			}
			tfunc* ptfi=zfind::func_search(tci,name);
			if(ptfi==null||ptfi->lambda_data.empty())
			{
				continue;
			}
			tclass* ptci=zfind::class_search(sh,name+"_C");
			if(ptci==null||ptci->vdata.empty())
			{
				continue;
			}
			rstr data_name=name+"_C_P";
			lambda_push_type(name,tfi);
			tword word;
			word.pos_src=v[i].vword.get_bottom().pos_src;
			word.pos=v[i].vword.get_bottom().pos;
			rbuf<tword> vtemp;
			sh.push_twi(vtemp,word,"mov");
			sh.push_twi(vtemp,word,data_name);
			sh.push_twi(vtemp,word,",");
			if(sh.m_mode==tsh::c_rvm||sh.m_mode==tsh::c_jit)
			{
				sh.push_twi(vtemp,word,rstr((uint)ptfi->lambda_data.begin()));
			}
			else
			{
				sh.push_twi(vtemp,word,ptfi->name+"_C_P_A");
			}
			sh.push_twi(vtemp,word,";");
			for(int j=0;j<ptci->vdata.count();j++)
			{
				sh.push_twi(vtemp,word,data_name);
				sh.push_twi(vtemp,word,".");
				sh.push_twi(vtemp,word,ptci->vdata[j].name);
				sh.push_twi(vtemp,word,"=");
				sh.push_twi(vtemp,word,ptci->vdata[j].name);
				sh.push_twi(vtemp,word,";");
			}
			vtemp+=v[i].vword;
			v[i].vword=vtemp;
		}
		zcontrol::part_vsent(v);
	}

	static void lambda_add_init_asm(tsh& sh,tfunc& tfi)
	{
		if(tfi.lambda_data.empty())
		{
			return;
		}
		tsent sent;
		sent.pos=tfi.first_pos;
		sent.vword.push(tword("mov"));//tword没有设置pos
		sent.vword.push(tword(tfi.name+"_C_P"));
		sent.vword.push(tword(","));
		if(sh.m_mode==tsh::c_rvm||sh.m_mode==tsh::c_jit)
		{
			sent.vword.push(tword(rstr((uint)tfi.lambda_data.begin())));
		}
		else
		{
			sent.vword.push(tword(tfi.name+"_C_P_A"));
		}
		tfi.vsent.push_front(sent);
	}

	static void lambda_var_replace(tsh& sh,tfunc& tfi)
	{
		if(tfi.lambda_data.empty())
		{
			return;
		}
		tclass* ptci=zfind::class_search(sh,tfi.name+"_C");
		if(ptci==null)
		{
			return;
		}
		lambda_push_type(tfi.name,tfi);
		rstr data_name=tfi.name+"_C_P";
		rbuf<tword>& v=tfi.vword;
		for(int i=0;i<v.count();i++)
		{
			if(zfind::data_member_search(*ptci,v[i].val)!=null)
			{
				v[i].multi.push(data_name);
				v[i].multi.push(".");
				v[i].multi.push(v[i].val);
				v[i].val.clear();
			}
		}
		zpre::arrange(v);
	}

	static rbool function_replace(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_js_func))
			{
				continue;
			}
			if(v.get(i+1).val!=rppoptr(c_sbk_l))
			{
				sh.error(v[i],"miss (");
				return false;
			}
			v[i].multi+=rppkey(c_lambda);
			v[i].multi+=rppoptr(c_sbk_l);
			v[i].multi+="var";
			v[i].multi+=",";
			v[i].multi+="var&";
			v[i].multi+="this";
			if(v.get(i+2).val!=rppoptr(c_sbk_r))
			{
				v[i].multi+=",";
			}
			v[i].val.clear();
			v[i+1].val.clear();
		}
		zpre::arrange(v);
		return true;
	}
};

#endif