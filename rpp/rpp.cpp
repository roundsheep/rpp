#include "zmain.h"

int main(int argc, char* argv[])
{
	static tvm vm;
	return zmain::rpp_main(vm,argc,argv);
}