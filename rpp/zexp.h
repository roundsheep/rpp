#ifndef zexp_h__
#define zexp_h__

#include "tsh.h"
#include "zctl.h"
#include "zcontrol.h"
#include "zftl.h"
#include "zmatch.h"
#include "zself.h"

//表达式标准化类，返回表达式的类型
struct zexp
{
	//处理函数名调用
	static rbool p_func_call(tsh& sh,tsent& src,tsent& outopnd,
		tfunc& tfi,int level,int& i)
	{
		tclass* ptci=tfi.ptci;
		rstr fname=src.vword[i].val;
		tfunc* ptfi=zfind::func_search(*ptci,fname);
		if(ptfi==null)
		{
			ptci=sh.m_main;
			if(!p_call(sh,src,null,outopnd,tfi,level,i,ptci))
				return false;
			return true;
		}
		//函数重载不能友元和非友元共存，友元标志很重要。
		if(ptfi->is_friend)
		{
			if(!p_call(sh,src,null,outopnd,tfi,level,i,ptci))
				return false;
		}
		else
		{
			tsent first;
			first.type=ptci->name+rppoptr(c_addr);
			first.vword+=rppkey(c_this);
			if(!p_call(sh,src,&first,outopnd,tfi,level,i,ptci))
				return false;
		}
		return true;
	}

	//处理[]函数指针直接调用
	static rbool p_point_call(tsh& sh,tsent& src,tsent& outopnd,
		tfunc& tfi,int level,int& i)
	{
		int left=i+1;
		int right=sh.find_symm_mbk(src.vword,left);
		if(right>=src.vword.count())
		{
			sh.error(src,"call miss ]");
			return false;
		}
		outopnd.type=src.vword[i].val;
		outopnd.vword+=rppoptr(c_mbk_l);
		outopnd.vword+=rppkey(c_pcall);
		outopnd.vword+=outopnd.type;
		
		rbuf<tsent> vsent;
		sh.split_param(vsent,src.vword.sub(left+1,right),src);
		if(vsent.empty())
		{
			sh.error(src,"call miss func");
			return false;
		}
		for(int j=0;j<vsent.count();j++)
		{
			if(!p_exp(sh,vsent[j],tfi,level))
				return false;
		}
		outopnd.vword+=vsent[0].vword;
		outopnd.vword+=rppoptr(c_mbk_l);
		for(int j=1;j<vsent.count();j++)
		{
			outopnd.vword+=vsent[j].vword;
		}
		outopnd.vword+=rppoptr(c_mbk_r);
		outopnd.vword+=rppoptr(c_mbk_r);
		i=right;
		return true;
	}

	static rbool p_temp_var(tsh& sh,tsent& src,tsent& outopnd,
		tfunc& tfi,int level,int& i)
	{
		int left=i+1;
		int right=sh.find_symm_sbk(src.vword,left);
		if(right>=src.vword.count())
		{
			sh.error(src,"class name miss (");
			return false;
		}
		rbuf<tsent> vsent;
		sh.split_param(vsent,src.vword.sub(left+1,right),src);
		if(!vsent.empty())
		{
			for(int j=0;j<vsent.count();j++)
			{
				if(!p_exp(sh,vsent[j],tfi,level))
					return false;
			}
		}
		outopnd.type=src.vword.get(i).val;
		outopnd.vword+=rppoptr(c_mbk_l);
		outopnd.vword+=rppkey(c_btemp);
		outopnd.vword+=outopnd.type;
		outopnd.vword+=rppoptr(c_mbk_l);
		for(int j=0;j<vsent.count();j++)
		{
			outopnd.vword+=vsent[j].vword;
		}
		outopnd.vword+=rppoptr(c_mbk_r);
		outopnd.vword+=rppoptr(c_mbk_r);
		i=right;
		return true;
	}
	
	//处理类名直接调用
	static rbool p_class_call(tsh& sh,tsent& src,tsent& outopnd,
		tfunc& tfi,int level,int& i)
	{
		if(src.vword.get(i+1).val!=sh.m_optr[toptr::c_dot])
		{
			sh.error(src,"class name miss .");
			return false;
		}
		rstr cname=src.vword[i].val;
		tclass* ptci=zfind::class_search(sh,cname);
		if(ptci==null)
		{
			sh.error(src,"can't find class name");
			return false;
		}
		i+=2;
		if(!p_call(sh,src,null,outopnd,tfi,level,i,ptci))
			return false;
		return true;
	}

	static rbool p_mbk(tsh& sh,const tsent& first,const tsent& second,tsent& outopnd,
		tfunc& tfi,int level)
	{
		rbuf<tsent> vsent;
		sh.split_param(vsent,second.vword,first);
		for(int j=0;j<vsent.count();j++)
		{
			if(!p_exp(sh,vsent[j],tfi,level))
				return false;
		}
		vsent.push_front(first);
		tclass* ptci=zfind::class_search_t(sh,first.type);
		if(ptci==null)
		{
			sh.error(first,"can't find func mbk");
			return false;
		}
		tfunc* ptfi=zmatch::find_replace(sh,*ptci,"[]",vsent);
		if(null==ptfi)
		{
			sh.error(first,"can't find func mbk");
			return false;
		}
		set_func(sh,outopnd,vsent,ptfi);
		return true;
	}

	static rbool p_lisp_param(tsh& sh,const tsent& src,rbuf<tsent>& vsent,
		tfunc& tfi,int level)
	{
		if(src.vword.get_bottom().val!=rppoptr(c_mbk_l)&&
			src.vword.get_top().val!=rppoptr(c_mbk_r))
		{
			sh.error(src,"lisp exp miss");
			return false;
		}
		for(int j=1;j<src.vword.count()-1;j++)
		{
			tsent sent;
			int right;
			if(src.vword[j]==rppoptr(c_mbk_l))
			{
				right=sh.find_symm_mbk(src.vword,j);
				if(right>=src.vword.count())
				{
					sh.error(src,"lisp exp");
					return false;
				}
				sent=src.sub(j,right+1);
			}
			else
			{
				sent=src.sub(j,j+1);
				right=j;
			}
			if(!p_exp(sh,sent,tfi,level))
			{
				return false;
			}
			vsent.push(sent);
			j=right;
		}
		return true;
	}

	static rbool p_lisp(tsh& sh,const tsent& src,tsent& outopnd,
		tfunc& tfi,int level)
	{
		if(src.vword.get_bottom().val!=rppoptr(c_mbk_l)&&
			src.vword.get_top().val!=rppoptr(c_mbk_r))
		{
			sh.error(src,"lisp exp miss");
			return false;
		}
		outopnd.vword.clear();
		outopnd.pos=src.pos;
		tclass* ptci=null;
		tfunc* ptfi=null;
		rstr cname=src.vword.get(1).val;
		rstr fname=src.vword.get(2).val;
		ptci=zfind::class_search(sh,cname);
		if(ptci!=null)
		{
			ptfi=zfind::func_search_dec(*ptci,fname);
			rbuf<tsent> vsent;
			if(ptfi!=null)
			{
				if(!p_lisp_param(sh,src.sub(3,src.vword.count()-1),vsent,tfi,level))
				{
					return false;
				}
			}
			else
			{
				if(!p_lisp_param(sh,src.sub(3,src.vword.count()-1),vsent,tfi,level))
				{
					return false;
				}
				ptfi=zmatch::find_replace(sh,*ptci,fname,vsent);
				if(ptfi==null)
				{
					sh.error(src,"lisp exp miss");
					return false;
				}
			}
			set_func(sh,outopnd,vsent,ptfi);
			return true;
		}
		if(cname==rppkey(c_pcall)||cname==rppkey(c_btemp))
		{
			outopnd.vword=src.vword;
			outopnd.type=fname;
			return true;
		}
		if(cname==rppoptr(c_dot))
		{
			tsent sent;
			rbuf<rbuf<tword> > vlisp;
			get_vlisp(sh,src.vword,vlisp);
			if(vlisp.count()!=3)
			{
				sh.error(src,"lisp exp miss");
				return false;
			}
			sent.vword=vlisp[1];
			sent.pos=src.pos;
			ifn(p_exp(sh,sent,tfi,level))
			{
				return false;
			}
			ptci=zfind::class_search_t(sh,sent.type);
			if(ptci==null)
			{
				sh.error(src,"lisp exp miss");
				return false;
			}
			tdata* ptdi=zfind::data_member_search(*ptci,vlisp[2].get(0).val);
			if(ptdi==null)
			{
				sh.error(src,"lisp exp miss");
				return false;
			}
			outopnd.vword=src.vword;
			outopnd.type=ptdi->type;
			return true;
		}
		if(cname==rppoptr(c_addr))
		{
			outopnd.vword=src.vword;
			outopnd.type="rp<void>";
			return true;
		}
		sh.error(src,"lisp exp miss");
		return false;
	}

	static void get_vsent(rbuf<rbuf<tword> >& vlisp,rbuf<tsent>& vsent,const tsent& src)
	{
		vsent.clear();
		for(int i=0;i<vlisp.count();i++)
		{
			tsent sent;
			sent.pos=src.pos;
			sent.vword=vlisp[i];
			vsent.push(sent);
		}
	}

	static rbool get_vlisp(tsh& sh,rbuf<tword> v,rbuf<rbuf<tword> >& vlisp)
	{
		vlisp.clear();
		for(int i=1;i<v.count()-1;i++)
		{
			if(v[i].val==rppoptr(c_mbk_l))
			{
				int right=sh.find_symm_mbk(v,i);
				if(right>=v.count())
				{
					return false;
				}
				vlisp+=v.sub(i,right+1);
				i=right;
			}
			else
			{
				rbuf<tword> temp;
				temp+=v[i];
				vlisp+=temp;
			}
		}
		return true;
	}
	
	//处理变参中括号调用,变参不能重载
	static rbool p_mbk_param(tsh& sh,tsent& src,tsent* pfirst,tsent& outopnd,
		tfunc& tfi,int level,int& i,tclass* ptci)
	{
		rstr cname=ptci->name;
		rstr fname=src.vword[i].val;
		int left=i+1;
		int right;
		right=sh.find_symm_mbk(src.vword,left);
		if(right>=src.vword.count())
		{
			sh.error(src,"call miss )");
			return false;
		}
		tfunc* ptfi=zfind::func_search(*ptci,fname);
		if(null==ptfi)
		{
			sh.error(src,"can't find call");
			return false;
		}
		rbuf<tsent> vsent;
		sh.split_param(vsent,src.vword.sub(left+1,right),src);
		for(int j=0;j<vsent.count();j++)
		{
			if(!p_exp(sh,vsent[j],tfi,level))
				return false;
		}
		tsent tmp;
		tmp.pos=src.pos;
		tmp.vword.push(tword(rstr(vsent.count())));
		vsent.push_front(tmp);
		//插入first还未处理引用的情况
		if(pfirst!=null)
		{
			vsent.push_front(*pfirst);//插入DOT前的对象
			if(!sh.is_point(pfirst->type))
			{
				vsent[0].vword.push_front(tword(rppoptr(c_addr)));
				if(!p_exp(sh,vsent[0],tfi,level))
				{
					return false;
				}
			}
		}
		tmp.vword.clear();
		tmp.vword+=tword(rppoptr(c_mbk_l));
		tmp.vword+=tword(rppoptr(c_addr));
		tmp.vword+=tword(ptci->name);
		tmp.vword+=tword(ptfi->name_dec);
		tmp.vword+=tword(rppoptr(c_mbk_r));

		outopnd.type=ptfi->retval.type;
		outopnd.vword+=tword(rppoptr(c_mbk_l));
		outopnd.vword+=tword(rppkey(c_pcall));
		outopnd.vword+=tword(outopnd.type);
		outopnd.vword+=tmp.vword;
		outopnd.vword+=tword(rppoptr(c_mbk_l));
		for(int j=0;j<vsent.count();j++)
		{
			outopnd.vword+=vsent[j].vword;
		}
		outopnd.vword+=tword(rppoptr(c_mbk_r));
		outopnd.vword+=tword(rppoptr(c_mbk_r));
		i=right;
		return true;
	}
	
	//无括号call
	static rbool p_call_n(tsh& sh,tsent& src,tsent* pfirst,tsent& outopnd,
		tfunc& tfi,int level,int& i,tclass* ptci)
	{
		rstr cname=ptci->name;
		rstr fname=src.vword[i].val;
		tfunc* ptfi;
		int left=i;
		int right=left+1;
		if(i+1==src.vword.count()-1||
			zfind::is_end_part_optr(sh,src.vword.get(i+1).val))
		{
			ptfi=zfind::func_search_empty_param_m(*ptci,fname,pfirst==null);
			if(ptfi==null)
			{
				sh.error(src,"can't find call");
				return false;
			}
		}
		else
		{
			ptfi=zfind::func_search_multi_param_m(*ptci,fname,pfirst==null);
			if(ptfi==null)
				ptfi=zfind::func_search_empty_param_m(*ptci,fname,pfirst==null);
			else
				right=src.vword.count()-1;
			if(ptfi==null)
			{
				sh.error(src,"can't find call");
				return false;
			}
		}
		tsent param=src.sub(left+1,right);
		outopnd.vword.push(tword(cname));
		outopnd.vword.push(tword(sh.m_optr[toptr::c_dot]));
		outopnd.vword.push(tword(fname));
		outopnd.vword.push(tword(rppoptr(c_sbk_l)));
		if(pfirst!=null&&!ptfi->is_friend)
		{
			outopnd.vword+=pfirst->vword;
			if(!param.empty())
			{
				outopnd.vword.push(tword(sh.m_optr[toptr::c_comma]));
				outopnd.vword+=param.vword;
			}
		}
		else
		{
			if(!param.empty())
			{
				outopnd.vword+=param.vword;
			}
		}
		outopnd.vword.push(tword(sh.m_optr[toptr::c_sbk_r]));
		//无括号调用添加括号递归处理
		if(!p_exp(sh,outopnd,tfi,level))
			return false;
		i=right-1;
		return true;
	}

	static rbool p_ftl(tsh& sh,tsent src,tsent* pfirst,tsent& outopnd,
		tfunc& tfi,int level,int& i,tclass* ptci)
	{
		int left=i+1;
		if(src.vword.get(left)!=rppoptr(c_tbk_l))
		{
			sh.error(src,"call miss <");
			return false;
		}
		int right=sh.find_symm_tbk(src.vword,left);
		if(right>=src.vword.count())
		{
			sh.error(src,"call miss >");
			return false;
		}
		rbuf<tword> temp=src.vword.sub(i,right+1);
		if(!zftl::ftl_replace(sh,*tfi.ptci,temp,ptci))
		{
			return false;
		}
		src.vword[right]=temp.get_bottom();
		i=right;
		return p_call(sh,src,pfirst,outopnd,tfi,level,i,ptci);
	}

	static rbool p_call(tsh& sh,tsent& src,tsent* pfirst,tsent& outopnd,
		tfunc& tfi,int level,int& i,tclass* ptci)
	{
		rstr cname=ptci->name;
		rstr fname=src.vword[i].val;
		if(src.vword.get(i+1).val==rppoptr(c_mbk_l))
		{
			return p_mbk_param(sh,src,pfirst,outopnd,tfi,level,i,ptci);
		}
		if(pfirst!=null&&sh.get_tname(pfirst->type)==rppkey(c_var)&&
			null==zfind::func_search(*ptci,fname))
		{
			return false;
			//a.test  ->   a["test"]
		}
		if(src.vword.get(i+1).val!=rppoptr(c_sbk_l))
		{
			return p_call_n(sh,src,pfirst,outopnd,tfi,level,i,ptci);
		}
		int left=i+1;
		int right;
		right=sh.find_symm_sbk(src.vword,left);
		if(right>=src.vword.count())
		{
			sh.error(src,"call miss )");
			return false;
		}
		rbuf<tsent> vsent;
		sh.split_param(vsent,src.vword.sub(left+1,right),src);
		for(int j=0;j<vsent.count();j++)
		{
			if(!p_exp(sh,vsent[j],tfi,level))
				return false;
		}
		if(pfirst!=null)
			vsent.push_front(*pfirst);//插入DOT前的对象
		tfunc* ptfi=zmatch::find_replace(sh,*ptci,fname,vsent);
		if(null==ptfi)
		{
			if(pfirst==null)
			{
				sh.error(src,"can't find call");
				return false;
			}
			vsent.pop_front();
			ptfi=zmatch::find_replace(sh,*ptci,fname,vsent);
			if(ptfi==null||!ptfi->is_friend)
			{
				sh.error(src,"can't find call");
				return false;
			}
		}
		set_func(sh,outopnd,vsent,ptfi);
		i=right;
		return true;
	}

	static rbool p_exp_all(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();++i)
		{
			if(sh.m_key.is_asm_ins(tfi.vsent[i].vword.get_bottom().val))
				continue;
			if(!zexp::p_exp(sh,tfi.vsent[i],tfi))
			{
				return false;
			}
		}
		return true;
	}

	static rbool p_exp(tsh& sh,tsent& src,tfunc& tfi,int level=0)
	{
		return p_exp(sh,src,src,tfi,level);
	}
	
	//表达式标准化，并设置dst.type
	static rbool p_exp(tsh& sh,tsent src,tsent& dst,tfunc& tfi,int level=0)
	{
		tclass& tci=*tfi.ptci;
		if(level++>c_rpp_deep)
		{
			sh.error(src,"expression too deep");
			return false;
		}
		dst.clear();
		rbuf<rstr> soptr;
		rbuf<tsent> sopnd;
		soptr.push(sh.m_optr[toptr::c_exp_part]);
		src.vword.push(tword(sh.m_optr[toptr::c_exp_part]));
		for(int i=0;i<src.vword.count();++i)
		{
			if(src.vword[i].val==sh.m_optr[toptr::c_exp_part]&&
				soptr.get_top()==sh.m_optr[toptr::c_exp_part])
				break;
			if(src.vword[i].is_const())
			{
				tsent outopnd;
				outopnd.pos=src.pos;
				outopnd.vword.push(src.vword[i]);
				outopnd.type=sh.get_ctype(sh,src.vword[i]);
				sopnd.push(outopnd);
			}
			elif(sh.m_key.is_asm_reg(src.vword[i].val))
			{
				tsent outopnd;
				outopnd.pos=src.pos;
				outopnd.vword.push(src.vword[i]);
				outopnd.type=sh.m_key[tkey::c_int];
				sopnd.push(outopnd);
			}
			elif(src.vword[i].val==rppoptr(c_sbk_l))
			{
				//todo,小括号重载有点复杂 
				int right=sh.find_symm_sbk(src.vword,i);
				if(right>=src.vword.count())
				{
					sh.error(src,"miss )");
					return false;
				}
				tsent outopnd;
				outopnd.pos=src.pos;
				if(!p_exp(sh,src.sub(i+1,right),outopnd,tfi,level))
					return false;
				sopnd.push(outopnd);
				i=right;
			}
			elif(src.vword.get(i+1)==rppoptr(c_mbk_l)&&
				zfind::is_class_t(sh,src.vword[i].val))
			{
				//[]函数指针调用
				tsent outopnd;
				outopnd.pos=src.pos;
				if(!p_point_call(sh,src,outopnd,tfi,level,i))
					return false;
				sopnd.push(outopnd);
			}
			elif(src.vword[i].val==sh.m_optr[toptr::c_mbk_l])
			{
				int right=sh.find_symm_mbk(src.vword,i);
				if(right>=src.vword.count())
				{
					sh.error(src,"miss ]");
					return false;
				}
				tsent outopnd;
				outopnd.pos=src.pos;
				if(sopnd.empty()||
					src.vword.get(i+1).val==rppoptr(c_addr)&&
					zfind::is_class(sh,src.vword.get(i+2).val))
				{
					if(!p_lisp(sh,src.sub(i,right+1),outopnd,tfi,level))
						return false;
				}
				else
				{
					if(!p_mbk(sh,sopnd.pop(),src.sub(i+1,right),outopnd,tfi,level))
						return false;
				}
				sopnd.push(outopnd);
				i=right;
			}
			elif(src.vword[i].val==sh.m_optr[toptr::c_dot])
			{
				if(sopnd.empty())
				{
					sh.error(src,". miss opnd");
					return false;
				}
				i++;
				rstr name=src.vword.get(i).val;
				tsent first=sopnd.pop();
				tclass* ptci=zfind::class_search_t(sh,first.type);
				if(null==ptci)
				{
					sh.error(src,". can't find class name");
					return false;
				}
				tsent outopnd;
				outopnd.pos=src.pos;
				//数据成员和函数成员不可以同名
				tdata* ptdi=zfind::data_member_search(*ptci,name);
				if(ptdi!=null)
				{
					outopnd.type=ptdi->type;
					outopnd.vword+=tword(rppoptr(c_mbk_l));
					outopnd.vword+=tword(rppoptr(c_dot));
					outopnd.vword+=first.vword;
					outopnd.vword+=tword(name);
					outopnd.vword+=tword(rppoptr(c_mbk_r));
					sopnd.push(outopnd);
					continue;
				}
				if(src.vword.get(i+1)==rppoptr(c_tbk_l)&&
					zfind::ftl_search(*ptci,name)!=null)
				{
					if(!p_ftl(sh,src,&first,outopnd,tfi,level,i,ptci))
						return false;
				}
				else
				{
					if(!p_call(sh,src,&first,outopnd,tfi,level,i,ptci))
						return false;
				}
				sopnd.push(outopnd);
			}
			elif(src.vword[i].val==sh.m_optr[toptr::c_arrow_r])
			{
				if(sopnd.empty())
				{
					sh.error(src,". miss opnd");
					return false;
				}
				tsent first=sopnd.pop();
				first.vword.push_front(tword(sh.m_optr[toptr::c_star]));
				if(!p_exp(sh,first,tfi,level))
					return false;
				sopnd.push(first);
				src.vword[i].val=sh.m_optr[toptr::c_dot];
				i--;
			}
			elif(sh.m_optr.is_optr(src.vword[i].val))
			{
				if(soptr.empty())
				{
					sh.error(src,"exp miss optr");
					return false;
				}
				rstr cur=src.vword[i].val;
				if(!sh.m_optr.precede(soptr.top(),cur))
				{
					soptr.push(cur);
					continue;
				}
				rstr theta=soptr.pop();
				if(sopnd.empty())
				{
					sh.error(src,"exp miss opnd");
					return false;
				}
				rbuf<tsent> vsent;
				vsent.push(sopnd.pop());
				tclass* ptci=zfind::class_search_t(sh,vsent[0].type);
				tfunc* ptfi;
				tsent outopnd;
				outopnd.pos=src.pos;
				if(ptci!=null)
				{
					ptfi=zmatch::find_replace(sh,*ptci,theta,vsent);
					if(ptfi!=null)
					{
						set_func(sh,outopnd,vsent,ptfi);
						sopnd.push(outopnd);
						i--;
						continue;
					}
				}
				if(sopnd.empty())
				{
					sh.error(src,"exp miss opnd");
					return false;
				}
				vsent.push_front(sopnd.pop());
				ptci=zfind::class_search_t(sh,vsent[0].type);
				if(ptci!=null)
				{
					ptfi=zmatch::find_replace(sh,*ptci,theta,vsent);
					if(ptfi!=null)
					{
						set_func(sh,outopnd,vsent,ptfi);
						sopnd.push(outopnd);
						i--;
						continue;
					}
				}
				ptci=zfind::class_search_t(sh,vsent[1].type);
				if(ptci!=null)
				{
					ptfi=zmatch::find_replace(sh,*ptci,theta,vsent);
					if(ptfi!=null)
					{
						set_func(sh,outopnd,vsent,ptfi);
						sopnd.push(outopnd);
						i--;
						continue;
					}
				}
				sh.error(src,"exp miss opnd 2");
				return false;
			}
			elif(zfind::is_class(sh,src.vword[i].val))
			{
				tsent outopnd;
				outopnd.pos=src.pos;
				if(src.vword.get(i+1).val==rppoptr(c_sbk_l))
				{
					//临时变量
					if(!p_temp_var(sh,src,outopnd,tfi,level,i))
					{
						return false;
					}
				}
				else
				{
					//类名直接调用
					if(!p_class_call(sh,src,outopnd,tfi,level,i))
						return false;
				}
				sopnd.push(outopnd);
			}
			else
			{
				rstr name=src.vword[i].val;
				tsent outopnd;
				outopnd.pos=src.pos;
				tdata* ptdi=zfind::local_search(tfi,name);
				if(ptdi!=null)
				{
					outopnd.vword+=src.vword[i];
					outopnd.type=ptdi->type;
					sopnd.push(outopnd);
					continue;
				}
				if(!tci.is_friend)
				{
					ptdi=zfind::data_member_search(tci,name);
					if(ptdi!=null)
					{
						//类成员变量增加this，如 m_a -> this.m_a 
						outopnd.vword+=tword(rppoptr(c_mbk_l));
						outopnd.vword+=tword(rppoptr(c_dot));
						outopnd.vword+=tword(rppkey(c_this));
						outopnd.vword+=src.vword[i];
						outopnd.vword+=tword(rppoptr(c_mbk_r));
						outopnd.type=ptdi->type;
						sopnd.push(outopnd);
						continue;
					}
				}
				ptdi=zfind::data_member_search(*sh.m_main,name);
				if(ptdi!=null)
				{
					//全局变量
					outopnd.vword+=tword(rppoptr(c_mbk_l));
					outopnd.vword+=tword(rppoptr(c_dot));
					outopnd.vword+=tword(rppkey(c_pmain));
					outopnd.vword+=src.vword[i];
					outopnd.vword+=tword(rppoptr(c_mbk_r));
					outopnd.type=ptdi->type;
					sopnd.push(outopnd);
					continue;
				}
				if(!p_func_call(sh,src,outopnd,tfi,level,i))
					return false;
				sopnd.push(outopnd);
			}
		}
		if(sopnd.count()!=1)
		{
			src.vword.print();
			sh.error(src,"expression error");
			return false;
		}
		dst=sopnd[0];
		if(dst.vword.empty()||dst.type.empty()||dst.pos.line==0)
		{
			sh.error(src,"expression error");
			return false;
		}
		return true;
	}

	static void set_func(tsh& sh,tsent& outopnd,rbuf<tsent>& vsent,tfunc* ptfi)
	{
		tclass* ptci=ptfi->ptci;
		outopnd.type=ptfi->retval.type;
		outopnd.vword+=tword(rppoptr(c_mbk_l));
		outopnd.vword+=tword(ptci->name);
		outopnd.vword+=tword(ptfi->name_dec);
		outopnd.vword+=tword(rppoptr(c_mbk_l));
		for(int i=0;i<vsent.count();i++)
		{
			outopnd.vword+=vsent[i].vword;
		}
		outopnd.vword+=tword(rppoptr(c_mbk_r));
		outopnd.vword+=tword(rppoptr(c_mbk_r));
	}
};

#endif