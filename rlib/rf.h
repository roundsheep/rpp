#ifndef rf_h__
#define rf_h__

#include "rstr.h"
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#ifdef _MSC_VER
#include <windows.h>
#include <conio.h>
#else
//#include <iconv.h>
#include <unistd.h>
#include <termios.h>
#endif
#include <stdlib.h>
#include <time.h>  
//#include <random>

struct rf
{
	enum
	{
		c_block_size=(64*1024)
	};

	static void error(rstr s=rstr())
	{
		printf("\n%s\n",s.cstr());
		getch();
		exit(1);
	}

	static void printl(rstr s=rstr())
	{
		printf("%s\n",s.cstr());
	}

	static void print(rstr s=rstr())
	{
		printf("%s ",s.cstr());
	}

	static void print(const char* s)
	{
		printf("%s ",s);//必须加上%s防止字符串中出现特殊字符
	}

	static int tick()
	{
#ifdef _MSC_VER
		return GetTickCount();
#else
		return (int)((double)clock()/CLOCKS_PER_SEC*1000);
#endif
	}

	static void sleep(int milliseconds)
	{
		if(milliseconds<50)
		{
			milliseconds=50;
		}
#ifdef _MSC_VER
		Sleep(milliseconds);
#else
		if(milliseconds>2100000000)
			::sleep(milliseconds/1000);
		else
			usleep(milliseconds*1000);
#endif
	}

	static int getch()
	{
#ifdef _MSC_VER
		return ::getch();
#else
		struct termios oldt,newt;
		int ch;
		tcgetattr(STDIN_FILENO,&oldt);
		newt=oldt;
		newt.c_lflag&=~(ICANON|ECHO);
		tcsetattr(STDIN_FILENO,TCSANOW,&newt);
		ch=getchar();
		tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
		return ch;
#endif
	}

	static int round(double a){
		return (int)(a+0.5);
	}

	/*static int rand_n(int n){
		random_device rd;
		return rd()%n;
	}*/

	static void srand()
	{
		::srand((uint)::time(null));
	}

	static int rand()
	{
		return ::rand();
	}

	static uchar get_bitmap(uchar bitmap[],int block)
	{
		uchar result;
		result=bitmap[block/8]>>(7-block%8);
		result&=0x1;
		return result;
	}

	static void set_bitmap(uchar bitmap[],int block)
	{
		bitmap[block/8]|=(1<<(7-block%8));
	}

	static rbool execmd(const char* p)
	{
		return 0==::system(p);
	}
#ifdef _MSC_VER
#else
	static rbool execmd_r(const char* cmd,rstr& result)
	{
		result=rstr();
		FILE* fp=popen(cmd,"r");

		if(null==fp)
		{
			return false;
		}

		rbuf<char> buf(4096+1);
		buf.m_p[4096]=0;
		int count;

		while(true)
		{
			count=fread(buf.m_p,1,4096,fp);

			if(count>0)
			{
				buf.m_p[count]=0;
				result+=buf.m_p;
			}

			if(count<4096)
			{
				break;
			}
		}

		pclose(fp);
		return true;
	}
#endif
};

#endif
