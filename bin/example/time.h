main
{
	s=getsl
	start=rf.tick
	rf.cmd(s)
	putsl
	putsl(rf.tick-start)
}