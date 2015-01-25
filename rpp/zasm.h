#ifndef zasm_h__
#define zasm_h__

#include "zopt.h"

//生成汇编代码
//tfunc.vsent->vasm
//返回指针或引用或int的表达式需要放入ebx中
//指针或int的引用是对本身的值进行判断
//其它类型的引用是对引用进行判断
struct zasm
{
	static rbool proc_func(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();i++)
		{
			rbuf<tasm> vasm;
			if(!proc_ret(sh,tfi.vsent[i],vasm,tfi))
			{
				sh.error(tfi.vsent[i],"build asm error");
				return false;
			}
			if(rppconf(c_op_match))
			{
				zopt::op_match(sh,vasm);
			}
			if(rppconf(c_op_merge))
				zopt::op_add_sub(sh,vasm);
			for(int j=0;j<vasm.count();j++)
			{
				vasm[j].pos=tfi.vsent[i].pos;
			}
			tfi.vasm+=vasm;
		}
		add_asm_ebp(sh,tfi);
		return true;
	}

	static void push_double(tsh& sh,rbuf<tasm>& vasm,double dval)
	{
		push_asm(vasm,rppkey(c_push),rstr(*((uint*)&dval+1)));
		push_asm(vasm,rppkey(c_push),rstr(*((uint*)&dval)));
	}

	static rbool add_asm_ebp(tsh& sh,tfunc& tfi)
	{
		rbuf<tasm> vasm;
		tasm item;
		item.pos=tfi.first_pos;
		int size=zfind::get_func_local_size(tfi);
		ifn(rppconf(c_op_zero)&&size==0)
		{
			item.vstr.clear();
			item.vstr.push(rppkey(c_sub));
			item.vstr.push(rppkey(c_esp));
			item.vstr.push(rppoptr(c_comma));
			item.vstr.push(rstr(size));
			vasm.push(item);
		}
		rbool b_use_ebp=use_ebp(sh,tfi.vasm);
		if(b_use_ebp)
		{
			item.vstr.clear();
			item.vstr.push(rppkey(c_push));
			item.vstr.push(rppkey(c_ebp));
			vasm.push(item);

			item.vstr.clear();
			item.vstr.push(rppkey(c_mov));
			item.vstr.push(rppkey(c_ebp));
			item.vstr.push(rppoptr(c_comma));
			item.vstr.push(rppkey(c_esp));
			vasm.push(item);
		}
		if(tfi.is_cfunc)
		{
			//todo 如果ebx、esi、edi未修改则不用保护
			item.vstr.clear();
			item.vstr.push(rppkey(c_push));
			item.vstr.push(rppkey(c_ebx));
			vasm.push(item);

			item.vstr.clear();
			item.vstr.push(rppkey(c_push));
			item.vstr.push(rppkey(c_esi));
			vasm.push(item);

			item.vstr.clear();
			item.vstr.push(rppkey(c_push));
			item.vstr.push(rppkey(c_edi));
			vasm.push(item);
		}
		vasm+=tfi.vasm;
		item.pos=tfi.last_pos;
		if(tfi.is_cfunc)
		{
			item.vstr.clear();
			item.vstr.push(rppkey(c_pop));
			item.vstr.push(rppkey(c_edi));
			vasm.push(item);

			item.vstr.clear();
			item.vstr.push(rppkey(c_pop));
			item.vstr.push(rppkey(c_esi));
			vasm.push(item);

			item.vstr.clear();
			item.vstr.push(rppkey(c_pop));
			item.vstr.push(rppkey(c_ebx));
			vasm.push(item);
		}
		if(b_use_ebp)
		{
			item.vstr.clear();
			item.vstr.push(rppkey(c_pop));
			item.vstr.push(rppkey(c_ebp));
			vasm.push(item);
		}
		ifn(rppconf(c_op_zero)&&size==0)
		{
			item.vstr.clear();
			item.vstr.push(rppkey(c_add));
			item.vstr.push(rppkey(c_esp));
			item.vstr.push(rppoptr(c_comma));
			item.vstr.push(rstr(size));
			vasm.push(item);
		}
		size=zfind::get_func_param_size(tfi);
		if(size>0)
		{
			item.vstr.clear();
			item.vstr.push(rppkey(c_reti));
			item.vstr.push(rstr(size));
			vasm.push(item);
		}
		else
		{
			item.vstr.clear();
			item.vstr.push(rppkey(c_retn));
			vasm.push(item);
		}
		tfi.vasm=r_move(vasm);
		return true;
	}

	static rbool use_ebp(tsh& sh,rbuf<tasm>& vasm)
	{
		if(!rppconf(c_op_ebp))
			return true;
		for(int i=0;i<vasm.count();i++)
			for(int j=0;j<vasm[i].vstr.count();j++)
				if(vasm[i].vstr[j]==rppkey(c_ebp))
					return true;
		return false;
	}

	static rbool proc_ret(tsh& sh,tsent& src,rbuf<tasm>& vasm,
		tfunc& tfi,int level=0)
	{
		rstr name=src.vword.get_bottom().val;
		if(sh.m_key.is_asm_ins(name))
		{
			push_asm(vasm,sh.vword_to_vstr(src.vword));
			return true;
		}
		//返回常量
		if(src.vword.count()==1)
		{
			if(src.vword[0].is_cint())
			{
				//经测试如果是小程序rppkey(c_mov)比"mov"效率高
				push_asm(vasm,rppkey(c_mov),rppkey(c_ebx),
					rppoptr(c_comma),src.vword[0].val);
				return true;
			}
			elif(sh.m_key.is_asm_reg(src.vword[0].val))
			{
				push_asm(vasm,rppkey(c_mov),rppkey(c_ebx),
					rppoptr(c_comma),src.vword[0].val);
				return true;
			}
			elif(src.vword[0].is_cpoint())
			{
				if(src.vword[0].is_caddr())
				{
					push_asm(vasm,rppkey(c_mov),
						rppkey(c_ebx),rppoptr(c_comma),
						src.vword[0].val);
				}
				else
				{
					push_asm(vasm,rppkey(c_mov),
						rppkey(c_ebx),rppoptr(c_comma),
						get_int(src.vword[0].val));
				}
				return true;
			}
		}
		if(src.vword.count()==7&&src.vword[1].val==rppoptr(c_addr))
		{
			push_asm(vasm,rppkey(c_mov),rppkey(c_ebx),rppoptr(c_comma),
				rppoptr(c_mbk_l),rppoptr(c_addr),rppoptr(c_comma),
				src.vword[3].val,rppoptr(c_comma),src.vword[5].val,
				rppoptr(c_mbk_r));
			return true;
		}
		//返回变量
		tdata* ptdi=zfind::local_search(tfi,
			get_src_in(sh,src).vword.get(0).val);
		if(ptdi!=null)
		{
			if(zfind::is_type_mebx(sh,src.type))
			{
				push_asm(vasm,rppkey(c_lea),rppkey(c_esi),
					rppoptr(c_comma),rppoptr(c_mbk_l),
					rppkey(c_ebp),rppoptr(c_plus),
					rstr(ptdi->off),rppoptr(c_mbk_r));
				//这里还可以优化
				if(!add_esi(sh,ptdi->type,vasm,src))
					return false;
				push_asm(vasm,rppkey(c_mov),rppkey(c_ebx),
					rppoptr(c_comma),rppoptr(c_mbk_l),
					rppkey(c_esi),rppoptr(c_mbk_r));
				if(sh.is_quote(src.type)&&
					(sh.get_tname(src.type)==rppkey(c_int)||
					sh.is_point(sh.get_tname(src.type))))
				{
					push_asm(vasm,rppkey(c_mov),rppkey(c_ebx),
						rppoptr(c_comma),rppoptr(c_mbk_l),
						rppkey(c_ebx),rppoptr(c_mbk_r));
				}
			}
			return true;
		}
		//返回函数
		return proc_ret_func(sh,src,vasm,tfi,level);
	}

	static rbool proc_ret_func(tsh& sh,tsent& src,rbuf<tasm>& vasm,
		tfunc& tfi,int level)
	{
		tdata retval;
		if(!a_exp(sh,get_src_in(sh,src),vasm,retval,tfi,level))
		{
			return false;
		}
		int size;
		//多数函数返回void，因此加上这句判断可以提高效率
		if(src.type!=rppkey(c_void)&&zfind::is_type_mebx(sh,src.type))
		{
			push_asm(vasm,rppkey(c_mov),rppkey(c_esi),
				rppoptr(c_comma),rppkey(c_esp));
			int cur=vasm.count();
			if(!add_esi(sh,retval.type,vasm,src))
				return false;
			if(cur==vasm.count())
			{
				vasm.pop();
				push_asm(vasm,rppkey(c_mov),rppkey(c_ebx),
					rppoptr(c_comma),rppoptr(c_mbk_l),
					rppkey(c_esp),rppoptr(c_mbk_r));
			}
			else
			{
				push_asm(vasm,rppkey(c_mov),rppkey(c_ebx),
					rppoptr(c_comma),rppoptr(c_mbk_l),
					rppkey(c_esi),rppoptr(c_mbk_r));
			}
			if(sh.is_quote(src.type)&&
				(sh.get_tname(src.type)==rppkey(c_int)||
				sh.is_point(sh.get_tname(src.type))))
			{
				push_asm(vasm,rppkey(c_mov),rppkey(c_ebx),
					rppoptr(c_comma),rppoptr(c_mbk_l),
					rppkey(c_ebx),rppoptr(c_mbk_r));
			}
		}
		if(!destruct_ret(sh,retval,vasm))
		{
			return false;
		}
		size=zfind::get_ceil_space(retval);
		push_asm(vasm,rppkey(c_add),rppkey(c_esp),rppoptr(c_comma),size);
		return true;
	}
	
	//解析表达式生成汇编代码
	static rbool a_exp(tsh& sh,const tsent& src,rbuf<tasm>& vasm,tdata& retval,
		tfunc& tfi,int level=0)
	{
		if(level>c_rpp_deep)
		{
			return false;
		}
		level++;
		if(src.vword.get_bottom().val!=rppoptr(c_mbk_l)||
			src.vword.get_top().val!=rppoptr(c_mbk_r))
		{
			return false;
		}
		if(src.vword.get(1).val==rppoptr(c_dot))
		{
			return false;
		}
		int size;
		rbuf<rbuf<tword> > vlisp;
		zexp::get_vlisp(sh,src.vword,vlisp);
		if(src.vword.get(1).val==rppkey(c_pcall))
		{
			if(vlisp.count()!=4)
			{
				return false;
			}
			retval.type=vlisp[1].get(0).val;
			retval.size=zfind::get_type_size(sh,retval.type);
			size=zfind::get_ceil_space(retval);
			push_asm(vasm,rppkey(c_sub),rppkey(c_esp),
				rppoptr(c_comma),size);
			rbuf<rbuf<tword> > temp_v;
			zexp::get_vlisp(sh,vlisp[3],temp_v);
			rbuf<tsent> vsent;
			zexp::get_vsent(temp_v,vsent,src);
			for(int i=vsent.count()-1;i>=0;i--)
			{
				if(!zexp::p_exp(sh,vsent[i],tfi,level,null))
				{
					return false;
				}
			}
			for(int i=vsent.count()-1;i>=0;i--)
			{
				tdata tdi;
				tdi.type=sh.get_tname(vsent[i].type);
				tdi.size=zfind::get_type_size(sh,tdi.type);
				if(!pass_param(sh,vsent[i],tdi,vasm,tfi,level))
					return false;
			}
			tsent sent=src;
			sent.vword=vlisp[2];
			if(!zexp::p_exp(sh,sent,tfi,level,null))
			{
				return false;
			}
			if(!proc_ret(sh,sent,vasm,tfi,level))
				return false;
			if(sent.type==rstr("rp<void>"))
			{
				push_asm(vasm,rppkey(c_call),rppkey(c_ebx));
			}
			else
				return false;
			return true;
		}
		if(vlisp.count()!=3)
		{
			return false;
		}
		zexp::get_vlisp(sh,vlisp[2],vlisp);
		rbuf<tsent> vsent;
		zexp::get_vsent(vlisp,vsent,src);
		for(int i=vsent.count()-1;i>=0;i--)
		{
			if(!zexp::p_exp(sh,vsent[i],tfi,level,null))
			{
				return false;
			}
		}
		rstr cname=src.vword.get(1).val;
		rstr fname=src.vword.get(3).val;//p_exp已经合并了name_dec
		tclass* ptci=zfind::class_search(sh,cname);
		if(null==ptci)
		{
			return false;
		}
		tfunc* ptfi=zfind::func_search_dec(*ptci,fname);
		if(ptfi==null)
		{
			return false;
		}
		retval=ptfi->retval;
		if(rppconf(c_op_empty_func)&&
			zfind::is_empty_struct_type(sh,ptci->name)&&
			(zfind::is_destruct(sh,*ptfi)||
			zfind::is_emptystruct(sh,*ptfi)))
			return true;
		if(ptfi->param.count()!=vsent.count())
		{
			return false;
		}
		size=zfind::get_ceil_space(retval);
		push_asm(vasm,rppkey(c_sub),rppkey(c_esp),rppoptr(c_comma),size);
		for(int i=vsent.count()-1;i>=0;i--)
		{
			if(!pass_param(sh,vsent[i],ptfi->param[i],vasm,tfi,level))
			{
				return false;
			}
		}
		push_asm(vasm,sh.get_func_declare_call(sh,*ptci,*ptfi));
		return true;
	}

	static rbool pass_param(tsh& sh,tsent& src,tdata& dst,rbuf<tasm>& vasm,
		tfunc& tfi,int level)
	{
		if(src.vword.empty())
			return false;
		int size;
		tsent src_in=get_src_in(sh,src);
		if(src_in.vword.get(1).val==rppkey(c_pcall)||
			zfind::is_class(sh,src_in.vword.get(1).val))
		{
			//先申请参数空间
			size=zfind::get_ceil_space(dst);
			push_asm(vasm,rppkey(c_sub),rppkey(c_esp),
				rppoptr(c_comma),size);
			//递归处理子表达式
			tdata retval;
			if(!a_exp(sh,src_in,vasm,retval,tfi,level))
				return false;
			//获取传递参数的地址分别放入esi和edi中，
			//包括函数返回后再dot，如int.get().m_in
			if(!obtain_var_addr_f(sh,retval,src,vasm))
				return false;
			//传递参数
			if(!copy_param(sh,src.type,dst.type,vasm))
				return false;
			//析构返回值
			if(!destruct_ret(sh,retval,vasm))
			{
				return false;
			}
			//释放栈
			size=zfind::get_ceil_space(retval);
			push_asm(vasm,rppkey(c_add),rppkey(c_esp),
				rppoptr(c_comma),size);
		}
		elif(src.vword[0].is_cint())
		{
			push_asm(vasm,rppkey(c_push),src.vword[0].val);
		}
		elif(src.vword[0].is_cuint())
		{
			push_asm(vasm,rppkey(c_push),get_int(src.vword[0].val));
		}
		elif(src.vword[0].is_cdouble())
		{
			double dval=src.vword[0].val.todouble();
			push_double(sh,vasm,dval);
		}
		elif(src.vword.count()==7&&
			src.vword.get(1).val==rppoptr(c_addr)&&
			src.vword[0].val==rppoptr(c_mbk_l))
		{
			rbuf<rstr> vtemp=sh.vword_to_vstr(src.vword);
			vtemp.push_front(rppkey(c_push));
			push_asm(vasm,vtemp);
		}
		elif(src.vword[0].is_cpoint())
		{
			if(src.vword.get_bottom().is_caddr())
				push_asm(vasm,rppkey(c_push),src.vword[0].val);
			else
				push_asm(vasm,rppkey(c_push),get_int(src.vword[0].val));
		}
		elif(src.vword[0].is_cstr())
		{
			push_asm(vasm,rppkey(c_push),src.vword[0].val);
		}
		elif(sh.m_key.is_asm_reg(src.vword[0].val))
		{
			push_asm(vasm,rppkey(c_push),src.vword[0].val);
		}
		else
		{
			rstr name=src_in.vword.get(0).val;
			tdata* ptdi=zfind::local_search(tfi,name);
			if(ptdi==null)
				return false;
			int cur=vasm.count()+3;
			if(!obtain_var_addr_var(sh,src,dst,ptdi,vasm))
				return false;
			if(cur==vasm.count()&&rppconf(c_op_pass))
			{
				if(zfind::is_op_pass_type(sh,dst.type)&&
					dst.type==src.type)
				{
					vasm.m_count-=3;
					push_asm(vasm,rppkey(c_push),rppoptr(c_mbk_l),
						rppkey(c_ebp),rppoptr(c_plus),
						rstr(ptdi->off),rppoptr(c_mbk_r));
					return true;
				}
				if(sh.is_quote(dst.type)&&sh.is_quote(src.type))
				{
					vasm.m_count-=3;
					push_asm(vasm,rppkey(c_push),rppoptr(c_mbk_l),
						rppkey(c_ebp),rppoptr(c_plus),
						rstr(ptdi->off),rppoptr(c_mbk_r));
					return true;
				}
				if(sh.is_quote(dst.type))
				{
					vasm.m_count-=3;
					push_asm(vasm,rppkey(c_lea),rppkey(c_esi),
						rppoptr(c_comma),rppoptr(c_mbk_l),
						rppkey(c_ebp),rppoptr(c_plus),
						rstr(ptdi->off),rppoptr(c_mbk_r));
					push_asm(vasm,rppkey(c_push),rppkey(c_esi));
					return true;
				}
			}
			//调用拷贝构造函数时还可以优化
			if(!copy_param(sh,src.type,dst.type,vasm))
				return false;
			return true;
		}
		return true;
	}

	static rbool obtain_var_addr_var(tsh& sh,tsent& src,tdata& dst,
		tdata* ptdi,rbuf<tasm>& vasm)
	{
		int size=zfind::get_ceil_space(dst);
		push_asm(vasm,rppkey(c_sub),rppkey(c_esp),rppoptr(c_comma),size);
		push_asm(vasm,rppkey(c_mov),rppkey(c_edi),rppoptr(c_comma),
			rppkey(c_esp));
		push_asm(vasm,rppkey(c_lea),rppkey(c_esi),rppoptr(c_comma),
			rppoptr(c_mbk_l),rppkey(c_ebp),rppoptr(c_plus),
			rstr(ptdi->off),rppoptr(c_mbk_r));
		if(!add_esi(sh,ptdi->type,vasm,src))
			return false;
		return true;
	}

	static rbool obtain_var_addr_f(tsh& sh,tdata& retval,
		tsent& src,rbuf<tasm>& vasm)
	{
		int size=zfind::get_ceil_space(retval);
		push_asm(vasm,rppkey(c_lea),rppkey(c_edi),rppoptr(c_comma),
			rppoptr(c_mbk_l),rppkey(c_esp),rppoptr(c_plus),
			rstr(size),rppoptr(c_mbk_r));
		push_asm(vasm,rppkey(c_mov),rppkey(c_esi),rppoptr(c_comma),
			rppkey(c_esp));
		if(!add_esi(sh,retval.type,vasm,src))
			return false;
		return true;
	}

	static tsent get_src_in(tsh& sh,tsent& src)
	{
		if(src.vword.get(1).val!=rppoptr(c_dot))
		{
			return src;
		}
		tsent temp=src;
		int start=0;
		rbuf<tword>& v=src.vword;
		while(start+2<v.count()&&
			  v[start].val==rppoptr(c_mbk_l)&&
			  v[start+1].val==rppoptr(c_dot))
		{
			start+=3;
		}
		if(v[start].val==rppoptr(c_mbk_l))
		{
			int right=sh.find_symm_mbk(v,start);
			temp.vword=v.sub(start,right+1);
		}
		else
		{
			temp.vword=v.sub(start,start+1);
		}
		return r_move(temp);
	}
	
	// [ . , [ . , [ . , a , b ] , c ] , d ]
	static rbool add_esi(tsh& sh,rstr type,rbuf<tasm>& vasm,tsent& src)
	{
		int start=0;
		rbuf<tword>& v=src.vword;
		if(v.get(1).val!=rppoptr(c_dot))
		{
			return true;
		}
		while(start+2<v.count()&&
			v[start].val==rppoptr(c_mbk_l)&&
			v[start+1].val==rppoptr(c_dot))
		{
			start+=3;
		}
		int right=start+2;
		if(v[start].val==rppoptr(c_mbk_l))
		{
			right=sh.find_symm_mbk(v,start);
			if(right>=v.count())
			{
				return false;
			}
			right+=2;
		}
		for(int i=0;i<start;i+=3)
		{
			tclass* ptci=zfind::class_search_t(sh,type);
			if(null==ptci)
			{
				return false;
			}
			tdata* ptdi=zfind::data_member_search(*ptci,v.get(right).val);
			if(null==ptdi)
			{
				return false;
			}
			if(sh.is_quote(type))
				push_asm(vasm,rppkey(c_mov),rppkey(c_esi),
					rppoptr(c_comma),rppoptr(c_mbk_l),
					rppkey(c_esi),rppoptr(c_mbk_r));
			if(ptdi->off!=0)
			{
				push_asm(vasm,rppkey(c_add),rppkey(c_esi),
					rppoptr(c_comma),ptdi->off);
			}
			type=ptdi->type;
			right+=3;
		}
		return true;
	}

	//src的真实地址已经存储在esi中,dst的地址在edi中
	static rbool copy_param(tsh& sh,const rstr& src,
		const rstr& dst,rbuf<tasm>& vasm)
	{
		//指针可以随意转换
		if(sh.is_point(dst)&&sh.is_point(src)||
			dst==rppkey(c_rd4)&&zfind::get_type_size(sh,src)==4||
			dst==rppkey(c_rcs)&&sh.is_point(src))
		{
			push_asm(vasm,rppkey(c_mov),rppoptr(c_mbk_l),
				rppkey(c_edi),rppoptr(c_mbk_r),
				rppoptr(c_comma),rppoptr(c_mbk_l),
				rppkey(c_esi),rppoptr(c_mbk_r));
			return true;
		}
		//目标是引用
		if(sh.is_quote(dst))
		{
			if(sh.is_quote(src))
			{
				//源是引用
				push_asm(vasm,rppkey(c_mov),rppoptr(c_mbk_l),
					rppkey(c_edi),rppoptr(c_mbk_r),
					rppoptr(c_comma),rppoptr(c_mbk_l),
					rppkey(c_esi),rppoptr(c_mbk_r));
			}				
			else
			{
				//源是对象
				push_asm(vasm,rppkey(c_mov),rppoptr(c_mbk_l),
					rppkey(c_edi),rppoptr(c_mbk_r),
					rppoptr(c_comma),rppkey(c_esi));
			}
			return true;
		}
		if(rppconf(c_op_pass))
		{
			if(zfind::is_op_pass_type(sh,dst))
			{
				if(sh.is_quote(src))
				{
					//源是引用
					push_asm(vasm,rppkey(c_mov),rppkey(c_esi),
						rppoptr(c_comma),rppoptr(c_mbk_l),
						rppkey(c_esi),rppoptr(c_mbk_r));
				}
				push_asm(vasm,rppkey(c_mov),rppoptr(c_mbk_l),
					rppkey(c_edi),rppoptr(c_mbk_r),
					rppoptr(c_comma),rppoptr(c_mbk_l),
					rppkey(c_esi),rppoptr(c_mbk_r));
				return true;
			}
			if(dst==rppkey(c_double)||dst==rppkey(c_int8))
			{
				if(sh.is_quote(src))
				{
					push_asm(vasm,rppkey(c_mov),rppkey(c_esi),
						rppoptr(c_comma),rppoptr(c_mbk_l),
						rppkey(c_esi),rppoptr(c_mbk_r));
				}
				push_asm(vasm,rppkey(c_mov),rppoptr(c_mbk_l),
					rppkey(c_edi),rppoptr(c_mbk_r),
					rppoptr(c_comma),rppoptr(c_mbk_l),
					rppkey(c_esi),rppoptr(c_mbk_r));
				push_asm(vasm,rppkey(c_mov),rppoptr(c_mbk_l),
					rppkey(c_edi),rppoptr(c_plus),"4",
					rppoptr(c_mbk_r),rppoptr(c_comma),
					rppoptr(c_mbk_l),rppkey(c_esi),
					rppoptr(c_plus),"4",rppoptr(c_mbk_r));
				return true;
			}
		}
		//目标是对象需要调用拷贝构造函数
		if(sh.is_quote(src))
		{
			//源是引用
			push_asm(vasm,rppkey(c_push),rppoptr(c_mbk_l),
				rppkey(c_esi),rppoptr(c_mbk_r));
		}
		else
		{
			//源是对象
			push_asm(vasm,rppkey(c_push),rppkey(c_esi));
		}
		push_asm(vasm,rppkey(c_push),rppkey(c_edi));
		tclass* ptci=zfind::class_search_t(sh,src);
		if(ptci==null)
			return false;
		tfunc* pcopystruct=zfind::copystruct_search(*ptci);
		if(pcopystruct==null)
			return false;
		push_asm(vasm,sh.get_func_declare_call(sh,*ptci,*pcopystruct));
		return true;
	}

	static rbool destruct_ret(tsh& sh,tdata& retval,rbuf<tasm>& vasm)
	{
		rstr type=retval.type;
		if(sh.is_quote(type))
			return true;
		if(type==rppkey(c_void))
			return true;
		tclass* ptci=zfind::class_search(sh,type);
		if(ptci==null)
			return false;
		tfunc* pdestruct=zfind::destruct_search(*ptci);
		if(pdestruct==null)
			return false;
		if(rppconf(c_op_empty_func)&&zfind::is_empty_struct_type(sh,type))
			return true;
		push_asm(vasm,rppkey(c_mov),rppkey(c_esi),
			rppoptr(c_comma),rppkey(c_esp));
		push_asm(vasm,rppkey(c_push),rppkey(c_esi));
		push_asm(vasm,sh.get_func_declare_call(sh,*ptci,*pdestruct));
		return true;
	}

	static rstr get_int(rstr& s)
	{
		return s.sub(0,s.count()-1);
	}

	static void push_asm(rbuf<tasm>& vasm,const rbuf<rstr>& vstr)
	{
		tasm item;
		item.vstr=vstr;
		vasm.push_move(item);
	}

	//todo 这里无法使用变参函数，可以使用变参宏或模板元简化代码
	static void push_asm(rbuf<tasm>& vasm,
		const rstr& s1,const rstr& s2,const rstr& s3,const rstr& s4,
		const rstr& s5,const rstr& s6,const rstr& s7,const rstr& s8,
		const rstr& s9,const rstr& s10,const rstr& s11,const rstr& s12)
	{
		tasm item;
		item.vstr.push(s1);
		item.vstr.push(s2);
		item.vstr.push(s3);
		item.vstr.push(s4);
		item.vstr.push(s5);
		item.vstr.push(s6);
		item.vstr.push(s7);
		item.vstr.push(s8);
		item.vstr.push(s9);
		item.vstr.push(s10);
		item.vstr.push(s11);
		item.vstr.push(s12);
		vasm.push_move(item);
	}

	static void push_asm(rbuf<tasm>& vasm,
		const rstr& s1,const rstr& s2,const rstr& s3,const rstr& s4,
		const rstr& s5,const rstr& s6,const rstr& s7,const rstr& s8,
		const rstr& s9,const rstr& s10)
	{
		tasm item;
		item.vstr.push(s1);
		item.vstr.push(s2);
		item.vstr.push(s3);
		item.vstr.push(s4);
		item.vstr.push(s5);
		item.vstr.push(s6);
		item.vstr.push(s7);
		item.vstr.push(s8);
		item.vstr.push(s9);
		item.vstr.push(s10);
		vasm.push_move(item);
	}

	static void push_asm(rbuf<tasm>& vasm,const rstr& s1,const rstr& s2,
		const rstr& s3,const rstr& s4,const rstr& s5,const rstr& s6,
		const rstr& s7,const rstr& s8)
	{
		tasm item;
		item.vstr.push(s1);
		item.vstr.push(s2);
		item.vstr.push(s3);
		item.vstr.push(s4);
		item.vstr.push(s5);
		item.vstr.push(s6);
		item.vstr.push(s7);
		item.vstr.push(s8);
		vasm.push_move(item);
	}

	static void push_asm(rbuf<tasm>& vasm,const rstr& s1,const rstr& s2,
		const rstr& s3,const rstr& s4,const rstr& s5,const rstr& s6,
		const rstr& s7)
	{
		tasm item;
		item.vstr.push(s1);
		item.vstr.push(s2);
		item.vstr.push(s3);
		item.vstr.push(s4);
		item.vstr.push(s5);
		item.vstr.push(s6);
		item.vstr.push(s7);
		vasm.push_move(item);
	}

	static void push_asm(rbuf<tasm>& vasm,const rstr& s1,const rstr& s2,
		const rstr& s3,const rstr& s4,const rstr& s5,const rstr& s6)
	{
		tasm item;
		item.vstr.push(s1);
		item.vstr.push(s2);
		item.vstr.push(s3);
		item.vstr.push(s4);
		item.vstr.push(s5);
		item.vstr.push(s6);
		vasm.push_move(item);
	}

	static void push_asm(rbuf<tasm>& vasm,const rstr& s1,const rstr& s2,
		const rstr& s3,const rstr& s4)
	{
		tasm item;
		item.vstr.push(s1);
		item.vstr.push(s2);
		item.vstr.push(s3);
		item.vstr.push(s4);
		vasm.push_move(item);
	}

	static void push_asm(rbuf<tasm>& vasm,const rstr& s1,const rstr& s2)
	{
		tasm item;
		item.vstr.push(s1);
		item.vstr.push(s2);
		vasm.push_move(item);
	}

	static void push_asm(rbuf<tasm>& vasm,const rstr& s1)
	{
		tasm item;
		item.vstr.push(s1);
		vasm.push_move(item);
	}
};

#endif