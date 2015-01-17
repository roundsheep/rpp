#ifndef zpre_h__
#define zpre_h__

#include "tsh.h"
#include "../rlib/ralgo.h"
#include "zread.h"
#include "zfind.h"

//词法分析、预处理、获取所有文件
//词表->m_file.vword
struct zpre
{
	static rbool process(tsh& sh)
	{
		if(sh.m_mode==tsh::c_jit)
		{
			sh.m_vdefine.insert_c(tmac(rstr("_JIT")));
		}
		elif(sh.m_mode==tsh::c_rvm)
		{
			sh.m_vdefine.insert_c(tmac(rppkey(c_rvm)));
		}
		elif(sh.m_mode==tsh::c_win)
		{
			sh.m_vdefine.insert_c(tmac(rstr("_WIN")));
		}
		elif(sh.m_mode==tsh::c_grub)
		{
			sh.m_vdefine.insert_c(tmac(rstr("_GRUB")));
		}
#ifdef _MSC_VER
		sh.m_vdefine.insert_c(tmac(rstr("_WIN32")));
#endif
		if(!obtain_all_file(sh))
			return false;
		for(tfile* p=sh.m_file.begin();p!=sh.m_file.end();p=sh.m_file.next(p))
			if(!obtain_def(sh,sh.m_vdefine,p->vword))
				return false;
		for(tfile* p=sh.m_file.begin();p!=sh.m_file.end();p=sh.m_file.next(p))
			if(!ifdef_replace(sh,sh.m_vdefine,p->vword))
				return false;
		for(tfile* p=sh.m_file.begin();p!=sh.m_file.end();p=sh.m_file.next(p))
		{
			ifn(def_replace(sh,sh.m_vdefine,p->vword))
			{
				return false;
			}
		}
		return true;
	}

	static rbool str_analyse(tsh& sh,rstr& src,rbuf<tword>& dst,tfile* pfile)
	{
		if(!zread::word_analyse(sh,src,dst,pfile))
		{
			return false;
		}
		const_replace(dst);
		double_combine(dst);
		key_replace(sh,dst);
		return true;
	}

	static rbool obtain_all_file(tsh& sh)
	{
		while(true)
		{
			int cur=sh.m_file.count();
			rbuf<rstrw> vname;
			for(tfile* p=sh.m_file.begin();
				p!=sh.m_file.end();p=sh.m_file.next(p))
			{
				count_tab(*p);
				if(!str_analyse(sh,p->cont,p->vword,p))
				{
					return false;
				}
				//仅编译器内部使用
				ifn(ifdef_replace(sh,sh.m_vdefine,p->vword))
				{
					return false;
				}
				if(!obtain_name(sh,vname,p->vword,*p))
				{
					sh.error(rstr("obtain error ")+
						sh.get_file_name(p));
					return false;
				}
			}
			if(!sh.m_is_pack_read)
			{
				if(!import_file(sh,vname))
				{
					sh.error("import error");
					return false;
				}
			}
			if(cur==sh.m_file.count())
				return true;
		}
	}

	static rbool import_file(tsh& sh,rbuf<rstrw>& vname)
	{
		for(int i=0;i<vname.count();i++)
			if(!zread::read_file(sh,vname[i]))
			{
				sh.error("can't read file "+vname[i].torstr());
				return false;
			}
		return true;
	}

	static rbool obtain_name(tsh& sh,rbuf<rstrw>& vname,rbuf<tword>& v,tfile& f)
	{
		rstrw exe_dir=rdir::get_exe_dir();
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_import))
			{
				continue;
			}
			if(v.get(i-1).val==rppoptr(c_pre))
				v[i-1].clear();
			v[i].val.clear();
			rstrw name;
			if(v.get(i+1).is_cstr())
			{
				name=v.get(i+1).val;
				v[i+1].clear();
			}
			else
			{
				//处理不带引号的import
				name+=rstrw("\"");
				int j;
				for(j=i+1;j<v.count();j++)
				{
					if(v[j].pos.line!=v[i].pos.line)
					{
						break;
					}
					name+=rstrw(v[j].val);
					v[j].val.clear();
				}
				name+=rstrw("\"");
			}
			if(name.count()<3)
				return false;
			if(sh.m_is_pack_read)
			{
				continue;
			}
			name.pop();
			name.pop_front();
			name=rdir::dir_std(name);
			rstrw temp=get_abs_name(rdir::get_prev_dir(f.name),name);
			if(vname.exist(temp))
				continue;
			if(zread::exist(sh,temp))
				continue;
			if(!zread::file_exist(temp))
			{
				rstrw dir;
				if(rdir::get_suffix(name)==rstrw("rp"))
					dir=rstrw("rp/");
				else
					dir=rstrw("rsrc/");
				temp=get_abs_name(exe_dir+dir,name);
			}
			if(vname.exist(temp))
				continue;
			if(zread::exist(sh,temp))
				continue;
			if(!zread::file_exist(temp))
				return false;
			vname.push(temp);
		}
		arrange(v);
		return true;
	}

	static rstrw get_abs_name(rstrw path,const rstrw& name)
	{
		if(path.empty())
			return rstrw();
		rbuf<rstrw> temp=r_split(name,rstrw("/"));
		if(temp.empty())
			return rstrw();
		for(int i=0;i<temp.count();i++)
		{
			if(temp[i]==rstrw(".."))
				path=rdir::get_prev_dir(path);
			elif(temp[i]==rstrw("."))
				;
			else
			{
				path+=temp[i];
				if(i!=temp.count()-1)
					path+=rstrw("/");
			}
		}
		return r_move(path);
	}

	static rstrw get_abs_path(const rstrw& s)
	{
		if(s.sub(0,2)==rstrw("//")||s.sub(1,3)==rstrw(":/"))
		{
			return s;
		}
		return get_abs_name(rdir::get_cur_dir(),s);
	}

	static void key_replace(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val==rppkey(c_include))
				v[i].val=rppkey(c_import);
			//这里不能用超级宏实现，因为有带冒号和不带冒号两种
			if(v[i].val==rppkey(c_private))
			{
				v[i].clear();
				if(rppoptr(c_colon)==v.get(i+1).val)
					v[i+1].clear();
			}
			if(v[i].val==rppkey(c_public))
			{
				v[i].clear();
				if(rppoptr(c_colon)==v.get(i+1).val)
					v[i+1].clear();
			}
			if(v[i].val==rppkey(c_protected))
			{
				v[i].clear();
				if(rppoptr(c_colon)==v.get(i+1).val)
					v[i+1].clear();
			}
		}
		arrange(v);
	}

	static rbool def_replace_one(tsh& sh,rbuf<tword>& v,rset<tmac>& vmac)
	{
		tmac item;
		for(int i=0;i<v.count();i++)
		{
			item.name=v[i].val;
			tmac* p=vmac.find(item);
			if(p==null)
			{
				continue;
			}
			if(p->is_super)
			{
				ifn(zsuper::replace_item(sh,v,i,*p))
				{
					return false;
				}
			}
			else
			{
				v[i].val.clear();
				v[i].multi=p->vstr;
			}
		}
		return true;
	}

	static rbool def_replace(tsh& sh,rset<tmac>& vdefine,rbuf<tword>& v)
	{
		for(int i=0;i<c_rpp_deep;i++)
		{
			ifn(def_replace_one(sh,v,vdefine))
			{
				return false;
			}
			rbool need;
			arrange(v,&need);
			ifn(need)
			{
				return true;
			}
		}
		return false;
	}

	static rbool need_arrange(rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].empty())
				return true;
			elif(v[i].is_multi())
				return true;
		}
		return false;
	}

	static void arrange(rbuf<tword>& src,rbool* pneed=null)
	{
		if(!need_arrange(src))
		{
			if(pneed!=null)
				*pneed=false;
			return;
		}
		if(pneed!=null)
			*pneed=true;
		rbuf<tword> wlist(src.count()*2);
		wlist.m_count=0;
		for(int i=0;i<src.count();i++)
		{
			if(src[i].empty())
				continue;
			elif(src[i].is_multi())
			{
				for(int j=0;j<src[i].multi.count();++j)
				{
					tword word;
					word.val=r_move(src[i].multi[j]);
					word.pos_src=src[i].pos_src;
					word.pos=src[i].pos;
					wlist.push_move(word);
				}
			}
			else
			{
				wlist.push_move(src[i]);
			}
		}
		src=r_move(wlist);
	}

	static rbool obtain_def(tsh& sh,rset<tmac>& vdefine,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_define))
			{
				continue;
			}
			if(v.get(i-1).val==rppoptr(c_pre))
				v[i-1].clear();
			if(v.get(i+1)=="$")
			{
				int right=i;
				ifn(zsuper::add_super_mac(sh,v,right,vdefine))
				{
					return false;
				}
				sh.clear_word_val(v,i,right+1);
				i=right;
				continue;
			}
			tmac item;
			item.name=v.get(i+1).val;
			if(item.name.empty())
			{
				sh.error(v[i]);
				return false;
			}
			for(int k=i+2;k<v.count()&&v.get(k).pos==v[i].pos;k++)
			{
				item.vstr.push(v[k].val);
				v[k].clear();
			}
			if(vdefine.exist(item))
			{
				vdefine.erase(item);
				/*sh.error(v.get(i+1),"redefined");
				return false;*/
			}
			vdefine.insert(item);
			v[i+1].clear();
			v[i].clear();
		}
		arrange(v);
		return true;
	}

	static rbool ifdef_replace(tsh& sh,rset<tmac>& vdefine,rbuf<tword>& v)
	{
		tmac item;
		for(int i=v.count()-1;i>=0;i--)
		{
			if(v[i]!=rppoptr(c_pre))
			{
				continue;
			}
			rstr key=v.get(i+1).val;
			if(key!=rppkey(c_ifdef)&&
				key!=rppkey(c_ifndef))
				continue;
			if(i+2>=v.count())
			{
				sh.error(v[i],"ifdef");
				return false;
			}
			item.name=v[i+2].val;
			int endpos=r_find_pos(v,tword(rppkey(c_endif)),i+3);
			if(endpos>=v.count())
			{
				sh.error(v[i],"ifdef");
				return false;
			}
			int elsepos=endpos;
			for(int j=i+3;j<endpos;j++)
				if(v[j]==rppoptr(c_pre)&&
					v.get(j+1)==rppkey(c_else))
				{
					elsepos=j+1;
					break;
				}
			rbool defined=vdefine.exist(item);
			if(key==rppkey(c_ifdef))
			{
				if(defined)
					sh.clear_word_val(v,elsepos,endpos);
				else
					sh.clear_word_val(v,i,elsepos);
			}
			else
			{
				if(!defined)
					sh.clear_word_val(v,elsepos,endpos);
				else
					sh.clear_word_val(v,i,elsepos);
			}
			v[i].clear();
			v[i+1].clear();
			v[i+2].clear();
			v[endpos-1].clear();
			v[endpos].clear();
			v[elsepos-1].clear();
			v[elsepos].clear();
		}
		arrange(v);
		return true;
	}

	static void double_combine(rbuf<tword>& v)
	{
		for(int i=1;i<v.count()-1;i++)
		{
			if(v[i]=="."&&
				v[i-1].val.is_number()&&
				v[i+1].val.is_number())
			{
				v[i-1].val+=v[i].val;
				v[i-1].val+=v[i+1].val;
				v[i].clear();
				v[i+1].clear();
				i++;
			}
		}
		arrange(v);
	}

	static void const_replace(rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			const_replace(v[i]);
		}
	}

	static void const_replace(tword& word)
	{
		if(word.val.count()>0&&rstr::is_number(word.val[0]))
		{
			if(r_find_pos<rstr,uchar>(word.val,'_')<word.val.count())
			{
				rstr s;
				for(int i=0;i<word.val.count();i++)
					if(word.val[i]!='_')
						s+=word.val[i];
				word.val=r_move(s);
			}
		}
		if(word.val.count()>2)
		{
			if(word.val[0]=='0'&&word.val[1]=='x')
			{
				word.val=rstr::hextodec(word.val.sub(2));
			}
			if(word.val[0]=='0'&&word.val[1]=='b')
			{
				word.val=rstr::bintodec(word.val.sub(2));
			}
		}
	}

	static int count_tab_line(rstr& s)
	{
		int i;
		for(i=0;i<s.count();i++)
		{
			if(s[i]!=' '&&s[i]!=0x9)
				break;
		}
		int sum=0;
		for(int k=0;k<i;k++)
		{
			if(s[k]==' ')
			{
				sum++;
			}
			elif(s[k]==0x9)
			{
				sum+=4;
			}
		}
		return sum/4;
	}

	static void count_tab(tfile& file)
	{
		file.line_list=r_split_e(file.cont,rstr("\n"));
		file.line_list.push_front("");//行号从1开始
		for(int i=0;i<file.line_list.count();i++)
		{
			file.tab_list.push(count_tab_line(file.line_list[i]));
		}
	}

	static rbool op_const_eval(tsh& sh,rbuf<tword>& v,rbool clear_sbk)
	{
		if(!rppconf(c_op_const_eval))
			return true;
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppoptr(c_sbk_l))
				continue;
			int right=sh.find_symm_sbk(v,i);
			if(right>=v.count())
				continue;
			int pos=i+1;
			if(pos+1>=right)
				continue;
			if(!zread::is_const_str(sh,v,pos,right))
				continue;
			int dst;
			if(!zread::const_eval(sh,
				sh.vword_to_vstr(v.sub(pos,right)),dst))
			{
				sh.error(v[i],"const_eval");
				return false;
			}
			v[pos].val=rstr((uint)dst);
			sh.clear_word_val(v,pos+1,right);
			if(clear_sbk)
			{
				v[i].val.clear();
				v[right].val.clear();
			}
		}
		arrange(v);
		return true;
	}
};

#endif