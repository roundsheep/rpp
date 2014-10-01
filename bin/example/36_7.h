/*
read self
*/

import "rfile.h"

main
{
	rfile file("example/36_7.h")
	file.read_all.print
	//auto close file in destructor
}