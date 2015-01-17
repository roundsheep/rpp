#ifndef rcode_h__
#define rcode_h__

#include "rf.h"
#include "rclass.h"

struct rcode
{
	static rstr gbk_to_utf8(const rstr& s)
	{
		return utf16_to_utf8(gbk_to_utf16(s));
	}

#ifdef _MSC_VER
	static rstr gbk_to_utf16(rstr s)
	{
		rstr result;
		//即使是英语操作系统应该也安装936代码页
		int size=MultiByteToWideChar(936,0,s.cstr_t(),-1,null,0);
		if(size<=0)
		{
			return result;
		}
		result.set_size(size*2);
		size=MultiByteToWideChar(936,0,s.cstr_t(),-1,result.cstrw_t(),size);
		if(size<=0)
		{
			return "";
		}
		result.m_buf.m_count-=2;
		return r_move(result);
	}
#else
	static rstr gbk_to_utf16(const rstr& s)
	{
		rstr result;
		ushort temp;
		ushort wuni;
		for(int i=0;i<s.count();i++)
		{
			get_first(temp)=s[i];
			get_second(temp)=0;
			wuni=gbk(temp);
			if(wuni!=0)
			{
				result+=get_first(wuni);
				result+=get_second(wuni);
				continue;
			}
			if(i+1>=s.count())
			{
				continue;
			}
			get_first(temp)=s[i];
			get_second(temp)=s[i+1];
			wuni=gbk(temp);
			if(wuni==0)
			{
				continue;
			}
			result+=get_first(wuni);
			result+=get_second(wuni);
			i++;
		}
		return r_move(result);
	}
#endif

	static rstr utf8_to_gbk(const rstr& s)
	{
		return utf16_to_gbk(utf8_to_utf16(s));
	}

	static rstr utf8_to_utf16(const rstr& s)
	{
		ushort temp;
		rstr result;
		for(int i=0;i<s.count();i++)
		{
			if(is_utf8_3(s[i]))
			{
				if(i+2>=s.count())
				{
					continue;
				}
				temp=0;
				temp|=(s[i]&0xef)<<12;
				temp|=(s[i+1]&0x3f)<<6;
				temp|=s[i+2]&0x3f;
				result.push(get_first(temp));
				result.push(get_second(temp));
				i+=2;
			}
			elif(is_utf8_2(s[i]))
			{
				if(i+1>=s.count())
				{
					continue;
				}
				temp=0;
				temp|=(s[i]&0x1f)<<6;
				temp|=s[i+1]&0x3f;
				result.push(get_first(temp));
				result.push(get_second(temp));
				i++;
			}
			else
			{
				result.push(s[i]);
				result.push(0);
			}
		}
		return r_move(result);
	}

#ifdef _MSC_VER
	static rstr utf16_to_gbk(rstr s)
	{
		rstr result;
		int size=WideCharToMultiByte(936,0,s.cstrw_t(),-1,null,0,null,null);
		if(size<=0)
		{
			return result;
		}
		result.set_size(size);
		size=WideCharToMultiByte(936,0,s.cstrw_t(),-1,
			(char*)result.begin(),size,null,null);
		if(size<=0)
		{
			return "";
		}
		result.pop();
		return r_move(result);
	}
#else
	static rstr utf16_to_gbk(const rstr& s)
	{
		rstr result;
		if(s.count()%2!=0)
		{
			return result;
		}
		ushort temp;
		ushort wgbk;
		for(int i=0;i<s.count();i+=2)
		{
			get_first(temp)=s[i];
			get_second(temp)=s[i+1];
			wgbk=unicode(temp);
			if(get_first(wgbk)!=0)
			{
				result+=get_first(wgbk);
			}
			if(get_second(wgbk)!=0)
			{
				result+=get_second(wgbk);
			}
		}
		return r_move(result);
	}
#endif

	static rstr utf16_to_utf8(const rstr& s)
	{
		rstr result;
		if(s.count()%2!=0)
		{
			return result;
		}
		ushort temp;
		for(int i=0;i<s.count();i+=2)
		{
			if(s[i]<=0x7f&&s[i+1]==0)
			{
				result+=s[i];
				continue;
			}
			temp=*(ushort*)(s.begin()+i);
			if(s[i]>0x7f&&s[i+1]<=0x7)
			{
				result.push(uchar((temp>>6)|0xc0));
				result.push(uchar((temp&0x3f)|0x80));
			}
			else
			{
				result.push((temp>>12)|0xe0);
				result.push(((temp>>6)&0x3f)|0x80);
				result.push((temp&0x3f)|0x80);
			}
		}
		return r_move(result);
	}

	static rstr to_utf16_txt(const rstr& s)
	{
		if(is_utf16_txt(s))
		{
			return s;
		}
		rstr temp;
		temp.push(0xff);
		temp.push(0xfe);
		if(is_utf8_txt(s))
		{
			return temp+utf8_to_utf16(s.sub(3));
		}
		return temp+gbk_to_utf16(s);
	}

	static rstr add_utf8_head(const rstr& s)
	{
		rstr temp;
		temp.push(0xef);
		temp.push(0xbb);
		temp.push(0xbf);
		temp+=s;
		return r_move(temp);
	}

	static rstr to_utf8_txt(const rstr& s)
	{
		if(is_utf8_txt(s))
		{
			return s;
		}
		rstr temp;
		temp.push(0xef);
		temp.push(0xbb);
		temp.push(0xbf);
		if(is_utf16_txt(s))
		{
			return temp+utf16_to_utf8(s.sub(2));
		}
		return temp+gbk_to_utf8(s);
	}

	static rstr to_gbk_txt(const rstr& s)
	{
		if(is_utf8_txt(s))
		{
			return utf8_to_gbk(s.sub(3));
		}
		if(is_utf16_txt(s))
		{
			return utf16_to_gbk(s.sub(2));
		}
		return r_move(s);
	}

	static rbool is_utf16_txt(const rstr& s)
	{
		return s.count()>=2&&
			s[0]==0xff&&
			s[1]==0xfe;
	}

	static rbool is_utf8_txt(const rstr& s)
	{
		return s.count()>=3&&
			s[0]==0xef&&
			s[1]==0xbb&&
			s[2]==0xbf;
	}

	static rbool is_utf16_big_txt(const rstr& s)
	{
		return s.count()>=2&&
			s[0]==0xfe&&
			s[1]==0xff;
	}

	static rbool is_utf32_txt(const rstr& s)
	{
		if(s.count()<4)
		{
			return false;
		}
		if(s[0]==0&&s[1]==0&&
			s[2]==0xfe&&
			s[3]==0xff)
		{
			return true;
		}
		if(s[0]==0xff&&
			s[1]==0xfe&&
			s[2]==0&&s[3]==0)
		{
			return true;
		}
		return false;
	}

	//是否utf8 3个字节起始
	static rbool is_utf8_3(uchar ch)
	{
		return ch>=0xe0&&ch<=0xef;
	}

	static rbool is_utf8_2(uchar ch)
	{
		return ch>=0xc0&&ch<=0xdf;
	}

	//是否unicode汉字，共20902个，+2个不常用字
	static rbool is_chs(ushort w)
	{
		return w>=0x4e00&&w<0x9fa6;
	}

	//是否gbk汉字第一字节，分布范围大约 30000多
	static rbool is_chs_first(uchar ch)
	{
		return ch>=0x81&&ch<=0xfd;
	}

#ifndef _MSC_VER
	static ushort unicode(int index)
	{
		return *(ushort*)&g_unicode[index*2];
	}

	static ushort gbk(int index)
	{
		return *(ushort*)&g_gbk[index*2];
	}
#endif

	static char& get_second(ushort& ch)
	{
		return *((char*)&ch+1);
	}

	static char& get_first(ushort& ch)
	{
		return *((char*)&ch);
	}
	
	//返回所需缓存区大小
	static int gbk_to_utf8_c(uchar* src,uchar* dst)
	{
		rstr s=gbk_to_utf8(src);
		s.push(0);
		if(dst!=null)
		{
			memcpy(dst,s.begin(),s.count());
		}
		return s.count();
	}

	static int gbk_to_utf16_c(uchar* src,uchar* dst)
	{
		rstr s=gbk_to_utf16(src);
		s.push(0);
		s.push(0);
		if(dst!=null)
		{
			memcpy(dst,s.begin(),s.count());
		}
		return s.count();
	}

	static int utf8_to_gbk_c(uchar* src,uchar* dst)
	{
		rstr s=utf8_to_gbk(src);
		s.push(0);
		if(dst!=null)
		{
			memcpy(dst,s.begin(),s.count());
		}
		return s.count();
	}

	static int utf8_to_utf16_c(uchar* src,uchar* dst)
	{
		rstr s=utf8_to_utf16(src);
		s.push(0);
		s.push(0);
		if(dst!=null)
		{
			memcpy(dst,s.begin(),s.count());
		}
		return s.count();
	}

	static int utf16_to_gbk_c(uchar* src,uchar* dst)
	{
		rstr s;
		for(uchar* p=src;;p+=2)
		{
			if(*p==0&&*(p+1)==0)
			{
				s.set_size(p-src);
				memcpy(s.begin(),src,s.count());
				break;
			}
		}
		s=utf16_to_gbk(s);
		s.push(0);
		if(dst!=null)
		{
			memcpy(dst,s.begin(),s.count());
		}
		return s.count();
	}

	static int utf16_to_utf8_c(uchar* src,uchar* dst)
	{
		rstr s;
		for(uchar* p=src;;p+=2)
		{
			if(*p==0&&*(p+1)==0)
			{
				s.set_size(p-src);
				memcpy(s.begin(),src,s.count());
				break;
			}
		}
		s=utf16_to_utf8(s);
		s.push(0);
		if(dst!=null)
		{
			memcpy(dst,s.begin(),s.count());
		}
		return s.count();
	}
};

#endif