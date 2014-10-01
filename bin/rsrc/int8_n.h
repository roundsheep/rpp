
class int8
{
	rd8 m_in;

	~int8()
	{
	}

	int8()
	{
	}

	friend int8 operator*(int8 a,int8 b)
	{
		mov eax,[ebp+(s_off b+4)]
		push eax
		mov ecx,b
		push ecx
		mov edx,[ebp+(s_off a+4)]
		push edx
		mov eax,a
		push eax
		rn call long_mul
		mov s_ret,eax
		mov [ebp+(s_off s_ret+4)],edx
		pop ebp
		reti 16
long_mul:
		mov eax,[esp+8]
		mov ecx,[esp+0x10]
		rn or ecx,eax  
		mov ecx,[esp+0xc]
		rn jne hard
		mov eax,[esp+4]
		rn db 0xf7,0xe1//mul eax,ecx
		reti 0x10
hard:
		push ebx
		rn db 0xf7,0xe1//mul eax,ecx
		mov ebx,eax
		mov eax,[esp+8]
		rn db 0xf7,0x64,0x24,0x14//mul eax,[esp+0x14]
		add ebx,eax
		mov eax,[esp+8]
		rn db 0xf7,0xe1//mul eax,ecx
		add edx,ebx
		pop ebx
		reti 0x10
	}

	friend int8 operator/(int8 a,int8 b)
	{
		mov eax,[ebp+(s_off b+4)]
		push eax
		mov ecx,b
		push ecx
		mov edx,[ebp+(s_off a+4)]
		push edx
		mov eax,a
		push eax
		rn call long_div
		mov s_ret,eax
		mov [ebp+(s_off s_ret+4)],edx
		pop ebp
		reti 16
long_div:
		push edi
		push esi
		push ebx
		rn xor edi,edi
		mov eax,[esp+0x14]
		rn or eax,eax 
		rn jge L1
		rn inc edi 
		mov edx,[esp+0x10] 
		rn neg eax 
		rn neg edx 
		rn sbb eax,0 
		mov [esp+0x14],eax 
		mov [esp+0x10],edx 
L1:
		mov eax,[esp+0x1c] 
		rn or eax,eax 
		rn jge L2
		rn inc edi 
		mov edx,[esp+0x18] 
		rn neg eax 
		rn neg edx 
		rn sbb eax,0 
		mov [esp+0x1c],eax 
		mov [esp+0x18],edx 
L2:
		rn or eax,eax 
		rn jne L3
		mov ecx,[esp+0x18] 
		mov eax,[esp+0x14] 
		rn xor edx,edx 
		rn db 0xf7,0xf1//div eax,ecx 
		mov ebx,eax 
		mov eax,[esp+0x10] 
		rn db 0xf7,0xf1//div eax,ecx 
		mov edx,ebx 
		jmp L4
L3:
		mov ebx,eax 
		mov ecx,[esp+0x18] 
		mov edx,[esp+0x14] 
		mov eax,[esp+0x10] 
L5:
		rn shr ebx,1 
		rn rcr ecx,1 
		rn shr edx,1 
		rn rcr eax,1 
		rn or ebx,ebx 
		rn jne L5
		rn db 0xf7,0xf1//div eax,ecx 
		mov esi,eax 
		rn db 0xf7,0x64,0x24,0x1c//mul eax,[esp+0x1c]
		mov ecx,eax 
		mov eax,[esp+0x18] 
		rn db 0xf7,0xe6//mul eax,esi 
		add edx,ecx 
		rn jb L6
		rn cmp edx,[esp+0x14] 
		rn ja L6
		rn jb L7
		rn cmp eax,[esp+0x10] 
		rn jbe L7
L6:
		rn dec esi 
L7:
		rn xor edx,edx 
		mov eax,esi 
L4:
		rn dec edi 
		rn jne L8
		rn neg edx 
		rn neg eax 
		rn sbb edx,0 
L8:
		pop ebx 
		pop esi
		pop edi
		reti 16  
	}
	
	friend bool operator<(int8 a,int8 b)
	{
		mov eax,[ebp+(s_off a+4)]
		rn cmp eax,[ebp+(s_off b+4)]
		rn jg tag_zero
		rn jl tag_one
		mov ecx,a 
		rn cmp ecx,b 
		rn jae tag_zero
tag_one:
		mov s_ret,1  
		return  
tag_zero:
		mov s_ret,0
	}

	int8(int8 a)
	{
		mov esi,this
		mov [esi],[ebp+s_off a]
		mov [esi+4],[ebp+(s_off a+4)]
	}

	int8(int8& a)
	{
		mov edi,this
		mov esi,a
		mov [edi],[esi]
		mov [edi+4],[esi+4]
	}

	int8(int a)
	{
		mov esi,this
		mov [esi],a
		mov [esi+4],0
		mov edi,a
		if(a<0)
		{
			mov esi,this
			mov [esi+4],uint::c_max
		}
	}

	int8(uint a)
	{
		mov esi,this
		mov [esi],a
		mov [esi+4],0
	}

	int toint()
	{
		mov esi,this
		mov s_ret,[esi]
	}

	rstr torstr()
	{
		rstr ret(this)
		return ret
	}

	operator=(int8 a)
	{
		mov esi,this
		mov [esi],[ebp+s_off a]
		mov [esi+4],[ebp+(s_off a+4)]
	}

	operator=(int8& a)
	{
		mov edi,this
		mov esi,a
		mov [edi],[esi]
		mov [edi+4],[esi+4]
	}

	friend operator<->(int8& a,int8& b)
	{
		c=a
		a=b
		b=c
	}

	operator+=(int8 a)
	{
		mov esi,this
		mov eax,[esi]
		add eax,a
		mov ecx,[esi+4]
		rn adc ecx,[ebp+(s_off a+4)]
		mov [esi],eax
		mov [esi+4],ecx
	}

	operator-=(int8 a)
	{
		mov esi,this
		mov eax,[esi]
		sub eax,a
		mov ecx,[esi+4]
		rn sbb ecx,[ebp+(s_off a+4)]
		mov [esi],eax
		mov [esi+4],ecx
	}

	operator*=(int8 a)
	{
		this=this*a
	}
	
	operator/=(int8 a)
	{
		this=this/a
	}
	
	operator%=(int8 a)
	{
		this=this%a
	}

	operator++()
	{
		this+=1
	}

	operator--()
	{
		this-=1
	}

	friend int8 operator+(int8 a,int8 b) 
	{
		a+=b
		return a
	}

	friend int8 operator-(int8 a,int8 b)
	{
		a-=b
		return a
	}
	
	//todo
	friend int8 operator%(int8 a,int8 b)
	{
		return a-a/b*b
	}

	friend bool operator<=(int8 a,int8 b)
	{
		return a<b||a==b;
	}

	friend bool operator==(int8 a,int8 b)
	{
		cesb a,b
		if(ebx)
		{
			cesb [ebp+(s_off a+4)],[ebp+(s_off b+4)]
		}
		mov s_ret,ebx
	}

	friend bool operator!=(int8 a,int8 b)
	{
		return !(a==b)
	}

	friend bool operator>(int8 a,int8 b)
	{
		return b<a;
	}

	friend bool operator>=(int8 a,int8 b)
	{
		return b<=a;
	}
	
	friend int8 operator neg(int8 a)
	{
		return 0-a;
	}

	clear()
	{
		this=0
	}

	bool empty()
	{
		return 0==this
	}

	print()
	{
		rf.print this
	}

	printl()
	{
		rf.printl this
	}
};