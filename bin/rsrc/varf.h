
namespace varf
{
	void* new()
	{
		sub esp,4
		calle c_new
		mov s_ret,[esp]
		add esp,4
	}
	
	void* new_copy(void* p)
	{
		sub esp,4
		push p
		calle c_new_copy
		mov s_ret,[esp]
		add esp,4
	}
	
	void delete(void* p)
	{
		push p
		calle c_delete
	}
	
	void set_int(void* p,int a)
	{
		push a
		push p
		calle c_set_int
	}
	
	void set_double(void* p,double a)
	{
		sub esp,8
		mov8 [esp],a
		push p
		calle c_set_double
	}
	
	void set_rstr(void* p,char* s,int size)
	{
		push size
		push s
		push p
		calle c_set_rstr
	}
	
	void set_var(void* p,void* a)
	{
		push a
		push p
		calle c_set_var
	}
	
	void set_func(void* p,void* a)
	{
		push a
		push p
		calle c_set_func
	}
	
	void* add(void* a,void* b)
	{
		sub esp,4
		push b
		push a
		calle c_add
		mov s_ret,[esp]
		add esp,4
	}
	
	void* minus(void* a,void* b)
	{
		sub esp,4
		push b
		push a
		calle c_minus
		mov s_ret,[esp]
		add esp,4
	}
	
	void* star(void* a,void* b)
	{
		sub esp,4
		push b
		push a
		calle c_star
		mov s_ret,[esp]
		add esp,4
	}
	
	void* divide(void* a,void* b)
	{
		sub esp,4
		push b
		push a
		calle c_divide
		mov s_ret,[esp]
		add esp,4
	}
	
	void* modulo(void* a,void* b)
	{
		sub esp,4
		push b
		push a
		calle c_modulo
		mov s_ret,[esp]
		add esp,4
	}
	
	void* sub(void* a,int begin,int end)
	{
		sub esp,4
		push end
		push begin
		push a
		calle c_sub
		mov s_ret,[esp]
		add esp,4
	}
	
	bool equal(void* a,void* b)
	{
		sub esp,4
		push b
		push a
		calle c_equal
		mov s_ret,[esp]
		add esp,4
	}
	
	bool less(void* a,void* b)
	{
		sub esp,4
		push b
		push a
		calle c_less
		mov s_ret,[esp]
		add esp,4
	}
	
	int toint(void* p)
	{
		sub esp,4
		push p
		calle c_toint
		mov s_ret,[esp]
		add esp,4
	}
	
	int get_rstr(void* p,char* buf)
	{
		sub esp,4
		push buf
		push p
		calle c_get_rstr
		mov s_ret,[esp]
		add esp,4
	}
	
	void* findf(void* p,char* name)
	{
		sub esp,4
		push name
		push p
		calle c_findf
		mov s_ret,[esp]
		add esp,4
	}
	
	var& quote_attr(void* p,char* name)
	{
		sub esp,4
		push name
		push p
		calle c_quote_attr
		mov s_ret,[esp]
		add esp,4
	}
	
	bool have(void* p,char* name)
	{
		sub esp,4
		push name
		push p
		calle c_have
		mov s_ret,[esp]
		add esp,4
	}
	
	void insert_pair(void* p,void* name,void* val)
	{
		push val
		push name
		push p
		calle c_insert_pair
	}
	
	int count(void* p)
	{
		sub esp,4
		push p
		calle c_count
		mov s_ret,[esp]
		add esp,4
	}
	
	void* stringify(void* p)
	{
		sub esp,4
		push p
		calle c_stringify
		mov s_ret,[esp]
		add esp,4
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
}

void* _js_findf(var* p,char* name)
{
	return varf.findf(p->m_p,name)
}

var _js_obj(int count)
{
	var* p=&count+1
	var* pret=p+count
	pret->var
	pret->set_int(0)
	for i=0;i<count;i+=2
		varf.insert_pair(pret->m_p,p->m_p,(p+1)->m_p)
		p->~var
		(p+1)->~var
		p+=2
	mov ecx,4
	imul ecx,count
	add ecx,4
	pop ebp
	add esp,sizeof(s_local)
	reti ecx
}