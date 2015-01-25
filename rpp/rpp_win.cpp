#include "zmain.h"

//#define RPP_SHOW_CONSOLE

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine,int nCmdShow)
{
	rbuf<rstr> v=zmain::param_split(lpCmdLine);
	v.push_front(rcode::utf8_to_gbk(rdir::get_exe_name().torstr()));
	int argc=v.count();
	if(argc>c_rpp_deep)
	{
		return 1;
	}
	char* argv[c_rpp_deep];
	for(int i=0;i<argc;i++)
	{
		argv[i]=v[i].cstr_t();
	}
	static tvm vm;
	vm.m_sh.m_hins=hInstance;
#ifdef RPP_SHOW_CONSOLE
	AllocConsole();
	freopen("CONIN$","r",stdin);
	freopen("CONOUT$","w",stdout);
	freopen("CONOUT$","w",stderr);
#endif
	int ret=zmain::rpp_main(vm,argc,argv);
#ifdef RPP_SHOW_CONSOLE
	FreeConsole();
#endif
	return ret;
}