#ifndef zftl_h__
#define zftl_h__

#include "zctl.h"

//函数模板替换类
struct zftl
{
	static rbool ftl_replace(tsh& sh,tclass& tci,rbuf<tword>& v,tclass* ptci)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppoptr(c_tbk_l))
				continue;
			int left=i;
			rstr name=v.get(left-1).val;
			if(ptci==null)
			{
				if(v.get(left-2).val==rppoptr(c_dot))
				{
					ptci=zfind::class_search(sh,v.get(left-3).val);
					if(ptci==null)
					{
						continue;
					}	
				}
				else
				{
					ptci=&tci;
				}
			}
			tfunc* ptfi=zfind::ftl_search(*ptci,name);
			if(ptfi==null)
			{
				ptci=sh.m_main;
				ptfi=zfind::ftl_search(*ptci,name);
			}
			if(null==ptfi)
				continue;
			int right=sh.find_symm_tbk(v,left);
			if(right>=v.count())
			{
				sh.error(v[left-1],"func template miss >");
				return false;
			}
			rbuf<rbuf<tword> > vparam=sh.comma_split(v.sub(left+1,right));
			if(vparam.count()!=ptfi->vtl.count())
			{
				sh.error(v[left-1],"func template not match");
				return false;
			}
			for(int j=left;j<=right;j++)
			{
				v[left-1].val+=v[j].val;
				v[j].val.clear();
			}
			//模板函数不可能重载，因此只要判断一下函数名是否相同即可
			if(zfind::func_search(*ptci,v[left-1].val)!=null)
			{
				continue;
			}
			tfunc item=*ptfi;
			item.vtl.free();
			zctl::vtl_replace(item.vword,ptfi->vtl,vparam);
			if(!f_type_replace(sh,item.vword))
			{
				return false;
			}
			if(!combine_ftl_name(sh,item.vword,ptfi->name))
			{
				return false;
			}
			if(!zmemb::func_add(sh,*ptci,item.vword,false))
			{
				return false;
			}
			i=right;
		}
		zpre::arrange(v);
		return true;
	}
	
	//处理函数模板中又出现新类型的情况
	static rbool f_type_replace(tsh& sh,rbuf<tword>& v)
	{
		int cur=sh.m_class.count();
		if(!zctl::type_replace(sh,v))
		{
			return false;
		}
		if(sh.m_class.count()>cur)
		{
			for(tclass* p=sh.m_class.begin();p!=sh.m_class.end();p=sh.m_class.next(p))
			{
				if(!f_proc_class(sh,*p))
				{
					return false;
				}
			}
		}
		return true;
	}
	
	static rbool f_proc_class(tsh& sh,tclass& tci)
	{
		if(!zctl::a_class(sh,tci))
		{
			return false;
		}
		if(!zmemb::recursion_get_size(sh,tci))
		{
			return false;
		}
		zmemb::obtain_size_func(sh,tci);
		return true;
	}

	static rbool combine_ftl_name(tsh& sh,rbuf<tword>& v,rstr& name)
	{
		for(int i=1;i<v.count();i++)
		{
			if(v[i]!=rppoptr(c_tbk_l))
			{
				continue;
			}
			if(name!=v[i-1].val)
			{
				continue;
			}
			int left=i;
			int right=sh.find_symm_tbk(v,left);
			if(right>=v.count())
			{
				sh.error(v[left-1],"miss >");
				return false;
			}
			for(int j=left;j<=right;j++)
			{
				v[left-1].val+=v[j].val;
				v[j].val.clear();
			}
			i=right;
		}
		zpre::arrange(v);
		return true;
	}
};

#endif
