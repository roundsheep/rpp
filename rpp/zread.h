#ifndef zread_h__
#define zread_h__

#include "tsh.h"
#include "zsuper.h"
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
		if(sh.m_is_pre_mode)
		{
			return true;
		}
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
		if(!read_match(sh,get_inf_dir(sh)+rstrw("rinf/match.txt")))
		{
			return false;
		}
		return true;
	}

	static rbuf<tasm> get_op_vasm(tsh& sh,const rbuf<rstr>& v)
	{
		rbuf<tasm> result;
		tasm item;
		rbuf<tword> vword;
		rbuf<rstr>& vstr=item.vstr;
		for(int i=0;i<v.count();i++)
		{
			ifn(word_analyse(sh,v[i],vword,null))
			{
				result.clear();
				return result;
			}
			vstr=sh.vword_to_vstr(vword);
			for(int j=0;j<vstr.count();j++)
			{
				if(j+1<vstr.count()&&vstr[j]=="@")
				{
					vstr[j]+=vstr[j+1];
					vstr.erase(j+1);
				}
				elif(vstr[j].get_bottom()=='\"')
				{
					vstr[j]=zsuper::del_quote(vstr[j]);
				}
			}
			result.push(item);
		}
		return r_move(result);
	}

	static rbool read_match(tsh& sh,const rstrw& name)
	{
		if(!rppconf(c_op_match))
		{
			return true;
		}
		rstr data=get_file_data(sh,name);
		if(data.empty())
		{
			sh.error("can't read match file "+name.torstr());
			return false;
		}
		rbuf<rstr> vstr=r_split_e(data,rstr("\r\n"));
		top_node item;
		int start=0;
		for(int i=0;i<vstr.count();i++)
		{
			if(vstr[i].empty())
			{
				if(item.src.empty())
				{
					item.src=get_op_vasm(sh,vstr.sub(start,i));
				}
				elif(item.dst.empty())
				{
					item.dst=get_op_vasm(sh,vstr.sub(start,i));
					sh.m_match.push(item);
					item.clear();
				}
				start=i+1;
			}
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
				rbuf<tword> vword;
				ifn(word_analyse(sh,vstr[i],vword,null))
				{
					sh.error("can't read conf file "+name.torstr());
					return false;
				}
				int dst;
				if(!const_eval(sh,sh.vword_to_vstr(vword),dst))
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

	static rbool word_analyse(tsh& sh,rstr& src,rbuf<tword>& result,tfile* pfile)
	{
		result.clear();
		tword cur_word;
		cur_word.pos.file=pfile;
		cur_word.pos.line=1;
		int len;
		uchar* start;
		uchar* p;
		rstr stemp;
		stemp.set_size(sh.m_optr.m_optr_max);
		for(p=src.cstr();*p;++p)
		{
			start=p;
			if(*p=='/')
			{
				//为了效率这里使用了过多的缩进
				if(*(p+1)=='*')
				{
					p+=2;
					int count=1;
					for(;*p&&*(p+1);p++)
					{
						if('\n'==*p)
							++cur_word.pos.line;
						if(*p&&*(p+1)&&*p=='/'&&*(p+1)=='*')
						{
							++count;
						}
						if(*p&&*(p+1)&&*p=='*'&&*(p+1)=='/')
						{
							--count;
						}
						if(0==count)
						{
							break;
						}
					}
					if(count)
					{
						sh.error(rstr(cur_word.pos.line)+rstr("miss */"));
						return false;
					}
					p++;
					continue;
				}
				elif(*(p+1)=='/')
				{
					p+=2;
					for(;*p&&*p!='\n';p++)
					{
						;
					}
					if(*p==0)
						return true;
					++cur_word.pos.line;
					continue;
				}
			}
			len=get_optr_s_len(sh,p,src.end()-p,stemp);
			if(len)
			{
				cur_word.val=rstr(p,p+len);
				push_word(result,cur_word);
				p+=len;
				p--;
			}
			elif(rstr::is_alpha(*p)||'_'==*p||
				rcode::is_utf8_3(*p)&&*(p+1)&&*(p+2)||
				rcode::is_utf8_2(*p)&&*(p+1))
			{
				for(;*p;++p)
				{
					if(rstr::is_number(*p)||rstr::is_alpha(*p)||'_'==*p)
						continue;
					elif(rcode::is_utf8_3(*p)&&*(p+1)&&*(p+2))
					{
						//todo 应该只能以汉字开头不能以特殊符号开头
						p+=2;
						continue;
					}
					elif(rcode::is_utf8_2(*p)&&*(p+1))
					{
						p++;
						continue;
					}
					else
						break;
				}
				cur_word.val=rstr(start,p);
				push_word(result,cur_word);
				p--;
			}
			elif(rstr::is_number(*p))
			{
				for(++p;*p&&(rstr::is_number(*p)||'_'==*p||
					rstr::is_alpha(*p));++p)
					;
				cur_word.val=rstr(start,p);
				push_word(result,cur_word);
				p--;
			}
			elif('"'==*p)
			{
				int line=cur_word.pos.line;
				for(++p;*p;++p)
				{
					if('\n'==*p)
						++cur_word.pos.line;
					if(*p=='\\')
					{
						if(*(p+1)=='x'&&*(p+2)&&*(p+3))
						{
							p+=3;
						}
						elif(*(p+1))
						{
							p++;
						}
						continue;
					}
					if(*p=='\"')
					{
						break;
					}
				}
				cur_word.pos.line=line;
				if(*p)
				{
					cur_word.val=rstr(start,p+1);
					push_word(result,cur_word);
				}
				else
				{
					sh.error(rstr(cur_word.pos.line)+rstr("miss \""));
					return false;
				}
			}
			elif('\''==*p)
			{
				int line=cur_word.pos.line;
				for(++p;*p;++p)
				{
					if('\n'==*p)
						++cur_word.pos.line;
					if(*p=='\\')
					{
						if(*(p+1)=='x'&&*(p+2)&&*(p+3))
						{
							p+=3;
						}
						elif(*(p+1))
						{
							p++;
						}
						continue;
					}
					if(*p=='\'')
					{
						break;
					}
				}
				cur_word.pos.line=line;
				if(*p)
				{
					cur_word.val=rstr(start,p+1);
					if(cur_word.val.count()<2)
					{
						sh.error(rstr(cur_word.pos.line)+rstr("miss '"));
						return false;
					}
					cur_word.val[0]='"';
					cur_word.val[cur_word.val.count()-1]='"';
					add_rstr(sh,result,cur_word);
				}
				else
				{
					sh.error(rstr(cur_word.pos.line)+rstr("miss '"));
					return false;
				}
			}
			elif('\n'==*p)
			{
				++cur_word.pos.line;
			}
			elif('\\'==*p&&'\\'==*(p+1))
			{
				p+=2;
				for(;*p&&*p!='\n'&&*p!=0xd;p++)
				{
					;
				}
				cur_word.val=zsuper::add_quote(rstr(start+2,p));
				add_rstr(sh,result,cur_word);
				if(*p==0)
					return true;
				if(*p=='\n')
					++cur_word.pos.line;
			}
			elif('`'==*p)
			{
				if(*(p+1)==0)
				{
					sh.error(rstr(cur_word.pos.line)+rstr("miss `"));
					return false;
				}
				p++;
				cur_word.val=rstr((uint)*(p));
				push_word(result,cur_word);
			}
		}
		return true;
	}

	static void push_word(rbuf<tword>& result,tword word)
	{
		word.pos_src=word.pos;
		result.push_move(word);
	}

	static int get_optr_s_len(tsh& sh,const uchar* s,int len,rstr& stemp)
	{
		for(int i=sh.m_optr.m_optr_max;i>0;i--)
		{
			if(len<i)
				continue;
			memcpy(stemp.begin(),s,i);
			stemp.m_buf.m_count=i;
			int pos=r_find_pos_b(sh.m_optr.m_optr_s,stemp);//这里是瓶颈
			if(pos<sh.m_optr.m_optr_s.count())
				return sh.m_optr.m_optr_s[pos].count();
		}
		return 0;
	}

	static void add_rstr(tsh& sh,rbuf<tword>& result,const tword& word)
	{
		if(result.get_top()==rppkey(c_import)||
			result.get_top()==rppkey(c_include))
		{
			push_word(result,word);
			return;
		}
		tword temp;
		temp.pos=word.pos;
		temp.val=rppkey(c_rstr);
		push_word(result,temp);
		temp.val=rppoptr(c_sbk_l);
		push_word(result,temp);
		push_word(result,word);
		temp.val=rppoptr(c_sbk_r);
		push_word(result,temp);
	}

	static rbool is_const_str(tsh& sh,const rstr& s)
	{
		return s.is_number()||s==rppoptr(c_sbk_l)||s==rppoptr(c_sbk_r)||
			s==rppoptr(c_plus)||s==rppoptr(c_minus)||s==rppoptr(c_star)||
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
		return r_move(ret);
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