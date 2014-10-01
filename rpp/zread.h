#ifndef zread_h__
#define zread_h__

#include "tsh.h"
#include "../rlib/rdir.h"

//读取基本配置信息和主文件  
struct zread
{
	static rbool process(tsh& sh)
	{
		if(!read_inf(sh))
			return false;
		if(sh.m_is_pack_read)
		{
			for(int i=0;i<sh.m_db.count()-4*2;i+=2)
			{
				ifn(read_file_pack(sh,sh.m_db[i]))
				{
					sh.error("can't read file");
					return false;
				}
			}
		}
		else
		{
			auto_import(sh);
			if(!read_file(sh,sh.m_main_file))
			{
				sh.error("can't read main file "+sh.m_main_file.torstr());
				return false;
			}
		}
		return true;
	}

	static rbool auto_import(tsh& sh)
	{
		ifn(rppconf(c_auto_import))
			return true;
		if(!read_file(sh,rdir::get_exe_dir()+rstrw("rsrc/basic.h")))
		{
			return false;
		}
		return true;
	}

	static rstrw get_inf_dir(tsh& sh)
	{
		if(sh.m_is_pack_read||sh.m_is_pack_mode)
		{
			return "";
		}
		return rdir::get_exe_dir();
	}

	static rbool read_inf(tsh& sh)
	{
		if(!read_optr(sh,get_inf_dir(sh)+rstrw("rinf/optr.txt")))
		{
			return false;
		}
		sh.m_optr.sort_optr();
		if(!read_key(sh,get_inf_dir(sh)+rstrw("rinf/key.txt")))
		{
			return false;
		}
		sh.m_key.sort_asm();
		if(!read_conf(sh,get_inf_dir(sh)+rstrw("rinf/conf.txt")))
		{
			return false;
		}
		return true;
	}

	static rbool read_conf(tsh& sh,const rstrw& name)
	{
		rstr data=get_file_data(sh,name);
		if(data.empty())
		{
			sh.error("can't read conf file "+name.torstr());
			return false;
		}
		rbuf<rstr> vstr=r_split(data,rstr("\r\n"));
		for(int i=0;i<vstr.count();i++)
		{
			if(i%2==0)
				continue;
			uint udst;
			if(i/2==tconf::c_stack_size)
			{
				rbuf<rstr> vstr2;
				word_analyse_a_q(vstr[i],vstr2);
				int dst;
				if(!const_eval(sh,vstr2,dst))
					return false;
				udst=(uint)dst;
			}
			else
				udst=vstr[i].touint();
			sh.m_conf.m_conf.push(udst);
		}
		if(sh.m_conf.m_conf.count()<=tconf::c_stack_size)
			return false;
		if(rppconf(c_stack_size)<1024*1024)
			rppconf(c_stack_size)=1024*1024;
		return true;
	}

	static rbool read_optr(tsh& sh,const rstrw& name)
	{
		rstr data=get_file_data(sh,name);
		if(data.count()<2)
		{
			sh.error("can't read optr file "+name.torstr());
			return false;
		}
		ifn(rcode::is_utf16_txt(data))
		{
			sh.error("can't read optr file "+name.torstr());
			return false;
		}
		rbuf<rstr> vstr;
		vstr=r_split(data.sub(2),rstr((void*)"\r\x00\n\x00",4));
		for(int i=0;i<vstr.count();i++)
		{
			if(vstr[i].empty())
			{
				sh.error("can't read optr file "+name.torstr());
				return false;
			}
			rstrw temp=vstr[i].cstrw_t();
			if(temp.is_number())
			{
				sh.m_optr.m_dic[sh.m_optr.m_optr.get_top()]=temp.toint();
			}
			else
				sh.m_optr.m_optr.push(temp.torstr());
		}
		return sh.m_optr.m_optr.count()==sh.m_optr.m_dic.count()&&
			sh.m_optr.m_optr.count()>toptr::c_exp_part;
	}

	static rbool read_key(tsh& sh,const rstrw& name)
	{
		rstr data=get_file_data(sh,name);
		if(data.empty())
		{
			sh.error("can't read key file "+name.torstr());
			return false;
		}
		rbuf<rstr> vstr=r_split(data,rstr("\r\n"));
		for(int i=0;i<vstr.count();i++)
		{
			if(vstr[i].empty())
			{
				sh.error("can't read key file "+name.torstr());
				return false;
			}
			sh.m_key.m_key.push(vstr[i]);
		}
		return sh.m_key.m_key.count()>tkey::c_pmain;
	}

	static rbool exist(tsh& sh,const rstrw& name)
	{
		return sh.m_file.exist(tfile(name));
	}

	static rbool read_file_pack(tsh& sh,const rstrw& name)
	{
		tfile item;
		item.name=name;
		item.cont=get_file_data(sh,name);//写入时已经是utf8，不用转换
		if(item.cont.empty())
		{
			return false;
		}
		sh.m_file.insert(item);
		return true;
	}

	static rbool read_file(tsh& sh,const rstrw& name)
	{
		if(name.empty())
			return false;
		ifn(file_exist(name))
		{
			return false;
		}
		tfile item;
		item.name=name;
		if(sh.m_file.exist(item))
			return true;
		if(rdir::get_suffix(name)==rstrw("rp"))
		{
			return false;
		}
#ifndef __COCOS2D_H__
		if((int)rfile::get_size8(name)>200*1024*1024)
		{
			return false;
		}
#endif
		item.cont=get_file_data(sh,name);
		if(item.cont.empty())
		{
			return false;
		}
		item.cont=rcode::to_utf8_txt(item.cont).sub(3);
		sh.m_file.insert(item);
		return true;
	}

	static rbool const_eval(tsh& sh,rbuf<rstr> src,int& dst,int level=0)
	{
		if(level++>c_rpp_deep)
		{
			sh.error("const eval level overflow");
			return false;
		}
		rbuf<rstr> soptr;
		rbuf<int> sopnd;
		soptr+=rppoptr(c_exp_part);
		src+=rppoptr(c_exp_part);
		for(int i=0;i<src.count();i++)
		{
			if(src[i]==rppoptr(c_exp_part)&&
				soptr.get_top()==rppoptr(c_exp_part))
				break;
			if(src[i].is_number())
			{
				int outopnd=src[i].toint();
				sopnd.push(outopnd);
			}
			elif(src[i]==rppoptr(c_sbk_l))
			{
				int right=sh.find_symm_sbk(src,i);
				if(right>=src.count())
				{
					sh.error("const eval miss )");
					return false;
				}
				int outopnd;
				if(!const_eval(sh,src.sub(i+1,right),outopnd,level))
					return false;
				sopnd.push(outopnd);
				i=right;
			}
			elif(sh.m_optr.is_optr(src[i]))
			{
				if(soptr.empty())
				{
					sh.error("const eval miss soptr");
					return false;
				}
				rstr cur=src[i];
				if(!sh.m_optr.precede(soptr.top(),cur))
				{
					soptr.push(cur);
					continue;
				}
				rstr theta=soptr.pop();
				if(sopnd.empty())
				{
					sh.error("const eval miss sopnd");
					return false;
				}
				int second=sopnd.pop();
				if(sopnd.empty())
				{
					if(theta==rppoptr(c_plus))
					{
					}
					elif(theta==rppoptr(c_minus))
					{
						second=-second;
					}
					else
					{
						sh.error("const eval miss sopnd");
						return false;
					}
					sopnd.push(second);
					i--;
					continue;
				}
				int first=sopnd.pop();
				int outopnd;
				if(!calc(sh,first,second,theta,outopnd))
				{
					sh.error("const eval calc error");
					return false;
				}
				sopnd.push(outopnd);
				i--;
			}
		}
		if(sopnd.count()!=1)
		{
			return false;
		}
		dst=sopnd[0];
		return true;
	}

	static rbool calc(tsh& sh,int first,int second,rstr& theta,int& outopnd)
	{
		if(theta==rppoptr(c_plus))
		{
			outopnd=first+second;
		}
		elif(theta==rppoptr(c_minus))
		{
			outopnd=first-second;
		}
		elif(theta==rppoptr(c_star))
		{
			outopnd=first*second;
		}
		elif(theta==rppoptr(c_divide))
		{
			if(second==0)
				return false;
			outopnd=first/second;
		}
		else
			return false;
		return true;
	}

	//快速词法分析
	static rbool word_analyse_a_q(rstr& src,rbuf<rstr>& dst)
	{
		dst.clear();
		uchar* start;
		uchar* p;
		for(p=src.cstr();*p;++p)
		{
			start=p;
			if(rstr::is_number(*p))
			{
				for(++p;*p&&rstr::is_number(*p);++p)
					;
				dst.push(rstr(start,p));
				p--;
			}
			elif(rstr::is_alpha(*p)||'_'==*p)
			{
				for(++p;*p&&(rstr::is_number(*p)||rstr::is_alpha(*p)||'_'==*p);++p)
					;
				dst.push(rstr(start,p));
				p--;
			}
			elif(is_asm_optr(*p))
			{
				dst.push(rstr(p,p+1));
			}
		}
		return true;
	}

	static rbool is_asm_optr(char ch)
	{
		return ch=='['||ch==']'||ch=='+'||ch=='-'||ch==','||ch==':'||
			ch=='('||ch==')'||ch=='*'||ch=='/';
	}

	static rbool is_const_str(tsh& sh,const rstr& s)
	{
		return s.is_number()||
			s==rppoptr(c_sbk_l)||
			s==rppoptr(c_sbk_r)||
			s==rppoptr(c_plus)||
			s==rppoptr(c_minus)||
			s==rppoptr(c_star)||
			s==rppoptr(c_divide);
	}

	static rbool is_const_str(tsh& sh,rbuf<tword>& v,int begin,int end)
	{
		for(int i=begin;i<end;i++)
			if(!is_const_str(sh,v[i].val))
				return false;
		return true;
	}

	static rbool file_exist(const rstrw& name)
	{
#ifndef __COCOS2D_H__
		return rfile::exist(name);
#else
		return !get_file_data(name).empty();
#endif
	}

#ifdef __COCOS2D_H__
	//读到空文件也算失败
	static rstr get_file_data(const rstrw& name)
	{
		int size;
		uchar* pbuf=CCFileUtils::getInstance()->getFileData(
			name.torstr().cstr_t(),"rb",(ssize_t*)&size);
		if(pbuf==null||size<=0)
		{
			return "";
		}
		rstr ret=rstr(pbuf,size);
		delete[] pbuf;
		return ret;
	}
#else
	static rstr get_file_data(tsh& sh,const rstrw& name)
	{
		if(sh.m_is_pack_read)
		{
			for(int i=0;i<sh.m_db.count();i+=2)
			{
				if(sh.m_db[i]==name)
				{
					return sh.m_db[i+1];
				}
			}
			return "";
		}
		else
		{
			return rfile::read_all_n(name);
		}
	}
#endif
};

#endif