#ifndef zmemb_h__
#define zmemb_h__

//提取类内部的成员，函数默认参数处理
//zclass::vword->vdata,vfunc,vmac
struct zmemb
{
	static rbool recursion_get_size(tsh& sh,tclass& tci,int level=0)
	{
		if(level>c_rpp_deep)
		{
			sh.error(rstr("level overflow ")+tci.name);
			return false;
		}
		level++;
		if(tci.size>0||tci.size==0&&tci.vdata.empty())
		{
			return true;
		}	
		int size=0;
		for(int i=0;i<tci.vdata.count();i++)
		{
			if(sh.is_point_quote(tci.vdata[i].type))
			{
				tci.vdata[i].off=size;
				tci.vdata[i].size=c_point_size;
				size+=tci.vdata[i].size*tci.vdata[i].count;
			}
			else
			{
				tclass* ptci=zfind::class_search(sh,tci.vdata[i].type);
				if(null==ptci)
				{
					sh.error(rstr("can't find ")+tci.vdata[i].type);
					return false;
				}
				if(!recursion_get_size(sh,*ptci,level))
					return false;
				tci.vdata[i].off=size;
				tci.vdata[i].size=ptci->size;
				size+=tci.vdata[i].size*tci.vdata[i].count;
			}
		}
		tci.size=size;
		return true;
	}

	static rbool analyse(tsh& sh,tclass& tci)
	{
		rbuf<tword>& v=tci.vword;
		int start=0;
		int i;
		for(i=0;i<v.count();i++)
		{
			if(i==v.count()-1&&start<i)
			{
				if(!data_add(sh,tci,v.sub(start,i+1)))
				{
					return false;
				}
				break;
			}
			elif(i>0&&v[i-1].pos!=v[i].pos)
			{
				if(v[i].val==rppoptr(c_bbk_l))
				{
					int right=sh.find_symm_bbk(v,i);
					if(right>=v.count())
					{
						sh.error(v.get(start),"miss }");
						return false;
					}
					if(!func_add(sh,tci,v.sub(start,right+1)))
					{
						return false;
					}
					i=right;
					start=right+1;
				}
				else
				{
					if(!data_add(sh,tci,v.sub(start,i)))
					{
						return false;
					}
					start=i;
					v[i-1].pos=v[i].pos;
					i--;
				}
			}
			elif(v[i].val==rppoptr(c_bbk_l))
			{
				int right=sh.find_symm_bbk(v,i);
				if(right>=v.count())
				{
					sh.error(v.get(start),"miss }");
					return false;
				}
				ifn(func_add(sh,tci,v.sub(start,right+1)))
				{
					return false;
				}
				i=right;
				start=right+1;
			}
			elif(v[i].val==rppoptr(c_semi))
			{
				if(!data_add(sh,tci,v.sub(start,i)))
				{
					return false;
				}
				start=i+1;
			}
			elif(v[i].val==rppkey(c_enum))
			{
				int left=i+1;
				if(v.get(left).val!=rppoptr(c_bbk_l))
				{
					sh.error(v.get(i),"miss {");
					return false;
				}
				int right=sh.find_symm_bbk(v,left);
				if(right>=v.count())
				{
					sh.error(v.get(i),"miss }");
					return false;
				}
				ifn(enum_add(sh,tci,v.sub(left+1,right)))
				{
					return false;
				}
				i=right;
				start=right+1;
			}
			elif(v[i].val==rppkey(c_mac))
			{
				int right;
				if(!mac_add(sh,tci,v,i,right))
					return false;
				i=right;
				start=right+1;
			}
		}
		return true;
	}

	static rbool enum_add(tsh& sh,tclass& tci,const rbuf<tword>& v)
	{
		uint enum_val=0;
		for(int j=0;j<v.count();j++)
		{
			if(v[j].val==rppoptr(c_comma)||v[j].val==rppoptr(c_semi))
			{
				continue;
			}
			tmac eitem;
			eitem.name=v[j].val;
			if(v.get(j+1).val==rppoptr(c_equal))
			{
				if(v.get(j+2)==rppoptr(c_sbk_l))
				{
					enum_val=v.get(j+3).val.touint();
					eitem.vstr.push(rstr(enum_val));
					enum_val++;
					j+=4;
				}
				else
				{
					enum_val=v.get(j+2).val.touint();
					eitem.vstr.push(rstr(enum_val));
					enum_val++;
					j+=2;
				}
			}
			else
			{
				eitem.vstr.push(rstr(enum_val));
				enum_val++;
			}
			if(tci.vmac.exist(eitem))
			{
				sh.error(v.get_bottom(),"enum redefined");
				return false;
			}
			tci.vmac.insert(eitem);
		}
		return true;
	}

	static rbool mac_add(tsh& sh,tclass& tci,rbuf<tword>& v,int i,int& right)
	{
		if(v.get(i+1).val=="$")
		{
			ifn(zsuper::add_super_mac(sh,v,i,tci.vmac))
			{
				return false;
			}
			right=i;
			return true;
		}
		tmac mitem;
		int left=i+2;
		mitem.name=v.get(left-1).val;
		if(mitem.name.empty())
		{
			sh.error(v.get(i),"mac miss name");
			return false;
		}
		if(v.get(left).val==rppoptr(c_sbk_l))
		{
			right=sh.find_symm_sbk(v,left);
			if(right>=v.count())
			{
				sh.error(v.get(i),"miss )");
				return false;
			}
			rbuf<rbuf<rstr> > temp=r_split<rbuf<rstr>,rstr>(
				sh.vword_to_vstr(v.sub(left+1,right)),
				rppoptr(c_comma));
			for(int j=0;j<temp.count();j++)
			{
				mitem.param.push(temp[j].get(0));
			}
			right++;
			left=right;
		}
		right=left;
		if(v.get(left).val==rppoptr(c_bbk_l))
		{
			right=sh.find_symm_bbk(v,left);
			if(right>=v.count())
			{
				sh.error(v.get(i),"miss }");
				return false;
			}
			for(int j=left+1;j<right;j++)
			{
				mitem.vstr.push(v[j].val);
			}
		}
		else
		{
			for(;right<v.count()&&v[right].pos==v[i].pos;right++)
				;
			right--;
			if(right>=v.count())
			{
				sh.error(v.get(i),"miss mac");
				return false;
			}
			if(mitem.param.empty())
			{
				for(int j=left;j<=right;j++)
					mitem.vstr.push(v[j].val);
			}
			else
			{
				mitem.vstr.push(rppoptr(c_sbk_l));
				for(int j=left;j<=right;j++)
				{
					if(mitem.param.exist(v[j].val))
					{
						mitem.vstr.push(rppoptr(c_sbk_l));
						mitem.vstr.push(v[j].val);
						mitem.vstr.push(rppoptr(c_sbk_r));
					}
					else
						mitem.vstr.push(v[j].val);
				}
				mitem.vstr.push(rppoptr(c_sbk_r));
			}
		}
		if(tci.vmac.exist(mitem))
		{
			sh.error(v.get_bottom(),"mac redefined");
			return false;
		}
		tci.vmac.insert(mitem);
		return true;
	}

	static rbool a_data_define(tsh& sh,tdata& item,const rbuf<tword>& vword)
	{
		int pos=vword.find(tword(rppoptr(c_equal)));
		if(pos<vword.count())
		{
			for(int i=pos+1;i<vword.count();i++)
				item.param.push(vword[i]);
		}
		else
		{
			pos=vword.find(tword(rppoptr(c_sbk_l)));
			if(pos<vword.count())
				for(int i=pos+1;i<vword.count()-1;i++)
					item.param.push(vword[i]);
		}
		//process func template
		if(vword.get(1).val==rppoptr(c_tbk_l))
		{
			int end=sh.find_symm_tbk(vword,1);
			if(end>=vword.count())
			{
				sh.error(vword.get(0),"miss >");
				return false;
			}
			for(int i=0;i<end;i++)
				item.type+=vword[i].val;
			int pointpos=end+1;
			for(;pointpos<pos;++pointpos)
			{
				if(vword[pointpos].val!=rppoptr(c_addr)&&
					vword[pointpos].val!=rppoptr(c_star))
					break;
				item.type+=vword[pointpos].val;
			}
			item.name=vword.get(pointpos).val;
		}
		else
		{
			item.type=vword.get(0).val;
			if(zfind::is_class_t(sh,item.type))
			{
				item.name=vword.get(1).val;
			}
			else
			{
				item.type="var&";
				item.name=vword.get(0).val;
			}
		}
		if(item.type.empty()||item.name.empty())
		{
			sh.error(vword.get(0),"miss data type or name");
			return false;
		}
		item.count=1;
		if(vword.get(2)==rppoptr(c_mbk_l))
		{
			if(vword.get(4)!=rppoptr(c_mbk_r))
			{
				sh.error(vword.get(0),"data array error");
				return false;
			}
			//保存数组个数
			item.count=vword.get(3).val.toint();
		}
		return true;
	}

	static rbool lambda_a_func_define(tsh& sh,tfunc& item,rbuf<tword>& v)
	{
		if(v.get(1)!=rppoptr(c_sbk_l))
		{
			sh.error(v.get(0),"miss (");
			return false;
		}
		item.name=v.get(0).val;
		int right=sh.find_symm_sbk(v,1);
		if(right>=v.count())
		{
			sh.error(v.get(0),"miss )");
			return false;
		}
		rbuf<tword> param=v.sub(2,right);
		rbuf<rbuf<tword> > list=sh.comma_split(param);
		int i=0;
		//如果第一个参数只有类型，则表明是返回值
		if(!list.empty()&&list[0].count()==1&&
			zfind::is_class_t(sh,list[0].get_bottom().val))
		{
			item.retval.type=list[0].get_bottom().val;
			i++;
		}
		else
		{
			item.retval.type=rppkey(c_void);
		}
		item.retval.name=rppkey(c_s_ret);
		for(;i<list.count();i++)
		{
			if(list[i].empty())
			{
				continue;
			}
			tdata ditem;
			ifn(a_data_define(sh,ditem,list[i]))
			{
				return false;
			}
			item.param.push(ditem);
		}
		return true;
	}

	static rbool a_func_define(tsh& sh,tfunc& item,rbuf<tword>& v,
		rbool lambda=false)
	{
		item.name.clear();
		if(v.get_bottom().val.sub(0,7)=="_LAMBDA")
		{
			return lambda_a_func_define(sh,item,v);
		}
		int start=0;
		if(v.get(start).val==rppkey(c_cfunc))
		{
			item.is_cfunc=true;
			start++;
		}
		if(v.get(start).val==rppkey(c_dynamic))
		{
			item.is_dynamic=true;
			start++;
		}
		if(v.get(start).val==rppkey(c_friend))
		{
			item.is_friend=true;
			start++;
		}
		if(lambda)
		{
			if(zfind::is_class_t(sh,v.get(start).val))
			{
				item.retval.type=v.get(start).val;
				start++;
			}
			else
				item.retval.type=rppkey(c_void);
		}
		else
		{
			if(zfind::is_class_t(sh,v.get(start).val)
				&&v.get(start+1).val!=rppoptr(c_sbk_l)
				&&!v.get(start+1).val.empty())
			{
				item.retval.type=v.get(start).val;
				start++;
			}
			else
				item.retval.type=rppkey(c_void);
		}
		if(item.retval.type.empty())
		{
			sh.error(v.get(0),"miss ret type");
			return false;
		}
		item.retval.name=rppkey(c_s_ret);
		if(v.get(start).val==rppoptr(c_destruct))
		{
			item.name+=v.get(start).val;
			start++;
			item.name+=v.get(start).val;
			start++;
		}
		elif(lambda&&(
			v.get(start)==rppoptr(c_bbk_l)||
			v.get(start)==rppoptr(c_sbk_l)))
		{
			item.name+="_SELF";
		}
		elif(sh.m_optr.is_optr(v.get(start).val))
		{
			item.name=v.get(start).val;
			if(v.get(start).val==rppoptr(c_mbk_l))
			{
				start++;
				item.name+=v.get(start).val;
			}
			start++;
		}
		else
		{
			item.name+=v.get(start).val;
			start++;
		}
		if(item.name.empty())
		{
			sh.error(v.get(0),"miss func name");
			return false;
		}
		rbuf<rbuf<tword> > list;
		int right;
		if(v.get(start).val==rppoptr(c_sbk_l))
		{
			right=sh.find_symm_sbk(v,start);
			if(right>=v.count())
			{
				sh.error(v.get(0),"miss )");
				return false;
			}
			list=sh.comma_split(v.sub(start+1,right));
		}
		else
		{
			list=sh.comma_split(v.sub(start));
		}
		for(int i=0;i<list.count();i++)
		{
			if(list[i].empty())
			{
				continue;
			}
			tdata ditem;
			ifn(a_data_define(sh,ditem,list[i]))
			{
				return false;
			}
			item.param.push(ditem);
		}
		return true;
	}

	static rbool data_add(tsh& sh,tclass& tci,const rbuf<tword>& v)
	{
		if(v.empty())
		{
			return true;
		}
		tdata item;
		if(!a_data_define(sh,item,v))
		{
			return false;
		}
		if(tci.vdata.exist(item))
		{
			v.print();
			sh.error(v.get_bottom(),"data member redefined");
			return false;
		}
		tci.vdata.push(item);
		return true;
	}
	
	//tl表示是否检查模板
	static rbool func_add(tsh& sh,tclass& tci,const rbuf<tword>& v,rbool tl=true)
	{
		tfunc item;
		int left=v.find(tword(rppoptr(c_bbk_l)));
		if(left>=v.count())
		{
			sh.error(v.get_bottom(),"miss {");
			return false;
		}
		int right=v.count()-1;
		item.first_pos=v[left].pos;
		item.last_pos=v.get(right).pos;
		item.ptci=&tci;//反射
		rbuf<tword> vhead=v.sub(0,left);
		if(tl)
		{
			func_tl_part(sh,item,vhead);
		}
		if(item.vtl.empty())
		{
			if(!a_func_define(sh,item,vhead))
			{
				return false;
			}
			if(item.name.sub(0,7)!="_LAMBDA")
			{
				add_this_func(sh,tci,item);
			}
			obtain_size_func(sh,item);
			item.name_dec=item.get_dec();
			if(tci.vfunc.exist(item))
			{
				if(tl)
				{
					sh.error(v.get_bottom(),"func redefined");
					return false;
				}
				else
					return true;
			}
			item.vword=v.sub(left+1,right);
			tci.vfunc.insert(item);
			if(!default_param_proc(sh,*tci.vfunc.find(item)))
			{
				return false;
			}
		}
		else
		{
			item.name_dec=item.get_dec();
			if(tci.vfunctl.exist(item))
			{
				sh.error(v.get_bottom(),"functl redefined");
				return false;
			}
			item.vword=v;
			tci.vfunctl.insert(item);
		}
		return true;
	}

	static void obtain_size_func(tsh& sh,tclass& tci)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			obtain_size_func(sh,*p);
		}
	}

	static void obtain_size_func(tsh& sh,tfunc& tfi)
	{
		tfi.retval.size=zfind::get_type_size(sh,tfi.retval.type);
		for(int i=0;i<tfi.param.count();i++)
		{
			tfi.param[i].size=zfind::get_type_size(sh,tfi.param[i].type);
		}
	}

	static void func_tl_part(tsh& sh,tfunc& item,rbuf<tword>& v)
	{
		int left=v.count();
		if(v.get_top()!=rppoptr(c_tbk_r))
		{
			if(v.get_top()!=rppoptr(c_sbk_r))
			{
				return;
			}
			left=sh.find_symm_word_rev(v,
				rppoptr(c_sbk_l),rppoptr(c_sbk_r),
				0,v.count());
			if(left>=v.count())
			{
				return;
			}
		}
		if(v.get(left-1)!=rppoptr(c_tbk_r))
			return;
		rbuf<tword> temp=v.sub(0,left);
		left=sh.find_symm_word_rev(temp,
			rppoptr(c_tbk_l),rppoptr(c_tbk_r),
			0,temp.count());
		if(left>=temp.count())
			return;
		item.name=temp.get(left-1).val;
		if(item.name.empty())
			return;
		rbuf<rbuf<rstr> > vdst;
		rbuf<rstr> vsrc=sh.vword_to_vstr(
			temp.sub(left+1,temp.count()-1));
		vdst=r_split(vsrc,rppoptr(c_comma));
		if(vdst.empty())
			return;
		for(int i=0;i<vdst.count();i++)
		{
			ttl tli;
			tli.name=vdst[i].get_bottom();
			item.vtl.push(tli);
		}
	}

	static void add_this_func(tsh& sh,tclass& tci,tfunc& tfi)
	{
		if(tci.is_friend)
			tfi.is_friend=true;
		if(!tfi.is_friend)
		{
			tdata tdi;
			tdi.name=rppkey(c_this);
			tdi.type=tci.name+rppoptr(c_addr);
			tfi.param.push_front(tdi);
		}
	}

	static rbool default_param_proc(tsh& sh,tfunc& tfi)
	{
		int i;
		for(i=0;i<tfi.param.count();i++)
			if(!tfi.param[i].param.empty())
				break;
		if(i>=tfi.param.count())
			return true;
		for(int j=i;j<tfi.param.count();j++)
			if(tfi.param[j].param.empty())
			{
				sh.error(tfi.vword.get_bottom(),"default param error");
				return false;
			}
		for(;i<tfi.param.count();i++)
		{
			tfunc ftemp=tfi;
			ftemp.param.erase(i,ftemp.param.count());
			rbuf<tword> vtemp;
			tword twi;
			twi.pos_src=tfi.first_pos;
			twi.pos=tfi.first_pos;
			for(int j=i;j<tfi.param.count();j++)
			{
				twi.val=tfi.param[j].type;
				vtemp.push(twi);
				twi.val=tfi.param[j].name;
				vtemp.push(twi);
				twi.val=rppoptr(c_sbk_l);
				vtemp.push(twi);
				for(int k=0;k<tfi.param[j].param.count();k++)
				{
					twi.val=tfi.param[j].param[k].val;
					vtemp.push(twi);
				}
				twi.val=rppoptr(c_sbk_r);
				vtemp.push(twi);
				twi.val=rppoptr(c_semi);
				vtemp.push(twi);
			}
			vtemp+=r_move(ftemp.vword);
			ftemp.vword=r_move(vtemp);
			for(int j=0;j<ftemp.param.count();j++)
				ftemp.param[j].param.free();
			ftemp.name_dec=ftemp.get_dec();
			if(tfi.ptci->vfunc.exist(ftemp))
			{
				sh.error(tfi.vword.get_bottom(),"func redefined");
				return false;
			}
			tfi.ptci->vfunc.insert(ftemp);
		}
		for(i=0;i<tfi.param.count();i++)
			tfi.param[i].param.free();
		return true;
	}
};

#endif
