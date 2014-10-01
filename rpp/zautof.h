#ifndef zautof_h__
#define zautof_h__

#include "zfind.h"

//自动增加类成员函数
struct zautof
{
	static rbool auto_add_func(tsh& sh,tclass& tci)
	{
		if(!rppconf(c_auto_addfunc))
			return true;
		if(sh.is_basic_type(tci.name)||
			tci.is_friend)
			return true;
		rbool added=false;
		tpos pos;
		pos.line=1;
		pos.file=0;
		if(!tci.vfunc.empty())
		{
			pos=tci.vfunc.begin()->first_pos;
		}
		if(zfind::destruct_search(tci)==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.first_pos=pos;
			tfi.last_pos=pos;
			tfi.last_pos.line++;
			tfi.name=rppoptr(c_destruct)+tci.name;

			tfi.param.push(tdata(tci.name+rppoptr(c_addr),rppkey(c_this)));
			tfi.retval=tdata(rppkey(c_void),"");

			tfi.name_dec=tfi.get_dec();
			tci.vfunc.insert(tfi);
			added=true;
		}
		if(zfind::emptystruct_search(tci)==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.first_pos=pos;
			tfi.last_pos=pos;
			tfi.last_pos.line++;

			tfi.name=tci.name;

			tfi.param.push(tdata(tci.name+rppoptr(c_addr),rppkey(c_this)));
			tfi.retval=tdata(rppkey(c_void),"");

			tfi.name_dec=tfi.get_dec();
			tci.vfunc.insert(tfi);
			added=true;
		}
		if(zfind::copystruct_search(tci)==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.first_pos=pos;
			tfi.last_pos=pos;
			tfi.last_pos.line++;

			tfi.name=tci.name;

			tfi.param.push(tdata(tci.name+rppoptr(c_addr),rppkey(c_this)));
			tfi.param.push(tdata(tci.name+rppoptr(c_addr),rppkey(c_rvara)));
			tfi.retval=tdata(rppkey(c_void),"");

			add_copy_sent(sh,tci,tfi);
			tfi.name_dec=tfi.get_dec();
			tci.vfunc.insert(tfi);
			added=true;
		}
		if(zfind::func_search(tci,tci.name,tci.name+"&",tci.name)==null)
		{
			tfunc tfi=*zfind::copystruct_search(tci);
			tfi.ptci=&tci;
			tfi.param[1].type=tci.name;
			tfi.name_dec=tfi.get_dec();
			tci.vfunc.insert(tfi);
			added=true;
		}
		if(zfind::func_search(tci,"=",tci.name+"&",tci.name+"&")==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.first_pos=pos;
			tfi.last_pos=pos;
			tfi.last_pos.line++;

			tfi.name="=";

			tfi.param.push(tdata(tci.name+rppoptr(c_addr),rppkey(c_this)));
			tfi.param.push(tdata(tci.name+rppoptr(c_addr),rppkey(c_rvara)));
			tfi.retval=tdata(rppkey(c_void),"");

			add_copy_sent(sh,tci,tfi);
			tfi.name_dec=tfi.get_dec();
			tci.vfunc.insert(tfi);
			added=true;
		}
		if(zfind::func_search(tci,"=",tci.name+"&",tci.name)==null)
		{
			tfunc tfi=*zfind::func_search(tci,"=",tci.name+"&",tci.name+"&");
			tfi.ptci=&tci;
			tfi.param[1].type=tci.name;
			tfi.name_dec=tfi.get_dec();
			tci.vfunc.insert(tfi);
			added=true;
		}
		if(zfind::func_search(tci,"&")==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.first_pos=pos;
			tfi.last_pos=pos;
			tfi.last_pos.line++;

			tfi.name="&";

			tfi.param.push(tdata(tci.name+rppoptr(c_addr),rppkey(c_this)));
			tfi.retval=tdata("rp<"+tci.name+">",rppkey(c_s_ret));

			tword twi;
			twi.pos=tfi.first_pos;
			twi.pos_src=tfi.first_pos;
			sh.push_twi(tfi.vword,twi,"mov");
			sh.push_twi(tfi.vword,twi,"s_ret");
			sh.push_twi(tfi.vword,twi,rppoptr(c_comma));
			sh.push_twi(tfi.vword,twi,"this");
			tfi.name_dec=tfi.get_dec();
			if(zfind::is_class(sh,tfi.retval.type))
				tci.vfunc.insert(tfi);
			added=true;
		}
		return true;
	}

	static void add_copy_sent(tsh& sh,tclass& tci,tfunc& tfi)
	{
		tword twi;
		twi.pos_src=tfi.first_pos;
		twi.pos=tfi.first_pos;
		for(int i=0;i<tci.vdata.count();i++)
		{
			if(sh.is_basic_type(tci.vdata[i].type)||
				sh.is_quote(tci.vdata[i].type))
				continue;
			sh.push_twi(tfi.vword,twi,rppkey(c_this));
			sh.push_twi(tfi.vword,twi,rppoptr(c_dot));
			sh.push_twi(tfi.vword,twi,tci.vdata[i].name);
			sh.push_twi(tfi.vword,twi,rppoptr(c_equal));
			sh.push_twi(tfi.vword,twi,rppkey(c_rvara));
			sh.push_twi(tfi.vword,twi,rppoptr(c_dot));
			sh.push_twi(tfi.vword,twi,tci.vdata[i].name);
			sh.push_twi(tfi.vword,twi,rppoptr(c_semi));
		}
	}

	static void op_rstr(tsh& sh)
	{
		if(!rppconf(c_op_empty_func))
		{
			return;
		}
		tclass* p=zfind::class_search(sh,"rbuf<char>");
		if(p!=null)
		{
			op_rstr(p->vword);
		}
	}

	static void op_rstr(rbuf<tword>& v)
	{
		for(int i=0;i<v.count()-3;i++)
		{
			if(v[i]!="r_new"&&v[i]!="r_delete")
			{
				continue;
			}
			if(v[i+1]!="<")
			{
				continue;
			}
			if(v[i+2]!="char")
			{
				continue;
			}
			if(v[i+3]!=">")
			{
				continue;
			}
			v[i].val+="_n";
		}
	}
};

#endif
