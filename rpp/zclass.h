#ifndef zclass_h__
#define zclass_h__

#include "tsh.h"
#include "zpre.h"
#include "zfind.h"

//提取所有类
//m_file.vword->m_class.vword
struct zclass
{	
	static rbool process(tsh& sh)
	{
		basic_type_add(sh);
		for(tfile* p=sh.m_file.begin();p!=sh.m_file.end();p=sh.m_file.next(p))
			if(!find_class(sh,p->vword))
				return false;
		main_add(sh);
		for(tfile* p=sh.m_file.begin();p!=sh.m_file.end();p=sh.m_file.next(p))
			if(!proc_class_again(sh,p->vword))
				return false;
		if(!inherit_proc_all(sh))
			return false;
		for(tfile* p=sh.m_file.begin();p!=sh.m_file.end();p=sh.m_file.next(p))
		{
			p->cont.m_buf.free();
			p->vword.free();
		}
		return true;
	}

	static void combine_multi_class_name(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<c_rpp_deep;i++)
		{
			ifn(combine_multi_class_name_one(sh,v))
			{
				return;
			}
			zpre::arrange(v);
		}
	}

	//暂不支持模板类复合或者继承复合
	static rbool combine_multi_class_name_one(tsh& sh,rbuf<tword>& v)
	{
		rbool ret=false;
		for(int i=1;i<v.count();i++)
		{
			if(v[i].val!=rppoptr(c_dot))
			{
				continue;
			}
			ifn(zfind::is_class(sh,v[i-1].val))
			{
				continue;
			}
			if(i+1>=v.count())
			{
				continue;
			}
			rstr full=v[i-1].val+v[i].val+v[i+1].val;
			ifn(zfind::is_class(sh,full))
			{
				continue;
			}
			ret=true;
			v[i-1].val=full;
			v[i].val.clear();
			v[i+1].val.clear();
		}
		return ret;
	}

	static void main_add(tsh& sh)
	{
		sh.m_main=zfind::class_search(sh,rppkey(c_main));
		if(sh.m_main==null)
		{
			tclass item;
			item.name=rppkey(c_main);
			sh.m_class.insert(item);
			sh.m_main=zfind::class_search(sh,rppkey(c_main));	
		}
		sh.m_main->is_friend=true;
	}

	static rbool proc_class_again(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
			sh.m_main->vword.push(v[i]);
		return true;
	}

	static void arrange_format(tsh& sh,rbuf<tword>& v,tfunc* ptfi)
	{
		zclass::arrange_format_bbk(sh,v,ptfi);
		zclass::arrange_format_ctrl(sh,v,ptfi);
	}
	
	//双重循环有待优化
	static void arrange_format_bbk(tsh& sh,rbuf<tword>& v,tfunc* ptfi=null)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppoptr(c_bbk_l)&&v[i].val!=rppoptr(c_bbk_r))
			{
				continue;
			}
			if(v.get(i-1).pos!=v[i].pos&&v.get(i+1).pos!=v[i].pos)
			{
				continue;
			}
			v[i].pos.line++;
			for(int j=i+1;j<v.count();j++)
				v[j].pos.line+=2;
			if(ptfi)
				ptfi->last_pos.line+=2;
		}
	}

	static void arrange_format_ctrl(tsh& sh,rbuf<tword>& v,tfunc* ptfi)
	{
		for(int i=1;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_switch)&&v[i].val!=rppkey(c_case)&&
				v[i].val!=rppkey(c_for)&&v[i].val!=rppkey(c_else)&&
				v[i].val!=rppkey(c_ifn)&&
				(v[i].val!=rppkey(c_if)||v[i-1].val==rppkey(c_else)))
			{
				continue;
			}
			if(v[i-1].pos!=v[i].pos)
			{
				continue;
			}
			v[i].pos.line++;
			for(int j=i+1;j<v.count();j++)
				v[j].pos.line++;
			ptfi->last_pos.line++;
		}	
	}

	static rbool is_bbk_prev(tsh& sh,const rstr& s)
	{
		return s==rppoptr(c_comma)||s==rppoptr(c_equal)||
			s==rppoptr(c_sbk_l)||s==rppoptr(c_mbk_l)||s==rppoptr(c_bbk_l);
	}

	static void arrange_format_l(tsh& sh,rbuf<tword>& v,tfunc* ptfi=null)
	{
		for(int i=0;i<v.count();i++)
		{
			//todo 未判断左大括号开头的情况
			if(v[i].val==rppoptr(c_bbk_l)&&is_bbk_prev(sh,v.get(i-1).val))
			{
				continue;
			}
			if(v[i].val!=rppoptr(c_bbk_l)&&v[i].val!=rppoptr(c_bbk_r))
			{
				continue;
			}
			if(v.get(i-1).pos!=v[i].pos&&v.get(i+1).pos!=v[i].pos)
			{
				continue;
			}
			//！warning：{ } need occupy one line，otherwise can't debug
			v[i].pos.line++;
			for(int j=i+1;j<v.count();j++)
				v[j].pos.line+=2;
			if(ptfi)
				ptfi->last_pos.line+=2;
		}
	}

	static rbool name_part(tsh& sh,tclass& tci,rbuf<tword>& name_w)
	{
		rbuf<tword> father;
		int colonpos=r_find_pos(name_w,tword(rppoptr(c_colon)));
		if(colonpos<name_w.count())
		{
			for(int i=colonpos+1;i<name_w.count();i++)
				father.push(name_w[i]);
			if(father.empty())
			{
				sh.error(tci.vword.get(0),"empty father");
				return false;
			}
			name_w.erase(colonpos,name_w.count());
			tci.vfather=sh.comma_split_t(father);
		}
		int pos=r_find_pos(name_w,tword(rppoptr(c_tbk_l)));
		if(pos<name_w.count())
		{
			rstr temp;
			for(int i=pos+1;i<name_w.count()-1;i++)
				temp+=name_w[i].val;
			rbuf<rstr> result=r_split(temp,rppoptr(c_comma));
			if(result.empty())
			{
				sh.error(tci.vword.get(0),"template >error");
				return false;
			}
			for(int i=0;i<result.count();i++)
			{
				ttl item;
				int epos=r_find_pos(result[i],rppoptr(c_equal));
				for(int j=0;j<epos;j++)
					item.name+=result[i][j];
				if(item.name.empty())
				{
					sh.error(tci.vword.get(0),"empty template");
					return false;
				}
				for(int j=epos+1;j<result[i].count();j++)
					item.val+=result[i][j];
				tci.vtl.push(item);
			}
		}
		for(int i=0;i<pos;i++)
			tci.name+=name_w[i].val;
		if(tci.name.empty())
		{
			sh.error(tci.vword.get(0),"empty class name");
			return false;
		}
		return true;
	}

	static rbool proc_old_tl(tsh& sh,rbuf<tword>& v,int i,rbuf<tword>& name_w)
	{
		int right=i-1;
		if(v.get(right)!=rppoptr(c_tbk_r))
			return true;
		int left=sh.find_symm_word_rev(v,rppoptr(c_tbk_l),rppoptr(c_tbk_r),0,i);
		if(left>=v.count())
			return true;
		if(v.get(left-1)!=rppkey(c_template))
		{
			return true;
		}
		rbuf<rbuf<tword> > temp;
		temp=sh.comma_split(v.sub(left+1,right));
		if(temp.empty())
		{
			return true;
		}
		
		for(int j=0;j<temp.count();j++)
		{
			if(temp[j].get(0)==rppkey(c_typename))
			{
				temp[j].pop_front();
			}
		}
		rbuf<tword> vtl;
		vtl+=rppoptr(c_tbk_l);
		for(int j=0;j<temp.count();j++)
		{
			if(j!=0)
			{
				vtl+=rppoptr(c_comma);
			}
			vtl+=temp[j];
		}
		vtl+=rppoptr(c_tbk_r);
		name_w.insert(1,vtl);
		for(int j=left-1;j<i;j++)
		{
			v[j].clear();
		}
		return true;
	}

	static rbool find_class(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_class))
			{
				continue;
			}
			int left=r_find_pos(v,tword(rppoptr(c_bbk_l)),i+1);
			if(left>=v.count())
			{
				sh.error(v.get(i),"miss {");
				return false;
			}
			tclass item;
			rbuf<tword> name_w;
			for(int j=i+1;j<left;j++)
				name_w.push(v[j]);
			if(name_w.empty())
			{
				sh.error(v.get(i),"miss class name");
				return false;
			}
			proc_old_tl(sh,v,i,name_w);
			int right=sh.find_symm_bbk(v,left);
			if(right>=v.count())
			{
				sh.error(v.get(i),"miss {");
				return false;
			}
			if(v.get(i-1).val==rppkey(c_friend))
			{
				v[i-1].clear();
				item.is_friend=true;
			}
			for(int j=left+1;j<right;j++)
				item.vword.push(v[j]);
			for(int j=i;j<=right;j++)
				v[j].clear();
			if(!class_add(sh,item,name_w))
				return false;
			i=right;
		}
		zpre::arrange(v);
		return true;
	}

	static rbool class_add(tsh& sh,tclass& tci,rbuf<tword>& name_w)
	{
		if(!name_part(sh,tci,name_w))
			return false;
		if(zfind::is_class(sh,tci.name)||zfind::is_classtl(sh,tci.name))
		{
			sh.error(name_w.get_bottom(),"type redefined");
			return false;
		}
		if(tci.vtl.empty())
		{
			sh.m_class.insert(tci);
		}
		else
		{
			sh.m_classtl.insert(tci);
		}
		return true;
	}

	static void insert_type(tsh& sh,const rstr& name,int size)
	{
		tclass item;
		item.name=name;
		item.size=size;
		if(!zfind::is_class(sh,item.name))
			sh.m_class.insert(item);
	}

	static void basic_type_add(tsh& sh)
	{
		insert_type(sh,rppkey(c_void),0);
		insert_type(sh,rppkey(c_rd1),1);
		insert_type(sh,rppkey(c_rd2),2);
		insert_type(sh,rppkey(c_rd4),4);
		insert_type(sh,rppkey(c_rd8),8);
		insert_type(sh,rppkey(c_rdp),c_point_size);
		insert_type(sh,rppkey(c_rcs),c_point_size);
	}

	static rbool inherit_proc_all(tsh& sh)
	{
		for(tclass* p=sh.m_class.begin();p!=sh.m_class.end();p=sh.m_class.next(p))
			if(!inherit_proc(sh,*p))
				return false;
		return true;
	}
	
	//模板继承模板如 A<T>:B<T>
	//模板继承非模板 A<T>:C,D
	//非模板继承非模板 E:C
	//模板实例暂时不能继承如 C:A<int>
	//如果定义同名函数会调用子类的函数，所以不要用覆盖和隐藏
	//后面自动生成构造、析构、拷贝构造、operator=不会继承，但用户定义的会继承
	static rbool inherit_proc(tsh& sh,tclass& tci,int level=0)
	{
		if(level++>c_rpp_deep)
		{
			sh.error("inherit too deep");
			return false;
		}
		if(tci.vfather.empty())
			return true;
		rbuf<tword> v;
		for(int i=0;i<tci.vfather.count();i++)
		{
			rstr cname=tci.vfather[i].vword.get(0).val;
			tclass* ptci=zfind::class_search(sh,cname);
			if(ptci==null)
			{
				ptci=zfind::classtl_search(sh,cname);
				if(ptci==null)
				{
					sh.error("inherit can't find "+cname);
					return false;
				}
			}
			if(!inherit_proc(sh,*ptci,level))
				return false;
			v+=ptci->vword;
		}
		v+=tci.vword;
		tci.vword=v;
		return true;
	}
};

#endif