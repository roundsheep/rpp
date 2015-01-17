#ifndef zctl_h__
#define zctl_h__

#include "tsh.h"
#include "zpre.h"
#include "zfind.h"
#include "zclass.h"
#include "zmac.h"
#include "zautof.h"
#include "zmemb.h"

//进行类模板替换，增加了一些新类
struct zctl
{
	static rbool process(tsh& sh)
	{
		rbuf<tclass*> vtmp;
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
		{
			zclass::combine_multi_class_name(sh,p->vword);
			vtmp.push(p);
		}
		for(int i=0;i<vtmp.count();i++)
		{
			ifn(type_replace(sh,vtmp[i]->vword,0))
			{
				return false;
			}
		}
		vtmp.clear();
		zautof::op_rstr(sh);
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
		{
			vtmp.push(p);
		}
		for(int i=0;i<vtmp.count();i++)
		{
			ifn(add_point_declare(sh,*vtmp[i]))
			{
				return false;
			}
		}
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
			if(!a_class(sh,*p))
			{
				return false;
			}
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
		{
			if(!zmemb::recursion_get_size(sh,*p))
				return false;
			if(p->name==rppkey(c_main))
			{
				sh.m_main_data.set_size(p->size);
			}
		}
		//简便起见，重复获取参数大小也没关系
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
			zmemb::obtain_size_func(sh,*p);
		return true;
	}

	static rbool add_point_declare(tsh& sh,tclass& tci)
	{
		if(sh.is_basic_type(tci.name)||tci.is_friend||sh.is_point(tci.name))
			return true;
		rbuf<tword> temp;
		temp+=rppkey(c_rp);
		temp+=rppoptr(c_tbk_l);
		temp+=tci.name;
		temp+=rppoptr(c_tbk_r);
		if(!type_replace(sh,temp))
		{
			return false;
		}
		return true;
	}
	
	//解析一个类
	static rbool a_class(tsh& sh,tclass& tci)
	{
		if(tci.vword.empty())
		{
			return true;
		}
		sbk_line_proc(sh,tci.vword,true);
		if(!zmemb::analyse(sh,tci))
		{
			return false;
		}
		ifn(zautof::auto_add_func(sh,tci))
		{
			return false;
		}
		tci.vword.free();
		return true;
	}

	static rbool type_replace(tsh& sh,rbuf<tword>& v,int level=0)
	{
		if(level++>c_rpp_deep)
		{
			sh.error("type level overflow");
			return false;
		}
		rbool need;
		point_replace(sh,v,need);
		for(int i=1;i<v.count();i++)
		{
			if(v[i]!=rppoptr(c_tbk_l))
			{
				continue;
			}
			tclass* pctl=zfind::classtl_search(sh,v[i-1].val);
			if(pctl==null)
			{
				continue;
			}
			int left=i;
			int right=sh.find_symm_tbk(v,left);
			if(right>=v.count())
			{
				sh.error(v[i-1],"miss >");
				return false;
			}
			rbuf<rbuf<tword> > vparam=sh.comma_split(v.sub(left+1,right));
			if(vparam.count()!=pctl->vtl.count())
			{
				sh.error(v[i-1],"template not match");
				return false;
			}
			ifn(is_type(sh,vparam))
			{
				continue;
			}
			for(int j=left;j<=right;j++)
			{
				v[left-1].val+=v[j].val;
				v[j].val.clear();
			}
			i=right;
			if(zfind::is_class(sh,v[left-1].val))
			{
				continue;
			}
			tclass item=*pctl;
			item.name=v[left-1].val;
			item.vtl.clear();
			vtl_replace(item.vword,pctl->vtl,vparam);
			sh.m_class.insert(item);
			tclass* ptci=zfind::class_search(sh,item.name);
			if(ptci==null)
			{
				sh.error(v.get(i),"can't find class "+item.name);
				return false;
			}
			ifn(type_replace(sh,ptci->vword,level))
			{
				return false;
			}
		}
		zpre::arrange(v);
		point_replace(sh,v,need);
		if(need)
		{
			if(!type_replace(sh,v,level))
			{
				return false;
			}
		}
		combine_quote(sh,v);
		zclass::arrange_format_l(sh,v);  //匿名函数要在这里处理大括号换行
		if(!zpre::op_const_eval(sh,v,false))
		{
			return false;
		}
		return true;
	}

	//todo 这里应该跳过匿名函数
	static void sbk_line_proc(tsh& sh,rbuf<tword>& v,rbool jump_bbk=false)
	{
		for(int i=0;i<v.count();i++)
		{
			int right;
			if(jump_bbk&&v[i]==rppoptr(c_bbk_l))
			{
				right=sh.find_symm_bbk(v,i);
				if(right<v.count())
				{
					i=right;
					continue;
				}
			}
			if(v[i]!=rppoptr(c_sbk_l)&&v[i]!=rppoptr(c_mbk_l))
			{
				continue;
			}
			int left=i;
			if(v[i]==rppoptr(c_sbk_l))
			{
				right=sh.find_symm_sbk(v,left);
			}
			else
			{
				right=sh.find_symm_mbk(v,left);
			}
			if(right>=v.count())
			{
				continue;
			}
			if(v[right].pos==v[left].pos)
			{
				continue;
			}
			for(int j=left+1;j<=right;j++)
			{
				//必须归于一行，导致提示行号不准确
				v[j].pos_src=v[left].pos_src;
				v[j].pos=v[left].pos;
			}
			i=right;
		}
	}
	
	//类模板只能替换类型，否则与函数模板冲突
	static rbool is_type(tsh& sh,rbuf<rbuf<tword> >& vparam)
	{
		for(int i=0;i<vparam.count();i++)
		{
			ifn(is_type(sh,vparam[i]))
			{
				return false;
			}
		}
		return true;
	}

	static rbool is_type(tsh& sh,rbuf<tword>& v)
	{
		return zfind::is_class_t(sh,v.get_bottom().val)||
			v.get(1)==rppoptr(c_tbk_l)&&
			zfind::is_classtl(sh,v.get_bottom().val);
	}

	static void vtl_replace(rbuf<tword>& result,rbuf<ttl>& vtl,
		rbuf<rbuf<tword> >& vparam)
	{
		for(int j=0;j<vparam.count();j++)
		{
			str_replace(result,vtl[j].name,tsh::vword_to_vstr(vparam[j]));
		}
		zpre::arrange(result);
	}

	static void str_replace(rbuf<tword>& v,const rstr& src,const rbuf<rstr>& vstr)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=src)
			{
				continue;
			}
			v[i].val.clear();
			v[i].multi=vstr;	
		}
	}

	static void point_replace(tsh& sh,rbuf<tword>& v,rbool& need)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v.get(i+1).val!=rppoptr(c_star)||
				!zfind::is_class(sh,v[i].val))
			{
				continue;
			}
			if(v.get(i+2).val==rppoptr(c_star))
			{
				if(v.get(i+3)==rppoptr(c_sbk_l))
				{
					continue;
				}
				rbuf<tword> vword;
				tword word;
				word.pos=v[i].pos;
				word.pos_src=v[i].pos_src;
				sh.push_twi(vword,word,rppkey(c_rp));
				sh.push_twi(vword,word,rppoptr(c_tbk_l));
				sh.push_twi(vword,word,rppkey(c_rp));
				sh.push_twi(vword,word,rppoptr(c_tbk_l));
				sh.push_twi(vword,word,v[i].val);
				sh.push_twi(vword,word,rppoptr(c_tbk_r));
				sh.push_twi(vword,word,rppoptr(c_tbk_r));
				v[i].multi=sh.vword_to_vstr(vword);
				v[i].val.clear();
				v[i+1].val.clear();
				v[i+2].val.clear();
			}
			else
			{
				if(v.get(i+2)==rppoptr(c_sbk_l))
				{
					continue;
				}
				rbuf<tword> vword;
				tword word;
				word.pos=v[i].pos;
				word.pos_src=v[i].pos_src;
				sh.push_twi(vword,word,rppkey(c_rp));
				sh.push_twi(vword,word,rppoptr(c_tbk_l));
				sh.push_twi(vword,word,v[i].val);
				sh.push_twi(vword,word,rppoptr(c_tbk_r));

				v[i].multi=sh.vword_to_vstr(vword);
				v[i].val.clear();
				v[i+1].val.clear();
			}
		}
		zpre::arrange(v,&need);
	}

	static void combine_quote(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v.get(i+1).val==rppoptr(c_addr)&&
				zfind::is_class(sh,v[i].val))
			{
				for(int j=i+1;j<v.count();j++)
				{
					if(v[j].val!=rppoptr(c_addr))
						break;
					v[i].val+=v[j].val;
					v[j].clear();
				}
			}
			elif((v[i].val==rppoptr(c_dot)||v[i]==rppoptr(c_arrow_r))&&
				v.get(i+1).val==rppoptr(c_destruct)&&
				zfind::is_class(sh,v.get(i+2).val))
			{
				v[i+1].val+=v[i+2].val;
				v[i+2].clear();
			}
		}
		zpre::arrange(v);
	}
};

#endif