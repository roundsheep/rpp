#ifndef tvar_h__
#define tvar_h__

#include "../rlib/rdic.h"

#define tvar_dic rdic<tvar*>

struct tvar
{
	//tclass* type;用指针可以支持用户自定义类型，但效率较低
	tvar* m_quote;//用于RPP代码引用到tvar
	int m_type;
	int8 m_val;//不初始化

	~tvar()
	{
		clear();
	}

	tvar()
	{
		m_quote=this;
		m_type=c_void;
	}

	tvar(int a)
	{
		m_quote=this;
		m_type=c_int;
		m_val=a;
	}

	tvar(double a)
	{
		m_quote=this;
		m_type=c_double;
		*(double*)&m_val=a;
	}

	tvar(tfunc* a)
	{
		m_quote=this;
		m_type=c_func;
		m_val=(int8)a;
	}

	tvar(const rstr& a)
	{
		m_quote=this;
		m_type=c_rstr;
		m_val=(int8)new rstr(a);
	}

	tvar(const tvar& a)
	{
		m_quote=this;
		copy(a);
	}

	void operator=(const tvar& a)
	{
		clear();
		copy(a);
	}

	void copy(const tvar& a)
	{
		m_type=a.m_type;
		if(a.m_type==c_rstr)
		{
			m_val=(int8)new rstr(v_pto_rstr(a.m_val));
		}
		elif(a.m_type==c_dic)
		{
			m_val=(int8)new tvar_dic;
			copy_dic(*(tvar_dic*)m_val,*(tvar_dic*)a.m_val);
		}
		else
		{
			m_val=a.m_val;
		}
	}

	static void copy_dic(tvar_dic& dst,tvar_dic& src)
	{
		for(tvar** p=src.begin();p!=src.end();p=src.next(p))
		{
			rdic_i<tvar*>* q=(rdic_i<tvar*>*)p;
			dst[q->key]=new tvar(**p);
		}
	}

	int toint()
	{
		return (int)m_val;
	}

	double todouble()
	{
		return *(double*)&m_val;
	}

	rstr torstr()
	{
		if(m_type==c_int)
		{
			return rstr((int)m_val);
		}
		elif(m_type==c_rstr)
		{
			return *(rstr*)m_val;
		}
		elif(m_type==c_double)
		{
			return rstr(*(double*)&m_val);
		}
		else
		{
			return rstr();
		}
	}

	tvar* quote_attr(char* name)
	{
		if(m_type!=c_dic)
		{
			set_dic();
		}
		tvar_dic& src=*(tvar_dic*)m_val;
		if(src.exist(name))
		{
			return *src.find(name);
		}
		tvar* p=new tvar;
		src[name]=p;
		return p;
	}

	void set_dic()
	{
		clear();
		m_type=c_dic;
		m_val=(int8)new tvar_dic;
	}

	void set_int(int a)
	{
		clear();
		m_type=c_int;
		m_val=a;
	}

	void set_double(double a)
	{
		clear();
		m_type=c_double;
		*(double*)&m_val=a;
	}

	void set_rstr(const rstr& a)
	{
		clear();
		m_type=c_rstr;
		m_val=(int8)new rstr(a);
	}

	void set_func(tfunc* ptfi)
	{
		clear();
		m_type=c_func;
		m_val=(int8)ptfi;
	}

	void set_var(const tvar& a)
	{
		clear();
		copy(a);
	}

	void insert_pair(tvar* name,tvar* val)
	{
		if(m_type!=c_dic)
		{
			set_dic();
		}
		tvar_dic& src=*(tvar_dic*)m_val;
		src[name->torstr()]=new tvar(*val);
	}

	static rbool equal(tvar* a,tvar* b)
	{
		if(a==null)
		{
			return b==null||b->m_type==c_void;
		}
		if(b==null)
		{
			return a==null||a->m_type==c_void;
		}
		if(a->m_type==c_rstr||b->m_type==c_rstr)
		{
			return a->torstr()==b->torstr();
		}
		elif(a->m_type==c_double||b->m_type==c_double)
		{
			return a->todouble()==b->todouble();
		}
		else
		{
			return a->toint()==b->toint();
		}
	}

	static rbool less(tvar* a,tvar* b)
	{
		if(a==null||b==null)
		{
			return false;
		}
		if(a->m_type==c_rstr||b->m_type==c_rstr)
		{
			return a->torstr()<b->torstr();
		}
		elif(a->m_type==c_double||b->m_type==c_double)
		{
			return a->todouble()<b->todouble();
		}
		else
		{
			return a->toint()<b->toint();
		}
	}

	static tvar* add(tvar* a,tvar* b)
	{
		tvar* p=new tvar;
		if(a->m_type==c_rstr||b->m_type==c_rstr)
		{
			p->set_rstr(a->torstr()+b->torstr());
		}
		elif(a->m_type==c_double||b->m_type==c_double)
		{
			p->set_double(a->todouble()+b->todouble());
		}
		else
		{
			p->set_int(a->toint()+b->toint());
		}
		return p;
	}

	static tvar* minus(tvar* a,tvar* b)
	{
		tvar* p=new tvar;
		if(a->m_type==c_double||b->m_type==c_double)
		{
			p->set_double(a->todouble()-b->todouble());
		}
		else
		{
			p->set_int(a->toint()-b->toint());
		}
		return p;
	}

	static tvar* star(tvar* a,tvar* b)
	{
		tvar* p=new tvar;
		if(a->m_type==c_double||b->m_type==c_double)
		{
			p->set_double(a->todouble()*b->todouble());
		}
		else
		{
			p->set_int(a->toint()*b->toint());
		}
		return p;
	}

	static tvar* divide(tvar* a,tvar* b)
	{
		tvar* p=new tvar;
		if(a->m_type==c_double||b->m_type==c_double)
		{
			p->set_double(a->todouble()/b->todouble());
		}
		else
		{
			p->set_int(a->toint()/b->toint());
		}
		return p;
	}

	static tvar* modulo(tvar* a,tvar* b)
	{
		tvar* p=new tvar;
		p->set_int(a->toint()%b->toint());
		return p;
	}

	tvar* sub(int begin,int end)
	{
		tvar* p=new tvar;
		if(m_type==c_rstr)
		{
			p->set_rstr(((rstr*)m_val)->sub(begin,end));
		}
		elif(m_type==c_dic)
		{
			p->set_dic();
			tvar_dic& src=*(tvar_dic*)m_val;
			tvar_dic& dst=*(tvar_dic*)p->m_val;
			for(int i=begin;i<end;i++)
			{
				tvar** temp=src.find(rstr(i));
				if(temp==null)
				{
					continue;
				}
				tvar* q=*temp;
				dst.insert(i-begin,new tvar(*q));
			}
		}
		return p;
	}

	void clear()
	{
		if(m_type==c_rstr)
		{
			delete r_to_prstr(m_val);
		}
		elif(m_type==c_dic)
		{
			free_dic(*(tvar_dic*)m_val);
			delete (tvar_dic*)m_val;
		}
		m_type=c_void;
	}

	int count()
	{
		if(m_type==c_dic)
		{
			tvar_dic& src=*(tvar_dic*)m_val;
			return src.count();
		}
		elif(m_type==c_rstr)
		{
			return ((rstr*)m_val)->count();
		}
		else
		{
			return 0;
		}
	}

	static rstr stringify(tvar& a)
	{
		rstr s;
		if(a.m_type==c_int)
		{
			s=a.toint();
		}
		elif(a.m_type==c_double)
		{
			s=a.todouble();
		}
		elif(a.m_type==c_rstr)
		{
			s="\'"+a.torstr()+"\'";
		}
		elif(a.m_type==c_func)
		{
			s=a.toint();
		}
		elif(a.m_type==c_dic)
		{
			tvar_dic& src=*(tvar_dic*)a.m_val;
			s+="{";
			int i=0;
			for(tvar** p=src.begin();p!=src.end();p=src.next(p))
			{
				if(src.get_key(p)!=i)
				{
					s+="\'"+src.get_key(p)+"\'";
					s+=":";
				}
				s+=stringify(**p);
				s+=",";
				i++;
			}
			if(s.get_top()==',')
			{
				s.pop();
			}
			s+="}";
		}
		return s;
	}

	static void free_dic(tvar_dic& src)
	{
		for(tvar** p=src.begin();p!=src.end();p=src.next(p))
		{
			delete *p;
		}
	}

	rbool have(char* name)
	{
		if(m_type!=c_dic)
		{
			return false;
		}
		tvar_dic& src=*(tvar_dic*)m_val;
		tvar** p=src.find(name);
		return p!=null;
	}

	void* findf(char* name)
	{
		if(m_type!=c_dic)
		{
			return null;
		}
		tvar_dic& src=*(tvar_dic*)m_val;
		tvar** p=src.find(name);
		tvar* q=*p;
		if(q->m_type!=c_func)
		{
			return null;
		}
		return (void*)q->m_val;
	}

	enum
	{
		c_void,
		c_int,
		c_double,
		c_rstr,
		c_dic,
		c_func,
	};

	enum
	{
		c_new=2000,
		c_new_copy,
		c_delete,

		c_set_int,
		c_set_double,
		c_set_rstr,
		c_set_var,
		c_set_func,

		c_add,
		c_minus,
		c_star,
		c_divide,
		c_modulo,
		c_sub,//求子串或者子数组

		c_equal,
		c_less,

		c_toint,
		c_get_rstr,

		c_findf,
		c_quote_attr,
		c_have,
		c_insert_pair,
		c_count,
		c_stringify,
	};
};

#endif