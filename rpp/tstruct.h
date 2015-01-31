#ifndef tstruct_h__
#define tstruct_h__

#include "../rlib/rstr.h"
#include "../rlib/rf.h"
#include "../rlib/rcode.h"
#include "../rlib/rset.h"
#include "../rlib/rdic.h"
#include "tvm_struct.h"
#include "talloc.h"

struct tword
{
	rstr val;//最常用的数据成员放在首位可以略微提高效率
	rbuf<rstr> multi;//todo:rstr很难保存行号，用rbuf<tword*>可能更好
	tpos pos;//跳转定位的位置
	tpos pos_src;//原始文件位置

	tword()
	{
		clear();
	}

	tword(const rstr& s)
	{
		clear();
		val=s;
	}

	tword(tword&& a)
	{
		pos=a.pos;
		val=a.val;
		multi=a.multi;
		pos_src=a.pos_src;
	}

	void operator=(tword&& a)
	{
		val.m_buf.free();
		multi.free();
		val=a.val;
		multi=a.multi;
		pos=a.pos;
		pos_src=a.pos_src;
	}

	void clear()
	{
		pos.clear();
		val.clear();
		multi.clear();
		pos_src.clear();
	}

	rbool empty() const
	{
		return val.empty()&&multi.empty();
	}
	
	//val和multi均不空表示sizeof
	rbool is_multi() const
	{
		return !multi.empty()&&val.empty();
	}

	friend rbool operator==(const tword& a,const tword& b)
	{
		return a.val==b.val;
	}

	friend rbool operator!=(const tword& a,const tword& b)
	{
		return !(a==b);
	}

	friend rbool operator==(const tword& a,const rstr& b)
	{
		return a.val==b;
	}

	friend rbool operator!=(const tword& a,const rstr& b)
	{
		return !(a==b);
	}

	friend rbool operator==(const rstr& a,const tword& b)
	{
		return a==b.val;
	}

	friend rbool operator!=(const rstr& a,const tword& b)
	{
		return !(a==b);
	}

	void print()
	{
		val.print();
	}

	rbool is_name() const
	{
		if(val.empty())
			return false;
		int i=0;
		if(rstr::is_alpha(val.get(i))||'_'==val.get(i))
			i++;
		elif(rcode::is_utf8_3(val.get(i))&&val.get(i+1)&&val.get(i+2))
		{
			i+=3;
		}
		elif(rcode::is_utf8_2(val.get(i))&&val.get(i+1))
		{
			i+=2;
		}
		else
			return false;
		for(;i<val.count();i++)
		{
			if(rstr::is_number(val[i])||rstr::is_alpha(val[i])||
				'_'==val[i]||'.'==val[i])
				continue;
			elif(rcode::is_utf8_3(val[i])&&val.get(i+1)&&val.get(i+2))
			{
				i+=2;
				continue;
			}
			elif(rcode::is_utf8_2(val[i])&&val.get(i+1))
			{
				i++;
				continue;
			}
			else
				return false;
		}
		return true;
	}

	rbool is_cint() const
	{
		return val.is_number();
	}

	rbool is_cuint() const
	{
		return val.sub(0,val.count()-1).is_number()&&val.get_top()=='u';
	}

	rbool is_caddr() const
	{
		if('&'!=val.get(0)||')'!=val.get_top())
		{
			return false;
		}
		for(int i=0;i<val.count()-2;i++)
			if(val[i]=='.')
				return true;
		return false;
	}

	rbool is_cdouble() const
	{
		if(!rstr::is_number(val.get_bottom())||
			!rstr::is_number(val.get_top()))
			return false;
		int count=0;
		for(int i=1;i<val.count();i++)
			if(val[i]=='.')
				count++;
		return count==1;
	}

	//常量指针，一般只用于null
	rbool is_cpoint() const
	{
		return val.sub(0,val.count()-1).is_number()&&val.get_top()=='p'||
			is_caddr();
	}

	//字符串常量，如"a"
	rbool is_cstr() const
	{
		return val.get_bottom()=='"';
	}

	rbool is_const() const
	{
		return rstr::is_number(val.get(0))||val.get(0)=='"'||is_caddr();
	}
};

struct tmac
{
	rstr name;
	rbuf<rstr> param;
	rbuf<rstr> vstr;
	rbool is_super;

	tmac()
	{
		is_super=false;
	}

	tmac(const rstr& a)
	{
		name=a;
		is_super=false;
	}

	friend rbool operator<(const tmac& a,const tmac& b)
	{
		return a.name<b.name;
	}
};

struct ttl
{
	rstr name;
	rstr val;//默认值
};

struct tfile
{
	rstrw name;
	rstr cont;
	rbuf<tword> vword;
	rbuf<rstr> line_list;
	rbuf<int> tab_list;//前面tab个数，1个tab等于4个空格（todo:改成8个空格）

	tfile()
	{
	}

	tfile(const rstrw& name)
	{
		this->name=name;
	}

	friend rbool operator<(const tfile& a,const tfile& b)
	{
		return a.name<b.name;
	}
};

struct tdata
{
	rstr type;
	rstr name;
	int size;//真实大小
	int count;//数组个数
	int off;//相对偏移
	rbuf<tword> param;//默认参数

	tdata()
	{
		clear();
	}

	tdata(const rstr& type)
	{
		clear();
		this->type=type;
	}

	tdata(const rstr& type,const rstr& name)
	{
		clear();
		this->type=type;
		this->name=name;
	}

	void clear()
	{
		type.clear();
		name.clear();
		size=0;
		count=0;
		off=0;
		param.clear();
	}

	void print()
	{
		type.print();
		name.print();
	}

	friend rbool operator==(const tdata& a,const tdata& b)
	{
		return a.name==b.name;
	}
};

//表达式和语句几乎没有区别
struct tsent
{
	rbuf<tword> vword;
	rstr type;//表达式类型，也可以理解为语句返回值的类型
	tpos pos;

	tsent()
	{
		clear();
	}

	tsent(tsent&& a)
	{
		pos=a.pos;
		type=a.type;
		vword=a.vword;
	}

	void operator=(tsent&& a)
	{
		type.m_buf.free();
		vword.free();
		pos=a.pos;
		type=a.type;
		vword=a.vword;
	}

	void clear()
	{
		pos.clear();
		type.clear();
		vword.clear();
	}

	tsent sub(int begin,int end) const
	{
		tsent ret;
		ret.pos=pos;
		ret.vword=vword.sub(begin,end);//类型信息为空
		return r_move(ret);
	}

	rbool empty() const
	{
		return vword.empty();
	}
	void print()
	{
		vword.print();
	}
};

struct tdynamic
{
	int key;
	rbuf<rbuf<rstr> > vparam;

	tdynamic()
	{
	}

	friend rbool operator<(const tdynamic& a,const tdynamic& b)
	{
		return a.key<b.key;
	}
};

struct tclass;

struct tfunc
{
	rbool is_cfunc;
	rbool is_dynamic;
	rbool is_friend;
	rstr lambda_data;
	tdata retval;
	rstr name;
	rstr name_dec;//增加一个dec变量是为了提高查找效率

	rbuf<ttl> vtl;
	rbuf<tdata> param;//参数列表
	rbuf<tdata> local;//局部变量表
	
	rbuf<tword> vword;//函数内容的词表
	rbuf<tsent> vsent;//语句表
	rbuf<tasm> vasm;

	tpos first_pos;//first_pos似乎没用
	tpos last_pos;
	
	int8 count;
	tclass* ptci;//反射到tclass，不用初始化

	uchar* code;//jit代码段
	rset<tdynamic> sdynamic;

	tfunc()
	{
		clear();
	}

	~tfunc()
	{
		if(code!=null)
		{
			talloc::free_v(code);//注意jit以后不能直接调用默认的拷贝构造函数
		}
	}

	void clear()
	{
		is_cfunc=false;
		is_dynamic=false;
		is_friend=false;
		lambda_data.clear();
		retval.clear();
		name.clear();
		name_dec.clear();

		vtl.clear();
		param.clear();
		local.clear();
		
		vword.clear();
		vsent.clear();
		vasm.clear();

		first_pos.clear();
		last_pos.clear();
		
		count=0;
		//ptci=null;
		code=null;
		sdynamic.clear();
	}

	rstr get_dec() const
	{
		rstr s;
		s+=name+"(";
		for(int j=0;j<param.count();j++)
		{
			if(j!=0)
			{
				s+=",";
			}
			s+=param[j].type;
		}
		s+=")";
		return r_move(s);
	}

	friend rbool operator<(const tfunc& a,const tfunc& b)
	{
		return a.name_dec<b.name_dec;
	}
};

struct tclass
{
	rstr name;
	rset<tmac> vmac;//宏列表
	rbuf<tdata> vdata;//数据成员列表
	rset<tfunc> vfunc;//函数成员列表
	rset<tfunc> vfunctl;//模板函数列表

	rbuf<tword> vword;
	rbuf<ttl> vtl;
	rbuf<tsent> vfather;//父类表

	int size;
	rbool is_friend;

	void clear()
	{
		name.clear();
		vmac.clear();
		vdata.clear();
		vfunc.clear();
		vfunctl.clear();

		vword.clear();
		vtl.clear();
		vfather.clear();

		size=0;
		is_friend=false;
	}

	tclass()
	{
		clear();
	}

	friend rbool operator<(const tclass& a,const tclass& b)
	{
		return a.name<b.name;
	}
};

struct taddr
{
	uint start;
	uint end;
	tfunc* ptfi;

	taddr()
	{
	}

	taddr(uint start,uint end,tfunc* ptfi)
	{
		this->start=start;
		this->end=end;
		this->ptfi=ptfi;
	}

	friend rbool operator<(const taddr& a,const taddr& b)
	{
		return a.end<=b.start;
	}
};

struct tenv
{
	tfunc* ptfi;
	uint v_ebp;

	tenv()
	{
		ptfi=null;
		v_ebp=0;
	}
};

#endif
