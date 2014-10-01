main
{
	stdcall["MessageBoxA",0,"abc","123",0]
	stdcall["MessageBoxW",0,utf16c("abc"),utf16c("123"),0]
	@MessageBoxA(0,"aaa","bbb",0)
	
#ifdef _JIT
	void[rf.find_dll("MessageBoxA"),0,"111","222",0]
#endif
}