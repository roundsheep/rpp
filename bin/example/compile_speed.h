//1.h amd 3.45GHZ cost time 3600ms ,new version 2800ms
//36_2.h cost time 5375ms (1.83 4985ms),new version 4438ms
main
{
	start=rf.tick
	for i=1 to 10
		rf.cmd("rpp example\\36_2.h")
	putsl
	putsl(rf.tick-start)
}