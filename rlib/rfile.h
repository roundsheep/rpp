#ifndef rfile_h__
#define rfile_h__

#include "rbuf.h"
#include "rstr.h"
#include "rf.h"
#include "rcode.h"
#include "rstrw.h"
#ifdef _MSC_VER
#else
//#define _FILE_OFFSET_BITS 64
//#include <sys/types.h>
#endif

struct rfile
{
	FILE* m_fp;
	
	~rfile()
	{
		close();
	}

	rfile()
	{
		m_fp=null;
	}

	rfile(rstrw name,rstrw mode="r")
	{
		open(name,mode);
	}

	rbool close()
	{
		if(null==m_fp)
		{
			return false;
		}
		if(0!=fclose(m_fp))
		{
			return false;
		}
		m_fp=null;
		return true;
	}
	
	//mode: rb只读，wb重新创建写,rb+读写
	//推荐用 r只读，w写，rw读写，本函数只有二进制模式
	rbool open(rstrw name,rstrw mode="r")
	{
		m_fp=null;
		if(!exist(name))
		{
			if(rstrw("rw")==mode)
			{
				if(!create(name))
				{
					return false;
				}
			}
			else
				return false;
		}
		if(rstrw("r")==mode)
		{
			mode=rstrw("rb");
		}
		elif(rstrw("w")==mode)
		{
			mode=rstrw("wb");
		}
		elif(rstrw("rw")==mode)
		{
			mode=rstrw("rb+");
		}
		m_fp=fopen_w(name.cstrw_t(),mode.cstrw_t());
		if(null==m_fp)
		{
			return false;
		}
		return true;
	}

	//强制打开一个新文件
	rbool open_n(rstrw name,rstrw mode="rw")
	{
		remove(name);
		return open(name,mode);
	}

	rbool read(int off,int len,void* buf)
	{
		return read((int8)off,len,buf);
	}

	rbool read(int8 off,int len,void* buf)
	{
		if(null==m_fp)
		{
			return false;
		}
		set_off8(off);
		return len==(int)fread(buf,1,len,m_fp);
	}

	rbool read(int len,void* buf)
	{
		if(null==m_fp)
		{
			return false;
		}
		return len==(int)fread(buf,1,len,m_fp);
	}
	
	//读取并返回整个文件，通常用于小文件
	rstr read_all()
	{
		rstr ret;
		ret.set_size(size());
		if(!read(0,size(),ret.begin()))
		{
			return rstr();
		}
		return r_move(ret);
	}

	//直接读一个小文件
	static rstr read_all_n(const rstrw& name)
	{
		rfile file(name);
		rstr ret;
		ret.set_size(file.size());
		if(!file.read(0,file.size(),ret.begin()))
		{
			return rstr();
		}
		return r_move(ret);
	}
	
	//读取一行（跳过空行）
	rstr read_line()
	{
		rstr ret;
		uchar c;
		while(read(1,&c))
		{
			if('\r'==c||'\n'==c)
			{
				if(0==ret.count())
				{
					continue;
				}
				else
				{
					return ret;
				}
			}
			ret+=c;
		}
		return r_move(ret);
	}
	
	//读取一行，仅用于utf16文件
	rstrw read_line_w()
	{
		rstrw ret;
		ushort c;
		while(read(2,&c))
		{
			if('\r'==c||'\n'==c)
			{
				if(0==ret.count())
				{
					continue;
				}
				else
				{
					return ret;
				}
			}
			ret+=c;
		}
		return r_move(ret);
	}

	rbool write(int off,int len,const void* buf)
	{
		return write((int8)off,len,buf);
	}

	rbool write(int8 off,int len,const void* buf)
	{
		if(null==m_fp)
		{
			return false;
		}
		set_off8(off);
		return len==(int)fwrite(buf,1,len,m_fp);
	}

	rbool write(int len,const void* buf)
	{
		if(null==m_fp)
		{
			return false;
		}
		return len==(int)fwrite(buf,1,len,m_fp);
	}

	rbool write(rstr s)
	{
		return write(s.size(),s.begin());
	}

	rbool write_line(rstr s)
	{
		return write(s.size(),s.begin())&&write(1,"\n");
	}

	rbool write_line_w(rstrw s)
	{
		return write(s.size(),s.begin())&&write(2,rstrw("\n").cstrw_t());
	}

	static rbool write_all_n(const rstrw& name,const rstr& cont)
	{
		rfile file;
		ifn(file.open_n(name))
		{
			return false;
		}
		return file.write(cont);
	}

	int size()
	{
		return (int)size8();
	}

	int8 size8()
	{
		if(null==m_fp)
		{
			return 0;//返回-1和返回0各有利弊
		}
		int8 cur=get_off8();
		fseek8(m_fp,0,SEEK_END);
		int8 ret=get_off8();
		set_off8(cur);
		return ret;
	}

	int get_off()
	{
		return (int)get_off8();
	}

	int8 get_off8()
	{
		if(null==m_fp)
		{
			return 0;
		}
		return ftell8(m_fp);
	}

	rbool set_off(int off)
	{
		return set_off8((int8)off);
	}

	rbool set_off8(int8 off)
	{
		return 0==fseek8(m_fp,off,SEEK_SET);
	}

	static rbool exist(rstrw name)
	{
		FILE* fp=fopen_w(name.cstrw_t(),rstrw("rb").cstrw_t());
		if(null==fp)
		{
			return false;
		}
		fclose(fp);
		return true;
	}

	//强制创建文件
	static rbool create(rstrw name,int8 size=0)
	{
		FILE* fp=fopen_w(name.cstrw_t(),rstrw("wb").cstrw_t());
		if(null==fp)
			return false;
		fclose(fp);
		if(size>0)
		{
			rfile file(name);
			file.set_off8(size-1);
			if(!file.write(1,"\0"))
			{
				return false;
			}
			return file.close();
		}
		return true;
	}

	static int8 ftell8(FILE* fp)
	{
#ifdef _MSC_VER
		return _ftelli64(fp);
#else
		return ftello(fp);
#endif
	}

	static int fseek8(FILE* fp,int8 off,int start)
	{
#ifdef _MSC_VER
		return _fseeki64(fp,off,start);
#else
		return fseeko(fp,off,start);
#endif
	}

	static FILE* fopen_w(wchar* name,wchar* mode)
	{
#ifdef _MSC_VER
		return _wfopen(name,mode);
#else
		return fopen(rstrw(name).torstr().cstr_t(),
			rstrw(mode).torstr().cstr_t());
#endif
	}

	static rbool remove(rstrw name)
	{
#ifdef _MSC_VER
		return 0==_wremove(name.cstrw_t());
#else
		return 0==::remove(name.torstr().cstr_t());
#endif	
	}

	static rbool rename(rstrw oldname,rstrw newname)
	{
#ifdef _MSC_VER
		return 0==::_wrename(oldname.cstrw_t(),newname.cstrw_t());
#else
		return 0==::rename(oldname.torstr().cstr_t(),
			newname.torstr().cstr_t());
#endif
	}
#ifdef _MSC_VER
	static int get_exe_size(char* name)
	{
		rstr temp=rfile::read_all_n(name);
		if(temp.empty())
		{
			return 0;
		}
		uchar* data=temp.begin();
		PIMAGE_DOS_HEADER dos_head=(PIMAGE_DOS_HEADER)data;
		PIMAGE_OPTIONAL_HEADER op_head=(PIMAGE_OPTIONAL_HEADER)(
			data+dos_head->e_lfanew+4+20);
		PIMAGE_FILE_HEADER file_head=(PIMAGE_FILE_HEADER)(
			data+dos_head->e_lfanew+4);
		PIMAGE_SECTION_HEADER sec=(PIMAGE_SECTION_HEADER)(
			data+dos_head->e_lfanew+sizeof(IMAGE_NT_HEADERS));
		int size=op_head->SizeOfHeaders;
		for(int i=0;i<file_head->NumberOfSections;i++)
		{
			size+=sec[i].SizeOfRawData;
		}
		return size;
	}
#endif
	static int8 get_update_time(rstrw path)
	{
#ifdef _MSC_VER
		int8 ft;
		HANDLE hFile = CreateFileW(path.cstrw_t(),
			GENERIC_READ,
			FILE_SHARE_READ,
			null,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			null);
		if(hFile == INVALID_HANDLE_VALUE)
			return 0;
		if(!GetFileTime(hFile,null,null,(FILETIME*)&ft))
			return 0;
		CloseHandle(hFile);
		return ft;
#else
		return 0;
		/*struct stat buf;
		int ret=stat(path,&buf);
		if(0!=ret)
		{
			return false;
		}
		*ft=(int64)buf.st_mtime;
		return true;*/
#endif
	}

	static int8 get_size8(rstrw name)
	{
		rfile file(name);
		return file.size8();
	}
};

#endif
