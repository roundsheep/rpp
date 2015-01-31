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

	static void _float_to_double(float a,double* b)
	{
		*b=a;
	}

	//当返回double时X86的ABI是通过xmm返回
	static void sin(double a,double* b)
	{
		*b=::sin(a);
	}

	static void cos(double a,double* b)
	{
		*b=::cos(a);
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

	static tfunc* get_cur_func(uchar* pasm)
	{
		taddr* p=get_psh()->m_addr.find(taddr((uint)pasm,(uint)pasm+1,null));
		if(p==null)
		{
			return null;
		}
		return p->ptfi;
	}

	static rset<tclass>* get_vclass()
	{
		return &get_psh()->m_class;
	}

	//fixme _ftelli64必须要加载msvcr110.dll才能使用
	static void ftell8(int8* ret,FILE* fp)
	{
		*ret=rfile::ftell8(fp);
	}

	//todo 非多线程安全
	static void* find_dll_q(const char* name)
	{
		tsh& sh=*get_psh();
		if(sh.m_dll_func.exist(name))
		{
			return sh.m_dll_func[name];
		}
		void* temp=find_dll_full(name);
		if(temp!=null)
		{
			sh.m_dll_func[name]=temp;
		}
		return temp;
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
