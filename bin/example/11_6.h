
void main()
{	
	==> 2 3 <==
	
	┏━━━━━┓
	┃ 1     2  ┃
	┃ 3     4  ┃  
	┗━━━━━┛	
}

mac$ ==> _word _word <==
{
	putsl(rstr($0)+rstr($1))
}

mac$ ┏━━━━━┓┃ _word _word ┃ ┃ _word _word ┃┗━━━━━┛
{
	putsl(rstr($0)+'--'+rstr($1)+'--'+rstr($2)+'--'+rstr($3))
}
