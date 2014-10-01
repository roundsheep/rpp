#ifndef rclass_h__
#define rclass_h__

#include "rbuf.h"

extern uchar g_unicode[];
extern uchar g_gbk[];

class rclass//本类用于初始化环境，只能实例化一次（单例模式）
{
public:
	rclass();
	~rclass();
};

#endif