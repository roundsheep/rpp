#ifndef zmain_h__
#define zmain_h__

#include "tvm.h"

//main函数进行参数解析
struct zmain
{
	static rbool write_inf(tsh& sh,rstrw name)
	{
		ifn(sh.m_db.write_new(name.torstr()))
		{
			return false;
		}
		ifn(sh.m_db.write_new(rfile::read_all_n(name)))
		{
			return false;
		}
		return true;
	}

	static rbool pack(tsh& sh)
	{
		rstrw name=tsh::get_main_name(sh)+".exe";
		rfile::remove(name);
		ifn(sh.m_db.open(name,"rw"))
		{
			return false;
		}
		for(tfile* p=sh.m_file.begin();p!=sh.m_file.end();p=sh.m_file.next(p))
		{
			ifn(sh.m_db.write_new(p->name.torstr()))
			{
				return false;
			}
			ifn(sh.m_db.write_new(p->cont))
			{
				return false;
			}
		}
		ifn(write_inf(sh,"rinf/conf.txt"))
		{
			return false;
		}
		ifn(write_inf(sh,"rinf/key.txt"))
		{
			return false;
		}
		ifn(write_inf(sh,"rinf/match.txt"))
		{
			return false;
		}
		ifn(write_inf(sh,"rinf/optr.txt"))
		{
			return false;
		}
		sh.m_db.close();
		ifn(rfile::write_all_n(name,rfile::read_all_n(rdir::get_exe_name())+rfile::read_all_n(name)))
		{
			return false;
		}
		return true;
	}

	static rbuf<rstr> param_split(rstr s)
	{
		rbuf<rstr> v;
		int j;
		for(int i=0;i<s.count();i++)
		{
			if(s[i]=='"')
			{
				j=s.find("\"",i+1);
				v.push(s.sub(i+1,j));
				i=j;
			}
			elif(s[i]==' ')
			{
				continue;
			}
			else
			{
				for(j=i;j<s.count();j++)
				{
					if(s[j]==' ')
					{
						break;
					}
				}
				v.push(s.sub(i,j));
				i=j;
			}
		}
		return v;
	}

	static void tick(int& start)
	{
		int end=rf::tick();
		rstr("time:").print();
		rf::printl(rstr(end-start).cstr());
		start=end;
	}

	static rbool compile(tsh& sh,tvm& vm,rstrw name)
	{
		if(name.empty()&&sh.m_mode==tsh::c_rvm)
			name=rdir::get_exe_dir()+rstrw("example/1.h");
		name=rdir::dir_std(name);
		name=zpre::get_abs_path(name);
		sh.m_main_file=name;
		int start=rf::tick();
		if(!zread::process(sh))
		{
			tsh::error("read basic inf error");
			return false;
		}
		if(!zpre::process(sh))
		{
			tsh::error("pre process error");
			return false;
		}
		if(sh.m_is_pack_mode)
		{
			return pack(sh);
		}
		if(!zclass::process(sh))
		{
			tsh::error("extract class error");
			return false;
		}
		if(!zctl::process(sh))
		{
			tsh::error("class template replace error");
			return false;
		}
		if(sh.m_mode==tsh::c_jit)
		{
			if(!zjit::run(sh))
			{
				return false;
			}
		}
		elif(sh.m_mode==tsh::c_rvm)
		{
			if(!vm.run(sh))
			{
				return false;
			}
		}
		elif(sh.m_mode==tsh::c_win)
		{
			if(!znasm::proc(sh))
			{
				return false;
			}
		}
		elif(sh.m_mode==tsh::c_grub)
		{
			if(!znasm::proc_grub(sh))
			{
				return false;
			}
		}
		if(rppconf(c_display_inf))
		{
			rf::printl();
			tick(start);
		}
		return true;
	}

	static int rpp_main(tvm& vm,int argc,char** argv)
	{
		tsh& sh=vm.m_sh;
		rstr name;
		sh.m_argc=argc;
		sh.m_argv=argv;
		zjitf::get_psh()=&sh;
		if(argc==1)
		{
			rstr exe_name=argv[0];
			exe_name=rcode::gbk_to_utf8(exe_name);
			int real_size=rfile::get_exe_size(exe_name.cstr_t());
			if(real_size<=0)
			{
				vm.m_sh.error("can't read exe file");
				return 1;
			}
			sh.m_is_pack_read=rfile::get_size8(exe_name)>real_size;
			if(sh.m_is_pack_read)
			{
				sh.m_mode=tsh::c_jit;
				ifn(sh.m_db.open_off(exe_name,real_size))
				{
					return 1;
				}
			}
		}
		if(argc==3)
		{
			name=argv[2];
			if(argv[1]==rstr("-jit"))
			{
				sh.m_mode=tsh::c_jit;
			}
			elif(argv[1]==rstr("-win"))
			{
				sh.m_mode=tsh::c_win;
			}
			elif(argv[1]==rstr("-grub"))
			{
				sh.m_mode=tsh::c_grub;
			}
			elif(argv[1]==rstr("-pack"))
			{
				sh.m_mode=tsh::c_jit;
				sh.m_is_pack_mode=true;
			}
			else
			{
				name=argv[1];
			}
		}
		elif(argc>=2)
		{
			name=argv[1];
		}
#ifdef _MSC_VER
		name=rcode::gbk_to_utf8(name);
#endif
		sh.m_ret=0;
		if(!compile(sh,vm,name))
		{
			sh.m_ret=1;
		}
		if(rppconf(c_pause))
			rf::getch();
		return sh.m_ret;
	}
};

#endif
