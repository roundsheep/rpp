#ifndef rpp_def_h__
#define rpp_def_h__

//#define RANALYSE
#define c_rpp_deep 150
#define c_point_size 4

#define rppkey(a) (sh.m_key[tkey::a])
#define rppoptr(a) (sh.m_optr[toptr::a])
#define rppconf(a) (sh.m_conf[tconf::a])

#define v_next_ins_js cur=(tins*)((uchar*)cur+r_size(tasm));goto next
#define v_next_ins reg.eip+=r_size(tasm);goto next

#define v_ato_int(a) (*r_to_pint(&(a)))
#define v_pto_int(a) (*r_to_pint(a))
#define v_ato_uint(a) (*r_to_puint(&(a)))
#define v_pto_uint(a) (*r_to_puint(a))
#define v_ato_char(a) (*r_to_pchar(&(a)))
#define v_pto_char(a) (*r_to_pchar(a))
#define v_pto_int8(a) (*r_to_pint8(a))
#define v_pto_f8(a) (*r_to_pdouble(a))
#define v_pto_rstr(a) (*r_to_prstr(a))

#define v_pto_pchar(a) ((char*)v_pto_int(a))
#define v_pto_pvoid(a) ((void*)v_pto_int(a))

#define v_get_imme_u(a) (r_to_uint((a).val))
#define v_get_reg_u(a) v_pto_uint(r_to_puchar(&reg)+(a).off)
#define v_get_addr_u(a) v_pto_uint(v_get_reg(a)+(a).val)

#define v_get_imme(a) ((a).val)
#define v_get_reg(a) v_pto_int(r_to_puchar(&reg)+(a).off)
#define v_get_addr(a) v_pto_int(v_get_reg(a)+(a).val)
#define v_get_lea(a) (v_get_reg(a)+v_get_imme(a))
#define v_get_addr_1(a) v_pto_char(v_get_reg(a)+(a).val)
#define v_get_addr_8(a) v_pto_int8(v_get_reg(a)+(a).val)
#define v_get_addr_f8(a) v_pto_f8(v_get_reg(a)+(a).val)

#define rppja znasm::get_opnd1_v(vstr)
#define rppjb znasm::get_opnd2_v(vstr)
#define rppj4(a,b,c,d) (zjiti::a(build_ins(sh,"nop",b,c,d)))
#define rppjf(a,b) sh.m_jit_f[a]=&b

#define rppjcmp(a)                   \
rstr s;                              \
if(znasm::count_mbk_l(vstr)==2)      \
{                                    \
	s+=rppj4(b_mov,"ecx",",",rppjb); \
	s+=rppj4(b_cmp,rppja,",","ecx"); \
}                                    \
else                                 \
{                                    \
	s+=zjiti::b_cmp(ins);            \
}                                    \
s+=rppj4(b_mov,"ebx",",","0");       \
s+=zjiti::a();                       \
return s

#define rppncmp(a)                                       \
if(count_mbk_l(vstr)==2)                                 \
{                                                        \
	result+="	mov ecx , "+get_opnd2(vstr)+"\n";        \
	result+="	cmp "+get_opnd1(vstr)+" , ecx\n";        \
}                                                        \
else                                                     \
{                                                        \
	result+="	cmp dword "+link_vstr(vstr.sub(1))+"\n"; \
}                                                        \
result+="	mov ebx , 0\n";                              \
result+=a;                                               \
return true

#endif
