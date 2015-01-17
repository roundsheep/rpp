#ifndef tanalyse_h__
#define tanalyse_h__

#ifdef RANALYSE
struct tanalyse_item
{
	rstr name;
	int8 count;

	friend rbool operator<=(const tanalyse_item& a,const tanalyse_item& b)
	{
		return a.count>=b.count;
	}
};

struct tanalyse
{
	static void analyse_add(uint addr)
	{
		analyse_add(((tasm*)addr)->ptfi);
	}

	static void analyse_add(tfunc* ptfi)
	{
		if(ptfi==null)
		{
			return;
		}
		ptfi->count++;
	}

	static void analyse_print(tsh& sh)
	{
		rbuf<tanalyse_item> v;
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
		{
			for(tfunc* q=p->vfunc.begin();
				q!=p->vfunc.end();q=p->vfunc.next(q))
			{
				if(q->count==0)
				{
					continue;
				}
				tanalyse_item item;
				item.count=q->count;
				item.name=sh.get_func_declare_name(*p,*q);
				v.push(item);
			}
		}
		r_qsort(v);
		rf::printl();
		for(int i=0;i<v.count();i++)
		{
			(rstr(v[i].count)+" "+v[i].name).printl();
		}
	}
};
#endif

#endif
