#include "rlib/rf.h"
#include "rlib/rclass.h"
#include "rpp/zmain.h"

int main(int argc, char* argv[])
{
	static tvm vm;
	return zmain::rpp_main(vm,argc,argv);
}