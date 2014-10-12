#ifndef zsuper_h__
#define zsuper_h__

#include "tsh.h"

//超级宏
struct zsuper
{
	static rbool add_super_mac(tsh& sh,rbuf<tword>& v,int& i,rset<tmac>& vmac)
	{
		tmac mitem;
		mitem.is_super=true;
		mitem.name=v.get(i+2).val;
		int left=i+3;
		int right=left;
		for (;right<v.count()&&v[right].pos==v[i].pos;right++)
			;
		if (right>=v.count())
		{
			sh.error(v.get(i),"miss mac");
			return false;
		}
		if (v[right]!=rppoptr(c_bbk_l))
		{
			sh.error(v.get(i),"miss mac $ {");
			return false;
		}
		mitem.param=sh.vword_to_vstr(v.sub(left,right));
		left=right;
		right=sh.find_symm_bbk(v,left);
		if (right>=v.count())
		{
			sh.error(v.get(i),"miss mac $ }");
			return false;
		}
		mitem.vstr=sh.vword_to_vstr(v.sub(left+1,right));
		if (vmac.exist(mitem))
		{
			vmac.erase(mitem);
			/*sh.error(v.get_bottom(),"mac redefined");
			return false;*/
		}
		vmac.insert(mitem);
		i=right;
		return true;
	}

	static rbool replace_item(tsh& sh,rbuf<tword>& v,int& i,tmac& mitem)
	{
		for (int j=i+2;j<=v.count();j++)
		{
			rbuf<tword> arr;
			rbuf<rstr> src=sh.vword_to_vstr(v.sub(i+1,j));
			if (match_here(sh,mitem.param,src,arr))
			{
				ifn (replace_super_word(v[i],mitem.vstr,arr))
				{
					sh.error(v.get(i));
					return false;
				}
				sh.clear_word_val(v,i,j);
				i=j-1;
				break;
			}
		}
		return true;
	}

	//todo 限制递归深度
	static rbool match_here(tsh& sh,const rbuf<rstr>& reg,const rbuf<rstr>& src,rbuf<tword>& arr)
	{
		if (reg.empty())
		{
			return src.empty();
		}
		if (reg[0]=="_mword")
		{
			return match_multi(sh,reg.sub(1),src,arr);
		}
		if (!src.empty())
		{
			if (reg[0]=="_word")
			{
				tword temp;
				temp.multi+=src[0];
				arr.push(temp);
				if (match_here(sh,reg.sub(1),src.sub(1),arr))
				{
					return true;
				}
				arr.pop();
				return false;
			}
			if (reg[0]!=src[0])
			{
				return false;
			}
			if (src[0]==rppoptr(c_sbk_l))
			{
				int right_r=sh.find_symm_sbk(reg,0);
				if (right_r>=reg.count())
				{
					return false;
				}
				int right_s=sh.find_symm_sbk(src,0);
				if (right_s>=src.count())
				{
					return false;
				}
				return match_here(sh,reg.sub(1,right_r),src.sub(1,right_s),arr)&&
					match_here(sh,reg.sub(right_r+1),src.sub(right_s+1),arr);
			}
			return match_here(sh,reg.sub(1),src.sub(1),arr);
		}
		return false;
	}

	static rbool match_multi(tsh& sh,const rbuf<rstr>& reg,const rbuf<rstr>& src,rbuf<tword>& arr)
	{
		for (int i=0;i<=src.count();i++)
		{
			tword temp;
			temp.multi=src.sub(0,i);
			arr.push(temp);
			if (match_here(sh,reg,src.sub(i),arr))
			{
				return true;
			}
			else
			{
				arr.pop();
			}
		}
		return false;
	}

	static rbool replace_super_word(tword& word,rbuf<rstr>& vstr,rbuf<tword>& arr)
	{
		word.val.clear();
		for (int i=0;i<vstr.count();i++)
		{
			if (vstr[i]=="$"&&vstr.get(i+1).is_number())
			{
				int num=vstr.get(i+1).toint();
				if (num>=arr.count())
				{
					return false;
				}
				if (vstr.get(i+2)=="=>"&&vstr.get(i+3).is_number())
				{
					int index=vstr.get(i+3).toint();
					if (index>=arr[num].multi.count())
					{
						return false;
					}
					word.multi+=arr[num].multi[index];
					i+=3;
				}
				else
				{
					word.multi+=arr[num].multi;
					i++;
				}
			}
			else
			{
				word.multi+=vstr[i];
			}
		}
		link_sharp(word.multi);
		return true;
	}

	static rstr del_quote(rstr s)
	{
		if (s.get_bottom()=='"')
		{
			return s.sub(1,s.count()-1);
		}
		return r_move(s);
	}

	static rstr add_quote(rstr s)
	{
		return "\""+s+"\"";
	}

	static void link_sharp(rbuf<rstr>& v)
	{
		for (int i=0;i<c_rpp_deep;i++)
		{
			ifn (link_sharp_one(v))
			{
				return;
			}
		}
	}

	static rbool link_sharp_one(rbuf<rstr>& v)
	{
		rbuf<rstr> dst;
		rbool ret=false;
		for (int i=0;i<v.count();i++)
		{
			if (i<v.count()-2&&v[i+1]=="##")
			{
				rstr temp=del_quote(v[i])+del_quote(v[i+2]);
				if (v[i].get_bottom()=='"'||v[i+2].get_bottom()=='"')
				{
					temp=add_quote(temp);
				}
				dst.push(temp);
				i+=2;
				ret=true;
			}
			elif (i<v.count()-1&&v[i]=="#")
			{
				dst.push(add_quote(v[i+1]));
				i++;
			}
			else
			{
				dst.push(v[i]);
			}
		}
		v=dst;
		return ret;
	}
};

#endif
