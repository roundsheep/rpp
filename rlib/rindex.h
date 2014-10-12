#ifndef rindex_h__
#define rindex_h__

#include "rlib/rdb.h"
#include "rlib/rdir.h"

struct rindex
{
	static void run(int argc, char* argv[])
	{
		if (argc==3)
		{
			if (rstr(argv[2]).get_top()=='\\')
			{
				if (!build(rcode::gbk_to_utf8(argv[1]),rcode::gbk_to_utf8(argv[2])))
					rf::error("build error");
			}
			else
			{
				search(rcode::gbk_to_utf8(argv[1]),rcode::gbk_to_utf8(argv[2]));
			}
		}
		elif (argc==2)
		{
			if (!check(rcode::gbk_to_utf8(argv[1])))
			{
				rf::error("check error");
			}
		}
		else
		{
			rf::error("miss path");
		}
	}

	static void search(rstrw name,rstr s)
	{
		rdbint db(name);
		for (int i=0;i<db.count();i+=3)
		{
			if (db[i].exist(s))
			{
				rcode::utf8_to_gbk(db[i]).printl();
				db[i+1].printl();
				db[i+2].printl();
			}
		}
	}

	static rbool check(rstrw name)
	{
		rdbint db(name);
		for (int i=0;i<db.count();i+=3)
		{
			rcode::utf8_to_gbk(db[i]).printl();
			db[i+1].printl();
			db[i+2].printl();
		}
		return true;
	}

	static rbool build(rstrw filename,rstrw path)
	{
		rbuf<rdir_item> list=rdir::get_file_bfs(path);
		r_qsort(list);
		rdbint db(filename,rstrw("rw"));
		for (int i=0;i<list.count();i++)
		{
			if (!db.write_new(list[i].path.torstr()))
			{
				return false;
			}
			if (!db.write_new(rfile::get_update_time(list[i].path)))
			{
				return false;
			}
			if (!db.write_new(rfile::get_size8(list[i].path)))
			{
				return false;
			}
			rcode::utf8_to_gbk(list[i].path.torstr()).printl();
		}
		return true;
	}
};

#endif
