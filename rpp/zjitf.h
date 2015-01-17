#ifndef zjitf_h__
#define zjitf_h__

#include "znasm.h"

//jit函数
struct zjitf
{
	static tsh*& get_psh()
	{
		static tsh* g_psh;
		return g_psh;
	}

	static void add8(int8* a,int8* b)
	{
		*a+=*b;
	}

	static void sub8(int8* a,int8* b)
	{
		*a-=*b;
	}

	static void imul8(int8* a,int8* b)
	{
		*a*=*b;
	}

	static void idiv8(int8* a,int8* b)
	{
		*a/=*b;
	}

	static void imod8(int8* a,int8* b)
	{
		*a%=*b;
	}

	static rbool cgsb8(int8* a,int8* b)
	{
		return *a>*b;
	}

	static rbool clsb8(int8* a,int8* b)
	{
		return *a<*b;
	}

	static void fadd8(double* a,double* b)
	{
		*a+=*b;
	}

	static void fsub8(double* a,double* b)
	{
		*a-=*b;
	}

	static void fmul8(double* a,double* b)
	{
		*a*=*b;
	}

	static void fdiv8(double* a,double* b)
	{
		*a/=*b;
	}

	static rbool fcgsb8(double* a,double* b)
	{
		return *a>*b;
	}

	static rbool fclsb8(double* a,double* b)
	{
		return *a<*b;
	}

	static uint bshl(uint a,uint b)
	{
		return a<<b; 
	}

	static uint bshr(uint a,uint b)
	{
		return a>>b; 
	}

	static int bsar(int a,int b)
	{
		return a>>b; 
	}

	static int get_argc()
	{
		return get_psh()->m_argc;
	}

	static char** get_argv()
	{
		return get_psh()->m_argv;
	}

	static void* get_hins()
	{
		return get_psh()->m_hins;
	}

	static void set_main_ret(int a)
	{
		get_psh()->m_ret=a;
	}

	//fixme
	static void ftell8(int8* ret,FILE* fp)
	{
		*ret=rfile::ftell8(fp);
	}

	static void* find_dll_full(const char* name)
	{
#ifdef _MSC_VER
		PBYTE pb=null;
		MEMORY_BASIC_INFORMATION mbi;
		while(VirtualQuery(pb,&mbi,r_size(mbi))==r_size(mbi))
		{
			if(mbi.State==MEM_FREE)
				mbi.AllocationBase=mbi.BaseAddress;
			if(mbi.AllocationBase==mbi.BaseAddress&&
				mbi.AllocationBase!=null) 
			{
				void* ret=GetProcAddress(
					(HINSTANCE)mbi.AllocationBase,name);
				if(ret!=null)
				{
					return ret;
				}
			} 
			pb+=mbi.RegionSize;
		}
		return null;
#else
		return null;
#endif
	}
};

#endif
