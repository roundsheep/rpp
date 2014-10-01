
void main()
{
	B 1 2
	C 1 [2 *3]
	C 1 [2*3*4]
	D 1 [2 3]
	
	v=array<int>[7,8,9]
	v.join('').printl
	
	v=E(1,2,3)
	v.join('').printl
	
	v=E(1,2,3,4)
	v.join('').printl
}

mac$ B _word _word
{
	rf.printl($0##$1)
}

mac$ C _word [ _mword ]
{
	putsl($0+$1)
}

mac$ D _word [ _mword ]
{
	putsl($0+$1=>0+$1=>1)
}

mac$ E ( _mword )
{
	array<int>[$0]
}