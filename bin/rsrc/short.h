class short
{
	rd2 m_in

	~short()
	{
	}

	short()
	{
	}

	short(short& a)
	{
		mov esi,this
		mov edi,a
		mov1 [esi],[edi]
		mov1 [esi+1],[edi+1]
	}

	short(short a)
	{
		mov esi,this
		mov1 [esi],a
		mov1 [esi+1],[ebp+(s_off a+1)]
	}
	
	ushort toushort()
	{
		mov esi,this
		mov1 s_ret,[esi]
		mov1 [ebp+(s_off s_ret+1)],[esi+1]
	}

	int toint()
	{
		temp=this.toushort
		if temp>32767
			return temp.toint-65536
		else
			return temp
	}

	rstr torstr()
	{
		rstr ret(this.toint)
		return ret
	}

	operator=(short& a)
	{
		mov esi,this
		mov edi,a
		mov1 [esi],[edi]
		mov1 [esi+1],[edi+1]
	}

	operator=(short a)
	{
		mov esi,this
		mov1 [esi],a
		mov1 [esi+1],[ebp+(s_off a+1)]
	}

	clear()
	{
		this=0
	}

	bool empty()
	{
		return this.toint==0
	}

	print()
	{
		rf.print this.torstr
	}

	printl()
	{
		rf.printl this.torstr
	}
}