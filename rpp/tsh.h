#ifndef tsh_h__
#define tsh_h__

#include "tconf.h"
#include "../rlib/rstrw.h"
#include "rpp_def.h"
#include "tstruct.h"
#include "../rlib/rset.h"
#include "../rlib/rmutex.h"
#include "../rlib/rdb.h"
#include "../rlib/rdir.h"
#include "talloc.h"

//共享类
struct tsh
{
	toptr m_optr;
	tkey m_key;
	tconf m_conf;

	rset<tclass> m_class;
	rset<tclass> m_classtl;//模板
	
	rset<tfile> m_file;
	rset<tmac> m_vdefine;

	tclass* m_main;
	rstr m_main_data;//全局变量存储区
	rstrw m_main_file;

	rmutex m_mutex;
	int m_mode;

	int m_argc;
	char** m_argv;

	rbuf<top_node> m_match;

	rdic<void*> m_jit_f;//jit静态函数地址表
	void* m_hins;
	int m_ret;

	rbool m_is_pack_read;
	rbool m_is_pack_mode;
	rbool m_is_pre_mode;
	rdbint m_db;

	enum
	{
		c_jit,
		c_rvm,
		c_win,
		c_grub
	};

	tsh()
	{
		m_main=null;
		m_mode=c_rvm;
		m_is_pack_read=false;
		m_is_pack_mode=false;
		m_is_pre_mode=false;
	}

	void print_vclass()
	{
		for(tclass* p=m_class.begin();p!=m_class.end();p=m_class.next(p))
		{
			p->name.printl();
		}
	}

	static void print_vword(rbuf<tword>& v)
	{
		rf::printl();
		for(int i=0;i<v.count();i++)
		{
			(v[i].val+" ").print();
		}
		rf::printl();
	}

	static void print_func_sent(tfunc& tfi)
	{
		rf::printl();
		for(int i=0;i<tfi.vsent.count();i++)
		{
			rf::print(tfi.vsent[i].pos.line);
			tfi.vsent[i].print();
			rf::printl();
		}
		rf::printl();
	}

	static void print_func_asm(tfunc& tfi)
	{
		for(int i=0;i<tfi.vasm.count();i++)
		{
			rf::print(rstr(tfi.vasm[i].pos.line)+" ");
			tfi.vasm[i].vstr.print();
			rf::printl();
		}
		rf::printl();
	}

	static void print_func_dec(tclass& tci)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			get_func_declare(tci,*p).printl();
		}
	}

	static rstr get_func_declare_name(tclass& tci,tfunc& tfi)
	{
		rstr s;
		s+=tci.name+"."+tfi.name+"(";
		for(int j=0;j<tfi.param.count();j++)
		{
			if(j!=0)
			{
				s+=",";
			}
			s+=tfi.param[j].type+" "+
				tfi.param[j].name;
		}
		s+=")";
		return r_move(s);
	}

	static rstr get_func_declare(tclass& tci,tfunc& tfi)
	{
		rstr s;
		s+=tci.name+"."+tfi.name+"(";
		for(int j=0;j<tfi.param.count();j++)
		{
			if(j!=0)
			{
				s+=",";
			}
			s+=tfi.param[j].type;
		}
		s+=")";
		return r_move(s);
	}

	static rbuf<rstr> get_func_declare_lisp(tsh& sh,tclass& tci,tfunc& tfi)
	{
		return get_func_declare_call(sh,tci,tfi).sub(1);
	}

	static rbuf<rstr> get_func_declare_call(tsh& sh,tclass& tci,tfunc& tfi)
	{
		rbuf<rstr> ret;
		ret+=rppkey(c_call);
		ret+=rppoptr(c_mbk_l);
		ret+=rppoptr(c_addr);
		ret+=tci.name;
		ret+=tfi.name_dec;
		ret+=rppoptr(c_mbk_r);
		return r_move(ret);
	}

	static void clear_word_val(rbuf<tword>& v,int begin,int end)
	{
		if(end>v.count())
			end=v.count();
		if(begin<0)
		{
			begin=0;
		}
		for(int i=begin;i<end;++i)
			v[i].val.clear();
	}

	static rstrw get_main_name(tsh& sh)
	{
		rstrw name=sh.m_main_file;
		name=rdir::get_name(name);
		name=rdir::get_prev_dir(sh.m_main_file)+name.sub(
			0,name.count()-rdir::get_suffix(name).count()-1);
		return r_move(name);
	}

	static rstr get_ctype(tsh& sh,const tword& word)
	{
		if(word.is_cint())
			return rppkey(c_int);
		elif(word.is_cuint())
			return rppkey(c_uint);
		elif(word.is_cdouble())
			return rppkey(c_double);
		elif(word.is_cpoint())
			return rstr("rp<void>");
		elif(word.is_cstr())
			return rstr("rp<char>");
		else
			return rstr();
	}
	
	//获取类型名称，忽略引用
	static rstr get_tname(const rstr& s)
	{
		if(is_quote(s))
			return s.sub(0,s.count()-1);
		else
			return s;
	}

	static rstr get_ttype(rstr s)
	{
		if(is_point_quote(s))
		{
			return s;
		}
		return s+"&";
	}

	rbool is_optr_func(tfunc& tfi)
	{
		if(m_optr.is_optr(tfi.name))
		{
			return true;
		}
		if(tfi.name=="[]")
		{
			return true;
		}
		return false;
	}

	rbool is_basic_type(const rstr& s)
	{
		return s==m_key[tkey::c_rd1]||s==m_key[tkey::c_rd2]||
			s==m_key[tkey::c_rd4]||s==m_key[tkey::c_rd8]||
			s==m_key[tkey::c_rdp]||s==m_key[tkey::c_void];
	}

	static rbool is_point_quote(const rstr& s)
	{
		return is_quote(s)||is_point(s);
	}

	static rbool is_quote(const rstr& s)
	{
		return s.get_top()=='&';
	}

	static rbool is_point(const rstr& s)
	{
		if(s.get_top()=='*')
			return true;
		if(s.count()>=5&&s[0]=='r'&&s[1]=='p'&&s[2]=='<'&&s.get_top()=='>')
			return true;
		return false;
	}

	void split_param(rbuf<tsent>& vsent,
		const rbuf<tword>& v,const tsent& src)
	{
		vsent.clear();
		rbuf<rbuf<tword> > temp=comma_split(v);
		for(int i=0;i<temp.count();i++)
		{
			tsent sent;
			sent.pos=src.pos;
			sent.vword=r_move(temp[i]);
			vsent.push_move(sent);
		}
	}

	void split_param(rbuf<tsent>& vsent,const rbuf<tword>& v)
	{
		vsent.clear();
		rbuf<rbuf<tword> > temp=comma_split(v);
		for(int i=0;i<temp.count();i++)
		{
			tsent sent;
			sent.vword=r_move(temp[i]);
			vsent.push_move(sent);
		}
	}

	//这里的重复代码是为了提高效率
	rbuf<rbuf<tword> > comma_split(rbuf<tword> v)
	{
		rbuf<rbuf<tword> > result;
		v.push(tword(m_optr[toptr::c_comma]));
		int count1=0;
		int count2=0;
		int count3=0;
		int start=0;
		for(int i=0;i<v.count();i++)
		{
			//大括号不可能出现在参数中
			if(v[i].val==m_optr[toptr::c_sbk_l])
				count1++;
			elif(v[i].val==m_optr[toptr::c_sbk_r])
				count1--;
			elif(v[i].val==m_optr[toptr::c_mbk_l])
				count2++;
			elif(v[i].val==m_optr[toptr::c_mbk_r])
				count2--;
			elif(count1==0&&count2==0&&count3==0&&
				v[i].val==m_optr[toptr::c_comma])
			{
				rbuf<tword> temp;
				for(int j=start;j<i;j++)
					temp.push_move(v[j]);
				if(!temp.empty())
					result.push_move(temp);
				start=i+1;
			}
		}
		return r_move(result);
	}

	rbuf<rbuf<rstr> > comma_split(rbuf<rstr> v)
	{
		rbuf<rbuf<rstr> > result;
		v.push(m_optr[toptr::c_comma]);
		int count1=0;
		int count2=0;
		int count3=0;
		int start=0;
		for(int i=0;i<v.count();i++)
		{
			if(v[i]==m_optr[toptr::c_sbk_l])
				count1++;
			elif(v[i]==m_optr[toptr::c_sbk_r])
				count1--;
			elif(v[i]==m_optr[toptr::c_mbk_l])
				count2++;
			elif(v[i]==m_optr[toptr::c_mbk_r])
				count2--;
			elif(count1==0&&count2==0&&count3==0&&
				v[i]==m_optr[toptr::c_comma])
			{
				rbuf<rstr> temp;
				for(int j=start;j<i;j++)
					temp.push_move(v[j]);
				if(!temp.empty())
					result.push_move(temp);
				start=i+1;
			}
		}
		return r_move(result);
	}
	
	//允许空元素
	rbuf<rbuf<tword> > comma_split_e(rbuf<tword> v)
	{
		rbuf<rbuf<tword> > result;
		v.push(tword(m_optr[toptr::c_comma]));
		int count1=0;
		int count2=0;
		int count3=0;
		int start=0;
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val==m_optr[toptr::c_sbk_l])
				count1++;
			elif(v[i].val==m_optr[toptr::c_sbk_r])
				count1--;
			elif(v[i].val==m_optr[toptr::c_mbk_l])
				count2++;
			elif(v[i].val==m_optr[toptr::c_mbk_r])
				count2--;
			elif(count1==0&&count2==0&&count3==0&&
				v[i].val==m_optr[toptr::c_comma])
			{
				rbuf<tword> temp;
				for(int j=start;j<i;j++)
					temp.push_move(v[j]);
				result.push_move(temp);
				start=i+1;
			}
		}
		return r_move(result);
	}

	//带大括号的split
	rbuf<rbuf<rstr> > comma_split_b(rbuf<rstr> v)
	{
		rbuf<rbuf<rstr> > result;
		v.push(m_optr[toptr::c_comma]);
		int count1=0;
		int count2=0;
		int count3=0;
		int start=0;
		for(int i=0;i<v.count();i++)
		{
			if(v[i]==m_optr[toptr::c_bbk_l])
				count3++;
			elif(v[i]==m_optr[toptr::c_bbk_r])
				count3--;
			elif(count1==0&&count2==0&&count3==0&&
				v[i]==m_optr[toptr::c_comma])
			{
				rbuf<rstr> temp;
				for(int j=start;j<i;j++)
					temp.push_move(v[j]);
				if(!temp.empty())
					result.push_move(temp);
				start=i+1;
			}
		}
		return r_move(result);
	}
	
	//带尖括号的split
	rbuf<tsent> comma_split_t(rbuf<tword> v)
	{
		rbuf<tsent> result;
		v.push(tword(m_optr[toptr::c_comma]));
		int count1=0;
		int count2=0;
		int count3=0;
		int start=0;
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val==m_optr[toptr::c_sbk_l])
				count1++;
			elif(v[i].val==m_optr[toptr::c_sbk_r])
				count1--;
			elif(v[i].val==m_optr[toptr::c_mbk_l])
				count2++;
			elif(v[i].val==m_optr[toptr::c_mbk_r])
				count2--;
			elif(v[i].val==m_optr[toptr::c_tbk_l])
				count3++;
			elif(v[i].val==m_optr[toptr::c_tbk_r])
				count3--;
			elif(count1==0&&count2==0&&count3==0&&
				v[i].val==m_optr[toptr::c_comma])
			{
				tsent temp;
				for(int j=start;j<i;j++)
					temp.vword.push_move(v[j]);
				if(!temp.vword.empty())
					result.push_move(temp);
				start=i+1;
			}
		}
		return r_move(result);
	}

	int find_symm_sbk(const rbuf<rstr>& v,int begin=0)
	{
		return find_symm_word_e(v,m_optr[toptr::c_sbk_l],
			m_optr[toptr::c_sbk_r],begin);
	}

	int find_symm_sbk(const rbuf<tword>& v,int begin=0)
	{
		return find_symm_word_e(v,m_optr[toptr::c_sbk_l],
			m_optr[toptr::c_sbk_r],begin);
	}

	int find_symm_mbk(const rbuf<tword>& v,int begin=0)
	{
		return find_symm_word_e(v,m_optr[toptr::c_mbk_l],
			m_optr[toptr::c_mbk_r],begin);
	}

	int find_symm_bbk(const rbuf<rstr>& v,int begin=0)
	{
		return find_symm_word_e(v,m_optr[toptr::c_bbk_l],
			m_optr[toptr::c_bbk_r],begin);
	}

	int find_symm_bbk(const rbuf<tword>& v,int begin=0)
	{
		return find_symm_word_e(v,m_optr[toptr::c_bbk_l],
			m_optr[toptr::c_bbk_r],begin);
	}

	int find_symm_tbk(const rbuf<tword>& v,int begin=0)
	{
		return find_symm_word_e(v,m_optr[toptr::c_tbk_l],
			m_optr[toptr::c_tbk_r],begin);
	}
	
	//未找到返回结束位置，应该判断count<0，但那样会降低效率
	static int find_symm_word_e(const rbuf<tword>& v,rstr &left,rstr &right,
		int begin=0)
	{
		int end=v.count();
		if(begin<0)
		{
			begin=0;
		}
		int count=0;
		int i;
		for(i=begin;i<end;i++)
		{
			if(left==v[i].val)
				++count;
			if(right==v[i].val)
				--count;
			if(0==count)
				return i;
		}
		return v.count();
	}

	//未找到返回结束位置
	static int find_symm_word_e(const rbuf<rstr>& v,rstr &left,rstr &right,
		int begin=0)
	{
		int end=v.count();
		if(begin<0)
		{
			begin=0;
		}
		int count=0;
		int i;
		for(i=begin;i<end;i++)
		{
			if(left==v[i])
				++count;
			if(right==v[i])
				--count;
			if(0==count)
				return i;
		}
		return v.count();
	}

	static int find_symm_word_rev(const rbuf<tword>& v,rstr &left,rstr &right,
		int begin,int end)
	{
		if(end>=v.count())
			end=v.count();
		if(begin<0)
		{
			begin=0;
		}
		int count=0;
		int i;
		for(i=end-1;i>=begin;--i)
		{
			if(left==v[i].val)
				--count;
			if(right==v[i].val)
				++count;
			if(0==count)
				return i;
		}
		return v.count();
	}

	static rbuf<rstr> vword_to_vstr(const rbuf<tword>& v)
	{
		rbuf<rstr> ret;
		for(int i=0;i<v.count();i++)
			ret.push(v[i].val);
		return r_move(ret);
	}

	static void push_twi(rbuf<tword>& v,tword& twi,const rstr& val)
	{
		twi.val=val;
		v.push(twi);
	}

	rstr get_file_name(tfile* p)
	{
		if(p==null)
		{
			return rstr();
		}
		return p->name.torstr();
	}

	static void error(rstr e=rstr())
	{
#ifndef __COCOS2D_H__
		rf::printl(e);
#else
		char* p=e.cstr_t();
		CCLOG("%s",p);
#endif
	}

	static rstr get_src_str(const tword& word)
	{
		tpos pos=word.pos_src;
		if(pos.file==null)
		{
			return rstr();
		}
		rstr ret;
		ret+="\nsrc_file: "+pos.file->name.torstr();
		ret+="\nsrc_line: "+rstr(pos.line);
		ret+="\nsrc_cont: "+pos.file->line_list.get(pos.line);
		return r_move(ret);
	}

	void error(const tword& word,rstr e=rstr())
	{
		error(rstr("file: ")+get_file_name(word.pos.file)+
			rstr("\nline: ")+rstr(word.pos.line)+
			rstr("\nword: ")+word.val+rstr("\n")+e);
		error(get_src_str(word));
	}

	void error(const tsent& sent,rstr e=rstr())
	{
		error(rstr("file: ")+get_file_name(sent.pos.file)+
			rstr("\nline: ")+rstr(sent.pos.line)+rstr("\n")+e);
		sent.vword.print();
		rf::printl();
		error(get_src_str(sent.vword.get_bottom()));
	}

	void error(const tasm& oasm,rstr e=rstr())
	{
		error(rstr("file: ")+get_file_name(oasm.pos.file)+
			rstr("\nline: ")+rstr(oasm.pos.line)+rstr("\n")+e);
		oasm.vstr.print();
		rf::printl();
	}

	static void error(tfunc& tfi,rstr e=rstr())
	{
		error(&tfi,e);
	}

	static void error(tfunc* ptfi,rstr e=rstr())
	{
		if(ptfi==null)
		{
			return;
		}
		tclass* ptci=ptfi->ptci;
		if(ptci==null)
		{
			return;
		}
		rf::printl(get_func_declare(*ptci,*ptfi));
		rf::printl(e);
	}
};

#endif
