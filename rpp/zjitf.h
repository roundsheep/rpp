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

	static void sqrt(double a,double* b)
	{
		*b=::sqrt(a);
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

	static void* find_dll_q(const char* name)
	{
		tsh& sh=*get_psh();
		if(sh.m_dll_func.exist(name))
		{
			return sh.m_dll_func[name];
		}
		if(sh.m_jit_f.exist(name))
		{
			return sh.m_jit_f[name];
		}
		return find_dll_full(name);
	}

	static void* find_dll_full(const char* name)
	{
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
	}

	//非多线程安全
	static void set_dll_list()
	{
		tsh& sh=*get_psh();
		PBYTE pb=null;
		MEMORY_BASIC_INFORMATION mbi;
		while(VirtualQuery(pb,&mbi,r_size(mbi))==r_size(mbi))
		{
			if(mbi.State==MEM_FREE)
				mbi.AllocationBase=mbi.BaseAddress;
			if(mbi.AllocationBase==mbi.BaseAddress&&
				mbi.AllocationBase!=null)
			{
				//todo 去掉GetProcAddress
				void* ret=GetProcAddress(
					(HINSTANCE)mbi.AllocationBase,"glutSpecialFunc");
				if(ret!=null)
				{
					set_dll_list_one(
						sh,(HINSTANCE)mbi.AllocationBase);
				}
				ret=GetProcAddress(
					(HINSTANCE)mbi.AllocationBase,"glBegin");
				if(ret!=null)
				{
					set_dll_list_one(
						sh,(HINSTANCE)mbi.AllocationBase);
				}
			}
			pb+=mbi.RegionSize;
		}
	}

	static void set_dll_list_one(tsh& sh,HINSTANCE hModule)
	{
		char *pRet=null;
		PIMAGE_DOS_HEADER pImageDosHeader=NULL;
		PIMAGE_NT_HEADERS pImageNtHeader=NULL;
		PIMAGE_EXPORT_DIRECTORY pImageExportDirectory=null;

		pImageDosHeader=(PIMAGE_DOS_HEADER)hModule;
		pImageNtHeader=(PIMAGE_NT_HEADERS)((
			DWORD)hModule+pImageDosHeader->e_lfanew);
		pImageExportDirectory=(PIMAGE_EXPORT_DIRECTORY)(
			(DWORD)hModule+pImageNtHeader->OptionalHeader.DataDirectory[
				IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

		DWORD dwExportRVA=pImageNtHeader->OptionalHeader.DataDirectory[
			IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		DWORD dwExportSize=pImageNtHeader->OptionalHeader.DataDirectory[
			IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

		DWORD* pAddressOfFunction=(DWORD*)(
			pImageExportDirectory->AddressOfFunctions+(DWORD)hModule);
		DWORD* pAddressOfNames=(DWORD*)(
			pImageExportDirectory->AddressOfNames+(DWORD)hModule);
		DWORD dwNumberOfNames=(DWORD)(
			pImageExportDirectory->NumberOfNames);
		DWORD dwBase=(DWORD)(pImageExportDirectory->Base);

		WORD* pAddressOfNameOrdinals=(WORD*)(
			pImageExportDirectory->AddressOfNameOrdinals+(DWORD)hModule);

		for(int i=0;i<(int)dwNumberOfNames;i++)
		{
			char *strFunction=(char*)(pAddressOfNames[i]+(DWORD)hModule);
			if(*strFunction==0)
			{
				continue;
			}
			pRet=(char*)(pAddressOfFunction[
				pAddressOfNameOrdinals[i]]+(DWORD)hModule);
			if((DWORD)pRet<dwExportRVA+(DWORD)hModule||
				(DWORD)pRet>dwExportRVA+(DWORD)hModule+dwExportSize)
			{
				sh.m_dll_func[strFunction]=(void*)pRet;
			}
		}
	}
};

#endif
