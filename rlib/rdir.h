#ifndef rdir_h__
#define rdir_h__

#include "rfile.h"
#include "rf.h"
#include "rstrw.h"
#include "rlist.h"

#ifdef _MSC_VER
#include <windows.h>
#include <io.h>
#else
//#include <dirent.h>  
//#include <sys/stat.h>  
//#include <unistd.h>  
//#include <sys/types.h> 
#endif

struct rdir_item
{
	rstrw path;
	rbool is_dir;

	rdir_item()
	{
		is_dir=false;
	}

	friend rbool operator<=(const rdir_item& a,const rdir_item& b)
	{
		return a.path<=b.path;
	}
};

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

struct rdir
{
	static rstrw erase_suffix(const rstrw& name)
	{
		rstrw s=get_suffix(name);
		return name.sub(0,name.count()-s.count()-1);
	}

	static rstrw get_suffix(const rstrw& name)
	{
		ushort ch='.';
		int pos=name.find_last(ch);
		if(pos<name.count())
			return name.sub(pos+1);
		return rstrw();
	}
	
	static rstrw get_prev_dir(rstrw path)
	{
		if(path.empty())
			return rstrw();
		path.pop();
		ushort ch='/';
		int pos=path.find_last(ch);
		if(pos>=path.count())
			return rstrw();
		return path.sub(0,pos+1);
	}

	static rstrw get_name(const rstrw& path)
	{
		if(path.empty())
			return rstrw();
		ushort ch='/';
		int pos=path.find_last(ch);
		if(pos>=path.count())
			return rstrw();
		return path.sub(pos+1);
	}

	static rstrw get_exe_name()
	{
		wchar buf[MAX_PATH+1];
		if(GetModuleFileNameW(null,(wchar*)buf,MAX_PATH)==0)
			return rstrw();
		return dir_std(rstrw(buf));
	}

	static rstrw get_exe_dir()
	{
		rstrw ret;
#ifndef __COCOS2D_H__
		wchar buf[MAX_PATH+1];
		if(GetModuleFileNameW(null,(wchar*)buf,MAX_PATH)==0)
			return ret;
		ret=buf;
#else
		ret=rstr(FileUtils::getInstance()->fullPathForFilename(
			"rinf/key.txt").c_str());
		ret=ret.sub(0,ret.count()-rstr("rinf/key.txt").count()+1);
#endif
		ret=dir_std(ret);
		ret=get_prev_dir(ret);
		return ret;
	}

	static rstrw get_cur_dir()
	{
		rstrw ret;
#ifndef __COCOS2D_H__
		wchar buf[MAX_PATH+1];
		if(GetCurrentDirectoryW(MAX_PATH,(wchar*)buf)==0)
			return ret;
		ret=buf;
		ret=dir_std(ret);
		ret+='/';
#else
		ret=rstr(FileUtils::getInstance()->fullPathForFilename(
			"rinf/key.txt").c_str());
		ret=ret.sub(0,ret.count()-rstr("rinf/key.txt").count()+1);
		ret=dir_std(ret);
		ret=get_prev_dir(ret);
#endif
		return ret;
	}
	
	//非win系统区分大小写
	static rstrw dir_std(rstrw s)
	{
		for(ushort* p=s.begin();p!=s.end();++p)
		{
			if('\\'==*p)
			{
				*p='/';
			}
		}
		return s;
	}

	static rbuf<rdir_item> get_file_bfs(rstrw path)
	{
		rbuf<rdir_item> ret;
#ifndef __COCOS2D_H__
		if(path.count()>4096||0==path.count())//ubuntu's max path len is 4096
		{
			return ret;
		}
		path=dir_std(path);
		if(path.count()>=1&&path[path.count()-1]!='/')
		{
			path+=rstr("/");
		}
		rlist<rstrw> queue;
		queue.push(path);
		while(!queue.empty())
		{
			path=queue.pop_front();
			WIN32_FIND_DATAW wfd; 
			HANDLE handle=FindFirstFileW(
				(path+rstr("*.*")).cstrw_t(),&wfd); 
			if(handle==INVALID_HANDLE_VALUE)
				continue;
			while(true)
			{
				rdir_item item;
				rstrw name=dir_std(wfd.cFileName);
				if(rstr(".")!=name&&rstr("..")!=name)
				{
					item.path=path+name;
					if(0!=(wfd.dwFileAttributes&
						FILE_ATTRIBUTE_DIRECTORY))
					{
						item.is_dir=true;
						queue.push(item.path+rstr("/"));
					}
					ret.push(item);
				}
				ifn(FindNextFileW(handle,&wfd))
				{
					break;
				}
			}
			FindClose(handle);
		}
#endif
		return ret;
	}
};

#endif
