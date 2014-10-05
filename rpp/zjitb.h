#ifndef zjitb_h__
#define zjitb_h__

#include "znasm.h"

//jit基础类
struct zjitb
{
	static rstr build_one(int one)
	{
		rstr s;
		s.set_size(1);
		s[0]=one;
		return r_move(s);
	}

	static rstr build_two(int one,int two)
	{
		rstr s;
		s.set_size(2);
		s[0]=one;
		s[1]=two;
		return r_move(s);
	}

	static rstr build_three(int one,int two,int three)
	{
		rstr s;
		s.set_size(3);
		s[0]=one;
		s[1]=two;
		s[2]=three;
		return r_move(s);
	}

	static rstr build_i(const tins& ins,int one)
	{
		rstr s;
		s.set_size(5);
		s[0]=one;
		*(int*)&s[1]=ins.first.val;
		return r_move(s);
	}

	static rstr build_r(const tins& ins,int one)
	{
		rstr s;
		s.set_size(1);
		s[0]=one;
		set_reg_bit(ins.first.off,&s[0]);
		return r_move(s);
	}

	static rstr build_r(const tins& ins,int one,int two)
	{
		rstr s;
		s.set_size(2);
		s[0]=one;
		s[1]=two;
		set_reg_bit(ins.first.off,&s[1]);
		return r_move(s);
	}

	static rstr build_a(const tins& ins,int one,int two)
	{
		rstr s;
		if(ins.first.off==treg::c_esp)
		{
			s.set_size(7);
		}
		else
		{
			s.set_size(6);
		}
		s[0]=one;
		s[1]=two;
		set_addr_bit(ins.first.off,&s[1],ins.first.val);
		return r_move(s);
	}

	static rstr build_relative(const tins& ins,uchar* start,int one)
	{
		rstr s;
		s.set_size(5);
		s[0]=one;
		*(int*)&s[1]=ins.first.val-((int)start+s.count());
		return r_move(s);
	}

	static rstr build_relative(const tins& ins,uchar* start,int one,int two)
	{
		rstr s;
		s.set_size(6);
		s[0]=one;
		s[1]=two;
		*(int*)&s[2]=ins.first.val-((int)start+s.count());
		return r_move(s);
	}

	static rstr build_ri(const tins& ins,int one)
	{
		rstr s;
		s.set_size(5);
		s[0]=one;
		set_reg_bit(ins.first.off,&s[0]);
		*(int*)&s[1]=ins.second.val;
		return r_move(s);
	}

	static rstr build_ri(const tins& ins,int one,int two)
	{
		rstr s;
		s.set_size(6);
		s[0]=one;
		s[1]=two;
		set_reg_bit(ins.first.off,&s[1]);
		*(int*)&s[2]=ins.second.val;
		return r_move(s);
	}

	static rstr build_rr(const tins& ins,int one,int two)
	{
		rstr s;
		s.set_size(2);
		s[0]=one;
		s[1]=two;
		set_reg_bit_center(ins.first.off,&s[1]);
		set_reg_bit(ins.second.off,&s[1]);
		return r_move(s);
	}

	static rstr build_ra(const tins& ins,int one,int two)
	{
		rstr s;
		if(ins.second.off==treg::c_esp)
		{
			s.set_size(7);
		}
		else
		{
			s.set_size(6);
		}
		s[0]=one;
		s[1]=two;
		set_reg_bit_center(ins.first.off,&s[1]);
		set_addr_bit(ins.second.off,&s[1],ins.second.val);
		return r_move(s);
	}

	static rstr build_ai(const tins& ins,int one,int two)
	{
		rstr s;
		if(ins.first.off==treg::c_esp)
		{
			s.set_size(11);
			*(int*)&s[7]=ins.second.val;
		}
		else
		{
			s.set_size(10);
			*(int*)&s[6]=ins.second.val;
		}
		s[0]=one;
		s[1]=two;
		set_addr_bit(ins.first.off,&s[1],ins.first.val);
		return r_move(s);
	}

	static rstr build_ar(const tins& ins,int one,int two)
	{
		rstr s;
		if(ins.first.off==treg::c_esp)
		{
			s.set_size(7);
		}
		else
		{
			s.set_size(6);
		}
		s[0]=one;
		s[1]=two;
		set_addr_bit(ins.first.off,&s[1],ins.first.val);
		set_reg_bit_center(ins.second.off,&s[1]);
		return r_move(s);
	}

	static rstr build_ra(const tins& ins,int one,int two,int three)
	{
		rstr s;
		if(ins.second.off==treg::c_esp)
		{
			s.set_size(8);
		}
		else
		{
			s.set_size(7);
		}
		s[0]=one;
		s[1]=two;
		s[2]=three;
		set_reg_bit_center(ins.first.off,&s[2]);
		set_addr_bit(ins.second.off,&s[2],ins.second.val);
		return r_move(s);
	}

	//设置寄存器相对寻址位
	static void set_addr_bit(int off,uchar* start,int val)
	{
		if(off==treg::c_esp)
		{
			*start|=0x4;
			*(start+1)=0x24;
			*(int*)(start+2)=val;
			return;
		}
		*(int*)(start+1)=val;
		if(off==treg::c_ebp)
		{
			*start|=0x5;
		}
		elif(off==treg::c_esi)
		{
			*start|=0x6;
		}
		elif(off==treg::c_edi)
		{
			*start|=0x7;
		}
		elif(off==treg::c_eax)
		{
			*start|=0x0;
		}
		elif(off==treg::c_ebx)
		{
			*start|=0x3;
		}
		elif(off==treg::c_ecx)
		{
			*start|=0x1;
		}
		elif(off==treg::c_edx)
		{
			*start|=0x2;
		}
	}

	static void set_reg_bit(int off,uchar* start)
	{
		if(off==treg::c_esp)
		{
			*start|=0x4;
		}
		elif(off==treg::c_ebp)
		{
			*start|=0x5;
		}
		elif(off==treg::c_esi)
		{
			*start|=0x6;
		}
		elif(off==treg::c_edi)
		{
			*start|=0x7;
		}
		elif(off==treg::c_eax)
		{
			*start|=0x0;
		}
		elif(off==treg::c_ebx)
		{
			*start|=0x3;
		}
		elif(off==treg::c_ecx)
		{
			*start|=0x1;
		}
		elif(off==treg::c_edx)
		{
			*start|=0x2;
		}
	}

	static void set_reg_bit_center(int off,uchar* start)
	{
		if(off==treg::c_esp)
		{
			*start|=0x4<<3;
		}
		elif(off==treg::c_ebp)
		{
			*start|=0x5<<3;
		}
		elif(off==treg::c_esi)
		{
			*start|=0x6<<3;
		}
		elif(off==treg::c_edi)
		{
			*start|=0x7<<3;
		}
		elif(off==treg::c_eax)
		{
			*start|=0x0<<3;
		}
		elif(off==treg::c_ebx)
		{
			*start|=0x3<<3;
		}
		elif(off==treg::c_ecx)
		{
			*start|=0x1<<3;
		}
		elif(off==treg::c_edx)
		{
			*start|=0x2<<3;
		}
	}
};

#endif 
