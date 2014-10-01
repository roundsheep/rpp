%ifndef __NASMX_DEMOS_INC__
%define __NASMX_DEMOS_INC__

;// nasmx.inc should always be the first file included.
;// If you get the following nasm error:
;//     fatal: unable to open include file 'nasmx.inc'
;// then you need to specify the full directory path to the NASMX
;// include directory using either the NASMENV environment variable or the
;// -I include switch from the command line.
;// For reference see the setpaths.bat file in the NASMX root directory.
;//////////////////////////////////////////////////////////////
;//
;// NASMX Preprocessor
;// Copyright (c) 2005-2013, The NASMX Project
;//
%ifndef __NASMX_INCLUDED__
%define __NASMX_INCLUDED__

;//////////////////////////////////////////////////////////////
;//
;// Check NASM Version
;//
%define __nxerrmsg "NASM v2.09.08 or later required"
%ifdef __NASM_VERSION_ID__
	%if __NASM_VERSION_ID__ < 0x02090800
		%fatal __nxerrmsg
	%endif
%else
	%fatal __nxerrmsg
%endif
%undef __nxerrmsg

;//////////////////////////////////////////////////////////////
;//
;// Set a NASMX Definition
;//
%macro NASMX_DEFINE 1-2
	%if %0 > 1
		%ixdefine __NASMX_%[%1]__ %[2]
	%else
		%ixdefine __NASMX_%[%1]__
	%endif
%endmacro


;//////////////////////////////////////////////////////////////
;//
;// Get a NASMX Definition
;//
%macro NASMX_GET_DEFINE 2
	%ifdef __NASMX_%[%1]__
		%xdefine %[%2] __NASMX_%[%1]__
	%elifdef %2
		%undef %[%2]
	%endif
%endmacro

;//////////////////////////////////////////////////////////////
;//
;// Undefine a NASMX Definition
;//
%macro NASMX_UNDEF 1
	%ifdef __NASMX_%[%1]__
		%undef __NASMX_%[%1]__
	%else
		%warning NASMX Definition %[1] is not defined
	%endif
%endmacro


;//////////////////////////////////////////////////////////////
;//
;// Define NASMX Version
;//
NASMX_DEFINE VERSION,"1.2"
NASMX_DEFINE VERSION_MAJOR, 1
NASMX_DEFINE VERSION_MINOR, 2
NASMX_DEFINE VERSION_REV, 12
NASMX_DEFINE VERSION_ID, 0x0102000C


;//////////////////////////////////////////////////////////////
;//
;// init NASMX context stack
;//
;%push __NASMX_CTX__


;//////////////////////////////////////////////////////////////
;//
;// Set a Context Stack Value
;//
%macro NASMX_SET_CONTEXT_VALUE 2
	%xdefine %$$%1 %2
%endmacro


;//////////////////////////////////////////////////////////////
;//
;// native data sizes
;// Must be defined here due to struc_size!
%ixdefine byte_size    1
%ixdefine word_size    2
%ixdefine dword_size   4
%ixdefine qword_size   8
%ixdefine tword_size  10
%ixdefine oword_size  16
%ixdefine __m64_size   8
%ixdefine __m128_size 16
%ixdefine __m256_size 32
%ixdefine __int64_size   8
%ixdefine __int128_size 16
%ixdefine __nxfloat_size     4
%ixdefine __nxdouble_size    8
%ixdefine __nxfloat32_size   4
%ixdefine __nxfloat64_size   8
%ixdefine __nxfloat80_size  10
%ixdefine __nxfloat128_size 16
;// native register sizes
%idefine al_size  1
%idefine bl_size  1
%idefine cl_size  1
%idefine dl_size  1
%idefine ah_size  1
%idefine bh_size  1
%idefine ch_size  1
%idefine dh_size  1
%idefine ax_size  2
%idefine bx_size  2
%idefine cx_size  2
%idefine dx_size  2
%idefine di_size  2
%idefine si_size  2
%idefine sp_size  2
%idefine bp_size  2
%idefine bpl_size 1
%idefine dil_size 1
%idefine sil_size 1
%idefine spl_size 1
%idefine eax_size 4
%idefine ebx_size 4
%idefine ecx_size 4
%idefine edx_size 4
%idefine edi_size 4
%idefine esi_size 4
%idefine esp_size 4
%idefine ebp_size 4
%idefine rax_size 8
%idefine rbx_size 8
%idefine rcx_size 8
%idefine rdx_size 8
%idefine rdi_size 8
%idefine rsi_size 8
%idefine rsp_size 8
%idefine rbp_size 8
%idefine r8d_size  4
%idefine r9d_size  4
%idefine r10d_size 4
%idefine r11d_size 4
%idefine r12d_size 4
%idefine r13d_size 4
%idefine r14d_size 4
%idefine r15d_size 4
%idefine r8_size  8
%idefine r9_size  8
%idefine r10_size 8
%idefine r11_size 8
%idefine r12_size 8
%idefine r13_size 8
%idefine r14_size 8
%idefine r15_size 8

;// register aliases used to help with portability
%ifidn __BITS__,64
%ixdefine __AX rax
%ixdefine __BX rbx
%ixdefine __CX rcx
%ixdefine __DX rdx
%ixdefine __DI rdi
%ixdefine __SI rsi
%ixdefine __BP rbp
%ixdefine __SP rsp
%else
%ixdefine __AX eax
%ixdefine __BX ebx
%ixdefine __CX ecx
%ixdefine __DX edx
%ixdefine __DI edi
%ixdefine __SI esi
%ixdefine __BP ebp
%ixdefine __SP esp
%endif
;// register names used to help with macros invoke and uses
%ixdefine __nx_reg_eax eax
%ixdefine __nx_reg_ebx ebx
%ixdefine __nx_reg_ecx ecx
%ixdefine __nx_reg_edx edx
%ixdefine __nx_reg_edi edi
%ixdefine __nx_reg_esi esi
%ixdefine __nx_reg_ebp ebp
%ixdefine __nx_reg_esp esp
%ixdefine __nx_reg_rax rax
%ixdefine __nx_reg_rbx rbx
%ixdefine __nx_reg_rcx rcx
%ixdefine __nx_reg_rdx rdx
%ixdefine __nx_reg_rdi rdi
%ixdefine __nx_reg_rsi rsi
%ixdefine __nx_reg_rbp rbp
%ixdefine __nx_reg_rsp rsp
%ixdefine __nx_reg_r8  r8
%ixdefine __nx_reg_r9  r9
%ixdefine __nx_reg_r10 r10
%ixdefine __nx_reg_r11 r11
%ixdefine __nx_reg_r12 r12
%ixdefine __nx_reg_r13 r13
%ixdefine __nx_reg_r14 r14
%ixdefine __nx_reg_r15 r15


;//////////////////////////////////////////////////////////////
;//
;// nasm storage types
;//
%ixdefine byte_reserve   RESB
%ixdefine word_reserve   RESW
%ixdefine dword_reserve  RESD
%ixdefine qword_reserve  RESQ
%ixdefine tword_reserve  REST
%ixdefine oword_reserve  RESO
%ixdefine __m64_reserve  RESQ
%ixdefine __m128_reserve RESO
%ixdefine __int64_reserve  RESQ
%ixdefine __int128_reserve RESO
%ixdefine __nxfloat_reserve    RESD
%ixdefine __nxdouble_reserve   RESQ
%ixdefine __nxfloat32_reserve  RESD
%ixdefine __nxfloat64_reserve  RESQ
%ixdefine __nxfloat80_reserve  RESO
%ixdefine __nxfloat128_reserve RESO

;//////////////////////////////////////////////////////////////
;//
;// nasm storage types
;//
%ixdefine byte_storage   DB
%ixdefine word_storage   DW
%ixdefine dword_storage  DD
%ixdefine qword_storage  DQ
%ixdefine tword_storage  DT
%ixdefine oword_storage  DO
%ixdefine __m64_storage  DQ
%ixdefine __m128_storage DO
%ixdefine __int64_storage  DQ
%ixdefine __int128_storage DO
%ixdefine __nxfloat_storage    DD
%ixdefine __nxdouble_storage   DQ
%ixdefine __nxfloat32_storage  DD
%ixdefine __nxfloat64_storage  DQ
%ixdefine __nxfloat80_storage  DO
%ixdefine __nxfloat128_storage DO

;//////////////////////////////////////////////////////////////
;//
;// nasmx classifcation of types
;//
;// Bits used to contain classification:
;//  0-10 : size of type in bytes
;//    11 : reserved (0)
;// 12-15 : type   ( TODO: struc/union/class )
;//         0 = int type
;//         8 = (bit 15 set if float type, otherwise clear)
;// 
%ixdefine byte_type   0x0001
%ixdefine word_type   0x0002
%ixdefine dword_type  0x0004
%ixdefine qword_type  0x0008
%ixdefine tword_type  0x000A
%ixdefine oword_type  0x0010
%ixdefine __m64_type  0x8008
%ixdefine __m128_type 0x8010
%ixdefine __m256_type 0x8020
%ixdefine __int64_type  0x8008
%ixdefine __int128_type 0x8010
%ixdefine __nxfloat_type    0x8004
%ixdefine __nxdouble_type   0x8008
%ixdefine __nxfloat32_type  0x8004
%ixdefine __nxfloat64_type  0x8008
%ixdefine __nxfloat80_type  0x800A
%ixdefine __nxfloat128_type 0x8010

;// NASMX int types
%ixdefine int8_t     byte
%ixdefine uint8_t    byte
%ixdefine int16_t    word
%ixdefine uint16_t   word
%ixdefine int32_t    dword
%ixdefine uint32_t   dword
%ixdefine long32_t   dword
%ixdefine ulong32_t  dword
%ixdefine int64_t    qword
%ixdefine uint64_t   qword
%ixdefine int80_t    tword
%ixdefine uint80_t   tword
%ixdefine int128_t   oword
%ixdefine uint128_t  oword
%ixdefine long64_t   qword
%ixdefine ulong64_t  qword
%ixdefine long128_t  oword
%ixdefine ulong128_t oword
;// NASMX Unicode Transformation Format types
%ixdefine utf8_t     uint8_t
%ixdefine utf16_t    uint16_t
%ixdefine utf32_t    uint32_t
;// C types
;// The classification of long_t type  must be performed
;// during __OUTPUT_FORMAT__ check as the actual sizes
;// are different among operating systems.
%ixdefine char_t     int8_t
%ixdefine uchar_t    uint8_t
%ixdefine short_t    int16_t
%ixdefine ushort_t   uint16_t
%ixdefine int_t      int32_t
%ixdefine uint_t     uint32_t
;// float types require special handling due to
;// nasm FLOAT directives!
%ixdefine float_t    __nxfloat
%ixdefine double_t   __nxdouble
%ixdefine float32_t  __nxfloat32
%ixdefine float64_t  __nxfloat64
%ixdefine float80_t  __nxfloat80
%ixdefine float128_t __nxfloat128
%idefine __float(v) dword __float32__(v)
%idefine __double(v) qword __float64__(v)

;// Map between native size and token.
;// The define name was chosen as it will
;// be displayed to the user if the macro
;// nx_token_from_size() is called with
;// an illegal value during assembly
%ixdefine __nx_error_token_size_1  byte
%ixdefine __nx_error_token_size_2  word
%ixdefine __nx_error_token_size_4  dword
%ixdefine __nx_error_token_size_8  qword
%ixdefine __nx_error_token_size_10 tword
%ixdefine __nx_error_token_size_16 xmm
%ixdefine __nx_error_token_size_32 xmm

%define typeof(x) ((x %+ _type >> 12) & 0x000F)
%define sizeof(x) x %+ _size
%define reserve(x) x %+ _reserve
%define declare(x) x %+ _storage
%define nx_token_from_size(x) __nx_error_token_size_ %+ x
%idefine is_typeof_float(x) ((x %+ _type >> 15) & 0x0001)

;//////////////////////////////////////////////////////////////
;//
;// NASMX ABI size and pointer types
;//
;//                 Windows     Linux       BSD
;//        type     32   64     32   64     32   64
;// sizeof(int)      4    4      4    4      4    4
;// sizeof(long)     4    4      4    8      4    8
;// sizeof(ptr)      4    8      4    8      4    8
;// sizeof(size_t)   4    8      4    8      4    8
%ixdefine size32_t dword
%ixdefine size64_t qword
%ixdefine size_t size%[__BITS__]_t
%assign __NASMX_ABI_BITS__ sizeof(size_t) * 8
%ifnidn __BITS__,__NASMX_ABI_BITS__
	%fatal unknown size_t: __BITS__
%endif
%ixdefine ptr32_t dword
%ixdefine ptr64_t qword
;// use for 32->64 bit expansion of either int or ptr
%ixdefine ptrdiff_t ptr%[__BITS__]_t
%ixdefine NASMX_PTR ptr%[__BITS__]_t
;// remember: intptr's can be confusing when going
;// from 32-bit to 64-bit as sizeof(ptr) is increased
;// to 8 but sizeof(int) remains 4 potential allowing
;// very large (long) ints when using intptrs as ints.
%ixdefine intptr_t ptrdiff_t
%ixdefine uintptr_t ptrdiff_t

;//////////////////////////////////////////////////////////////
;//
;// Calling Convention Enumerations
;// Used within IMPORT, PROTO, PROC, & INVOKE
;//
;// BIT
;//   0-3: PREFIX - index into decorations (i.e. '_', '@', or '?' )
;//        all bits clear if no prefix
;//   4-7: SUFFIX - index into decorations (i.e. '@', '?', '$' )
;//        all bits clear if no suffix
;//  8-11: CALLCONV - calling convention enum
;//    12: ARGROTATE - rotate direction of args pushed to stack
;//        set if left rotation ( PASCAL = %rotate 1 ), clear if right rotation ( CDECL = %rotate -1)
;//    13: RETPOPSARGS - stack adjustment responsibility
;//        set if callee pops args ( PASCAL/STDCALL ), clear if caller adjusts stack ( CDECL )
;//
;// Notes:
;//    You may create your own calling convention for your operating
;//    system of choice by modifying the flags and macros.  However,
;//    the standard calling conventions should continue to exist to
;//    permit interfacing with system calls ( unless you're writing
;//    an OS with a new ABI/API ;)
;//
%define __NASMX_DECLSPEC_CC_PREFIX_MASK__      0x0007
%define __NASMX_DECLSPEC_CC_SUFFIX_MASK__      0x0070
%define __NASMX_DECLSPEC_CC_ENUM_MASK__        0x0F00
%define __NASMX_DECLSPEC_CC_ARGROTATE_MASK__   0x1000
%define __NASMX_DECLSPEC_CC_RETPOPSARGS_MASK__ 0x2000

;// universal calling conventions
%idefine __NASMX_DECLSPEC_CALLCONV_NAKED__    0
%idefine __NASMX_DECLSPEC_CALLCONV_PASCAL__   0x3100
;// although currently specific to win32, the
;// STDCALL calling convention can be useful
%idefine __NASMX_DECLSPEC_CALLCONV_STDCALL__  0x2321
;// The following 3 calling conventions are highly
;// platform dependent and are defined within the
;// OUTPUT_FORMAT check
;// %idefine __NASMX_DECLSPEC_CALLCONV_CDECL__ 0x????
;// %idefine __NASMX_DECLSPEC_CALLCONV_THISCALL__ 0x????
;// %idefine __NASMX_DECLSPEC_CALLCONV_FASTCALL__ 0x????

;// CALLCONV Enums
;// coincidentally compatible with DotGNU
;// System.Runtime.InteropServices.CallingConvention Enum
%idefine __NASMX_DECLSPEC_CC_ENUM_NAKED__     0
%idefine __NASMX_DECLSPEC_CC_ENUM_PASCAL__    1
%idefine __NASMX_DECLSPEC_CC_ENUM_CDECL__     2
%idefine __NASMX_DECLSPEC_CC_ENUM_STDCALL__   3
%idefine __NASMX_DECLSPEC_CC_ENUM_THISCALL__  4
%idefine __NASMX_DECLSPEC_CC_ENUM_FASTCALL__  5
;// %idefine __NASMX_DECLSPEC_CALLCONV_CC_ENUM_RDFCALL__   6

;// internal CALLCONV macros
%define nx_declspec_cc_prefix(x)       (x & 0x0007)
%define nx_declspec_cc_suffix(x)      ((x >> 4) & 0x0007)
%define nx_declspec_cc_enum(x)        ((x >> 8) & 0x000F)
%define nx_declspec_cc_argrotate(x)   ((x >> 12) & 0x0001)
%define nx_declspec_cc_retpopsargs(x) ((x >> 13) & 0x0001)

;// String containing chars that the prefix/suffix bits index into.
;// If you are adding __thiscall convention for your compiler of
;// choice make sure that the initial suffix char is present in
;// the decorations string.
%define __NASMX_DECLSPEC_CC_DECORATIONS__     '_@?$'
;// The following formatted string is used by the nx_is* functions
%define __NASMX_NUMALPHATOKEN__ '.+-0123456789EeABCDFabcdfGHIJKLMNOPQRSTUVWXYZghijklmnopqrstuvwxyz$@_'

;//////////////////////////////////////////////////////////////
;// internal macro call
;// used by IMPORT, PROC, & INVOKE
;// usage:
;//    nx_callconv_decorate callconv, funcname, suffix
;// examples:
;//    nx_callconv_decorate __NASMX_DECLSPEC_CALLCONV_STDCALL__, WinProc, 16
;//    nx_callconv_decorate __NASMX_DECLSPEC_CALLCONV_CDECL__, printf
;//    nx_callconv_decorate __NASMX_DECLSPEC_CALLCONV_THISCALL__, ClassName.MethodName, 'Z1yi4p8aue'
%macro nx_callconv_decorate 2-3.nolist 0
	%ifnum %1
		%assign %%__pidx nx_declspec_cc_prefix(%1)
		%assign %%__sidx nx_declspec_cc_suffix(%1)
	%else
		%fatal 'CALLCONV undefined:' %1
	%endif
	%ifnum %2
		%fatal 'internal error'
	%endif

	%if %%__pidx > 0
		%substr %%__sig __NASMX_DECLSPEC_CC_DECORATIONS__ %%__pidx
		%ifnum %2
			%fatal 'internal error'
		%elifid %2
			%defstr %%__func %2
		%eliftoken %2
			%defstr %%__func %2
		%else
			%define %%__func %2
		%endif
		%strcat %%__sig %%__sig,%%__func
	%else
		%ifstr %2
			%define %%__func %2
		%else
			%defstr %%__sig %2
		%endif
	%endif
	%if %%__sidx > 0
		%ifnum %3
			%defstr %%__suffix %3
		%elifid %3
			%defstr %%__suffix %3
		%else
			%ifstr %3
				%define %%__suffix %3
			%else
				%defstr %%__suffix %3
			%endif
		%endif
		%substr %%__stok __NASMX_DECLSPEC_CC_DECORATIONS__ %%__sidx
		%strcat %%__sig %%__sig,%%__stok,%%__suffix
	%endif

	%deftok %$__nxtok_decorated_name %%__sig

%endmacro

;//////////////////////////////////////////////////////////////
;//
;// Target Operating System Type definitions
;//
%define __NASMX_DECLSPEC_SYSTYPE_LINUX__   1
%define __NASMX_DECLSPEC_SYSTYPE_MACHOS__  2
%define __NASMX_DECLSPEC_SYSTYPE_BSD__     3
%define __NASMX_DECLSPEC_SYSTYPE_WINDOWS__ 4

;//////////////////////////////////////////////////////////////////
;// Macro to control NASMX default behavior
;// example usage:
;//    NASMX_PRAGMA CALLSTACK, 32 ; Enable callstack optimization
;//    NASMX_PRAGMA CALLSTACK, 0  ; Disable callstack optimization
;//    NASMX_PRAGMA WARNING, DISABLE, ALL
;//    NASMX_PRAGMA WARNING, DISABLE, 8001[, 8002, 8003]
;//    NASMX_PRAGMA WARNING, ENABLE, ALL
;// The macro will eventually allow us to enable/disable
;// warnings generated by the NASMX macro type system
%imacro NASMX_PRAGMA 1-*.nolist
	%ifidni WARNING,%1
		%warning actual pragma warnings currently unimplemented
		%ifidni ENABLE, %2
			;// assert: warnings are enabled if undefined
			%ifidni ALL,%3
				%ifdef __NASMX_PRAGMA_WARNING_8001__
					%undef __NASMX_PRAGMA_WARNING_8001__
				%endif
				%ifdef __NASMX_PRAGMA_WARNING_8002__
					%undef __NASMX_PRAGMA_WARNING_8002__
				%endif
			%else
				%rep %0-2
					%rotate 1
					%ifdef __NASMX_PRAGMA_WARNING_%1__
						%undef __NASMX_PRAGMA_WARNING_%1__
					%endif
				%endrep
			%endif
		%elifidni DISABLE, %2
			;// assert: warnings are disabled if defined
			%ifidni ALL,%3
				%ifndef __NASMX_PRAGMA_WARNING_8001__
					%define __NASMX_PRAGMA_WARNING_8001__
				%endif
				%ifndef __NASMX_PRAGMA_WARNING_8002__
					%define __NASMX_PRAGMA_WARNING_8002__
				%endif
			%else
				%rep %0-2
					%rotate 1
					%ifndef __NASMX_PRAGMA_WARNING_%3__
						%define __NASMX_PRAGMA_WARNING_%3__
					%endif
				%endrep
			%endif
		%else
			%error 'syntax: nasmx_pragma warning, [enable | disable], ...'
		%endif
	%elifidni CALLSTACK, %1
		;// This option is very useful with fastcall
		%ifdef NASMX_ENDPROC
			%error %1 pragma cannot be defined within procedure definition
		%endif
		%ifnum %2
			%assign %%__nx_stack_adjust %2
			%if %%__nx_stack_adjust > 0
				;// assert: turning on call stack feature
				%ifidni __OUTPUT_FORMAT__, win64
					;// assert: Win64 calling convention requires
					;// that the call stack be at least 32
					%if %%__nx_stack_adjust < 32
						%assign %%__nx_stack_adjust 32
					%endif
				%endif
				;// Note: x64 calling convention requires that
				;// the call stack be aligned to 16 bytes.
				;// Use 8 bytes for x86
				%assign %%__nx_align __NASMX_ABI_BITS__ / 4
				%assign %%__off (((%%__nx_stack_adjust + %%__nx_align) / %%__nx_align) * %%__nx_align)
				%assign %%__tmp (%%__nx_stack_adjust + %%__nx_align)
				%if (%%__off != %%__tmp)
					%assign %%__nx_stack_adjust %%__off
				%endif
			%endif
			;// This must be a global define since it is used within the procedure
			;// and during invoke and thus could be hidden by one or more user contexts
			%ifdef __NX_PROLOGUE_CALLSTACK_ADJUST__
				%undef __NX_PROLOGUE_CALLSTACK_ADJUST__
			%endif
			%if %%__nx_stack_adjust > 0
				%xdefine __NX_PROLOGUE_CALLSTACK_ADJUST__ %%__nx_stack_adjust
			%endif
		%else
			%error %1 pragma requires valid byte count
		%endif
	%else
		%error 'unknown nasmx_pragma:' %1
	%endif
%endmacro

;//////////////////////////////////////////////////////////////////
;// Macro to override default ABI behavior (Declare Specialization)
;// example usage:
;//    NASMX_DECLSPEC CALLCONV, PASCAL
;//    NASMX_DECLSPEC CALLCONV, STDCALL, DEFAULT
;//    NASMX_DECLSPEC SYSTYPE, LINUX
;//
;// notes:
;//    Make sure you understand the effects of using this command
;//    before making a global declaration!  Unresolved linker errors
;//    may haunt you!
%imacro NASMX_DECLSPEC 1-8.nolist
	%ifidni %1,CALLCONV
		%if %0 < 2
			%error 'nasmx_declspec syntax'
		%endif
		%ifndef __NASMX_DECLSPEC_CALLCONV_%[%2]__
			%error 'unknown declspec calltype:' %2
		%endif
		%if %0 == 2
			%ixdefine __NASMX_DECLSPEC_CALLCONV_DEFAULT__ __NASMX_DECLSPEC_CALLCONV_%[%2]__
		%else
			%ifdef __NASMX_DECLSPEC_CALLCONV_%[%3]__
				%warning 'declspec redefining' %1 %2 %3
			%endif
			%ixdefine __NASMX_DECLSPEC_CALLCONV_%[%3]__ __NASMX_DECLSPEC_CALLCONV_%[%2]__
		%endif
	%else
		%ifidni %1,SYSTYPE
			%ifndef __NASMX_DECLSPEC_SYSTYPE_%[%2]__
				%error 'unknown declspec systype:' %2
			%endif
			%if %0 == 2
				%ixdefine __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ __NASMX_DECLSPEC_SYSTYPE_%[%2]__
			%else
				%ifdef __NASMX_DECLSPEC_SYSTYPE_%[%3]__
					%warning 'declspec redefining' %1 %2 %3
				%endif
				%ixdefine __NASMX_DECLSPEC_SYSTYPE_%[%3]__ __NASMX_DECLSPEC_SYSTYPE_%[%2]__
			%endif
		%else
			%error 'unknown declspec:' %1
		%endif
	%endif
%endmacro


;//////////////////////////////////////////////////////////////////
;// OUTPUT_FORMAT check
;//
;// All system specific defines are made here
;//
%ifidn	 __OUTPUT_FORMAT__,elf
	%idefine __NASMX_DECLSPEC_CALLCONV_CDECL__    0x0200
	%idefine __NASMX_DECLSPEC_CALLCONV_THISCALL__ 0x0410
	%idefine __NASMX_DECLSPEC_CALLCONV_FASTCALL__ 0x0500
	%define __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__    2
	%define __NASMX_DECLSPEC_FASTCALL_MAXFLOATREGS__  0
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_4__ 'ECXEDX'
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_2__ 'CX DX '
	%ifdef FREEBSD
		NASMX_DECLSPEC SYSTYPE, BSD
	%elifdef NETBSD
		NASMX_DECLSPEC SYSTYPE, BSD
	%elifdef OPENBSD
		NASMX_DECLSPEC SYSTYPE, BSD
	%else
		NASMX_DECLSPEC SYSTYPE, LINUX
	%endif
	NASMX_DECLSPEC CALLCONV, CDECL, DEFAULT
	%idefine long_t long32_t
	%idefine ulong_t ulong32_t
	%idefine wchar_t dword
	%ifdef UNICODE
		%idefine NASMX_TCHAR wchar_t
		%idefine NASMX_TEXT(x) __utf32__(x)
	%else
		%idefine NASMX_TCHAR char_t
		%idefine NASMX_TEXT(x) x
	%endif
%elifidn __OUTPUT_FORMAT__,elf32
	;// gcc does NOT prepend underscores to functions
	;// declared using cdecl
	%idefine __NASMX_DECLSPEC_CALLCONV_CDECL__    0x0200
	%idefine __NASMX_DECLSPEC_CALLCONV_THISCALL__ 0x0410
	%idefine __NASMX_DECLSPEC_CALLCONV_FASTCALL__ 0x0500
	%define __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__    2
	%define __NASMX_DECLSPEC_FASTCALL_MAXFLOATREGS__  0
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_4__ 'ECXEDX'
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_2__ 'CX DX '
	%ifdef FREEBSD
		NASMX_DECLSPEC SYSTYPE, BSD	
	%elifdef NETBSD
		NASMX_DECLSPEC SYSTYPE, BSD	
	%elifdef OPENBSD
		NASMX_DECLSPEC SYSTYPE, BSD	
	%else
		NASMX_DECLSPEC SYSTYPE, LINUX
	%endif
	NASMX_DECLSPEC CALLCONV, CDECL, DEFAULT
	%idefine long_t long32_t
	%idefine ulong_t ulong32_t
	%idefine wchar_t dword
	%ifdef UNICODE
		%idefine NASMX_TCHAR wchar_t
		%idefine NASMX_TEXT(x) __utf32__(x)
	%else
		%idefine NASMX_TCHAR char_t
		%idefine NASMX_TEXT(x) x
	%endif
%elifidn __OUTPUT_FORMAT__,elf64
	%idefine __NASMX_DECLSPEC_CALLCONV_CDECL__    0x0200
	%idefine __NASMX_DECLSPEC_CALLCONV_THISCALL__ 0x0410
	%idefine __NASMX_DECLSPEC_CALLCONV_FASTCALL__ 0x0500
	%define __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__    6
	%define __NASMX_DECLSPEC_FASTCALL_MAXFLOATREGS__  8
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_8__ 'RDIRSIRDXRCXR8 R9 '
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_4__ 'EDIESIEDXECXR8DR9D'
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_2__ 'DI SI DX CX R8WR9W'
	%ifdef FREEBSD
		NASMX_DECLSPEC SYSTYPE, BSD	
	%elifdef NETBSD
		NASMX_DECLSPEC SYSTYPE, BSD	
	%elifdef OPENBSD
		NASMX_DECLSPEC SYSTYPE, BSD	
	%else
		NASMX_DECLSPEC SYSTYPE, LINUX
	%endif
	NASMX_DECLSPEC CALLCONV, FASTCALL, DEFAULT
	%idefine long_t long64_t
	%idefine ulong_t ulong64_t
	%idefine wchar_t dword
	%ifdef UNICODE
		%idefine NASMX_TCHAR wchar_t
		%idefine NASMX_TEXT(x) __utf32__(x)
	%else
		%idefine NASMX_TCHAR char_t
		%idefine NASMX_TEXT(x) x
	%endif
%elifidn __OUTPUT_FORMAT__,macho
	%idefine __NASMX_DECLSPEC_CALLCONV_CDECL__    0x0201
	%idefine __NASMX_DECLSPEC_CALLCONV_THISCALL__ 0x0411
	%idefine __NASMX_DECLSPEC_CALLCONV_FASTCALL__ 0x0501
	%define __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__    0
	%define __NASMX_DECLSPEC_FASTCALL_MAXFLOATREGS__  0
	NASMX_DECLSPEC SYSTYPE, MACHOS
	NASMX_DECLSPEC CALLCONV, CDECL, DEFAULT
	%idefine long_t long32_t
	%idefine ulong_t ulong32_t
	%idefine wchar_t dword
	%ifdef UNICODE
		%idefine NASMX_TCHAR wchar_t
		%idefine NASMX_TEXT(x) __utf32__(x)
	%else
		%idefine NASMX_TCHAR char_t
		%idefine NASMX_TEXT(x) x
	%endif
%elifidn __OUTPUT_FORMAT__,macho32
	%idefine __NASMX_DECLSPEC_CALLCONV_CDECL__    0x0201
	%idefine __NASMX_DECLSPEC_CALLCONV_THISCALL__ 0x0411
	%idefine __NASMX_DECLSPEC_CALLCONV_FASTCALL__ 0x0501
	%define __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__    0
	%define __NASMX_DECLSPEC_FASTCALL_MAXFLOATREGS__  0
	NASMX_DECLSPEC SYSTYPE, MACHOS
	NASMX_DECLSPEC CALLCONV, CDECL, DEFAULT
	%idefine long_t long32_t
	%idefine ulong_t ulong32_t
	%idefine wchar_t dword
	%ifdef UNICODE
		%idefine NASMX_TCHAR wchar_t
		%idefine NASMX_TEXT(x) __utf32__(x)
	%else
		%idefine NASMX_TCHAR char_t
		%idefine NASMX_TEXT(x) x
	%endif
%elifidn __OUTPUT_FORAMT__,macho64
	%idefine __NASMX_DECLSPEC_CALLCONV_CDECL__    0x0201
	%idefine __NASMX_DECLSPEC_CALLCONV_THISCALL__ 0x0411
	%idefine __NASMX_DECLSPEC_CALLCONV_FASTCALL__ 0x0501
	%define __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__    6
	%define __NASMX_DECLSPEC_FASTCALL_MAXFLOATREGS__  8
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_8__ 'RDIRSIRDXRCXR8 R9 '
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_4__ 'EDIESIEDXECXR8dR9d'
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_2__ 'DI SI DX CX R8wR9w'
	NASMX_DECLSPEC SYSTYPE, MACHOS
	NASMX_DECLSPEC CALLCONV, FASTCALL, DEFAULT
	%idefine long_t long64_t
	%idefine ulong_t ulong64_t
	%idefine wchar_t dword
	%ifdef UNICODE
		%idefine NASMX_TCHAR wchar_t
		%idefine NASMX_TEXT(x) __utf32__(x)
	%else
		%idefine NASMX_TCHAR char_t
		%idefine NASMX_TEXT(x) x
	%endif
%elifidn __OUTPUT_FORMAT__,win32
	%idefine __NASMX_DECLSPEC_CALLCONV_CDECL__    0x0201
	%idefine __NASMX_DECLSPEC_CALLCONV_THISCALL__ 0x2423
	%idefine __NASMX_DECLSPEC_CALLCONV_FASTCALL__ 0x2522
	%define __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__    2
	%define __NASMX_DECLSPEC_FASTCALL_MAXFLOATREGS__  0
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_4__ 'ECXEDX'
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_2__ 'CX DX '
	NASMX_DECLSPEC SYSTYPE, WINDOWS
	NASMX_DECLSPEC CALLCONV, STDCALL, DEFAULT
	%idefine long_t long32_t
	%idefine ulong_t ulong32_t
	%idefine wchar_t word
	%ifdef UNICODE
		%idefine NASMX_TCHAR wchar_t
		%idefine NASMX_TEXT(x) __utf16__(x)
	%else
		%idefine NASMX_TCHAR char_t
		%idefine NASMX_TEXT(x) x
	%endif
%elifidn __OUTPUT_FORMAT__,win64
	%idefine __NASMX_DECLSPEC_CALLCONV_CDECL__    0x0201
	%idefine __NASMX_DECLSPEC_CALLCONV_THISCALL__ 0x2423
	%idefine __NASMX_DECLSPEC_CALLCONV_FASTCALL__ 0x0500
	%define __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__    4
	%define __NASMX_DECLSPEC_FASTCALL_MAXFLOATREGS__  4
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_8__ 'RCXRDXR8 R9 '
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_4__ 'ECXEDXR8DR9D'
	%define __NASMX_DECLSPEC_FASTCALL_REGINTARGS_2__ 'CX DX R8WR9W'
	NASMX_DECLSPEC SYSTYPE, WINDOWS
	NASMX_DECLSPEC CALLCONV, FASTCALL, DEFAULT
	%idefine long_t long32_t
	%idefine ulong_t ulong32_t
	%idefine wchar_t word
	%ifdef UNICODE
		%idefine NASMX_TCHAR wchar_t
		%idefine NASMX_TEXT(x) __utf16__(x)
	%else
		%idefine NASMX_TCHAR char_t
		%idefine NASMX_TEXT(x) x
	%endif
%else
	%idefine __NASMX_DECLSPEC_CALLCONV_CDECL__    0x0200
	%idefine __NASMX_DECLSPEC_CALLCONV_THISCALL__ 0x0410
	%idefine __NASMX_DECLSPEC_CALLCONV_FASTCALL__ 0x0500
	%define __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__    0
	%define __NASMX_DECLSPEC_FASTCALL_MAXFLOATREGS__  0
	NASMX_DECLSPEC SYSTYPE, LINUX
	NASMX_DECLSPEC CALLCONV, CDECL, DEFAULT
	%if sizeof(size_t) == 32
		%idefine long_t long32_t
		%idefine ulong_t ulong32_t
	%else
		%idefine long_t long64_t
		%idefine ulong_t ulong64_t
	%endif
	%idefine wchar_t dword
	%ifdef UNICODE
		%idefine NASMX_TCHAR wchar_t
		%idefine NASMX_TEXT(x) __utf32__(x)
	%else
		%idefine NASMX_TCHAR char_t
		%idefine NASMX_TEXT(x) x
	%endif
%endif

;// pull in appropriate syscalls
%if __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ == __NASMX_DECLSPEC_SYSTYPE_BSD__
    %ifdef FREEBSD
	%include 'bsd/syscalls.freebsd.inc'
    %elifdef NETBSD
	%include 'bsd/syscalls.netbsd.inc'
    %elifdef OPENBSD
	%include 'bsd/syscalls.openbsd.inc'
    %endif
%elif __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ == __NASMX_DECLSPEC_SYSTYPE_LINUX__
    %include 'linux/syscall.inc'
%endif


;//////////////////////////////////////////////////////////////
;//
;// NASMX_STRUC
;//
%imacro NASMX_STRUC 0-*.nolist __NX_ROOT, __NX_ROOT, __NX_CTX_STRUC
	%ifnidni %3,__NX_CTX_STRUC
		%ifnidni %3,__NX_CTX_UNION
			%fatal unknown arg: %{3}
		%endif
	%endif
	%ifctx __NX_CTX_STRUC
		%define %%__is_nested 1
	%elifctx __NX_CTX_UNION
		%define %%__is_nested 1
	%else
		%define %%__is_nested 0
	%endif
	%ifidni %1, __NX_ROOT
		%define %%__is_named 0
	%else
		%define %%__is_named 1
	%endif
	%if (%%__is_nested == 0)
		%if (%%__is_named == 0)
			%fatal root structure or union requires name
		%endif
		%push %3
		%xdefine %$__nx_name %1
		%assign %$__nx_size 0
	%else
		%if (%%__is_named == 0)
			%xdefine %%__tname %{$__nx_name}
		%else
			%xdefine %%__tname %{$__nx_name}.%1
		%endif
		;// the following 5 line sequence fails on 2.08.02
		%assign %%__tsize %$__nx_size
		%push %3
		%xdefine %$__nx_name %%__tname
		%assign %$__nx_size %%__tsize
		%if (%%__is_named == 1)
			%{$__nx_name} equ %$__nx_size
		%endif
	%endif
	%assign %$__nx_osize %$__nx_size
	%assign %$__nx_usize 0
	%define %$__nx_isnamed %%__is_named
	%if (%%__is_named == 1)
		%ifidni %3,__NX_CTX_STRUC
			%xdefine %{$__nx_name}_type struc_type
		%elifidni %3,__NX_CTX_UNION
			%xdefine %{$__nx_name}_type union_type
		%else
			%fatal unknown arg: %{3}
		%endif
		%xdefine %{$__nx_name}_reserve byte_reserve
		%xdefine %{$__nx_name}_storage byte_storage
	%endif
%endmacro

%imacro NASMX_UNION 0-*.nolist __NX_ROOT, __NX_ROOT, __NX_CTX_UNION
	NASMX_STRUC %1, __NX_ROOT, __NX_CTX_UNION
%endmacro

%imacro NASMX_ALIGN 1.nolist
	%ifnctx __NX_CTX_STRUC
		%ifnctx __NX_CTX_UNION
			%fatal corrupt context
		%endif
	%endif
	%if (%1 > 0)
		%assign %%__off (((%$__nx_size + %1) / %1) * %1)
		%assign %%__tmp (%$__nx_size + %1)
		%if (%%__off != %%__tmp)
			%assign %$__nx_size %%__off
		%endif
	%endif
%endmacro

;// usage:
;//   NASMX_RESERVE hWnd, NASMX_PTR, 1
%imacro NASMX_RESERVE 2-3.nolist 1
	%ifnctx __NX_CTX_STRUC
		%ifnctx __NX_CTX_UNION
			%fatal corrupt context
		%endif
	%endif
	%ifndef %{2}_size
		%fatal unknown size for %{$__nx_name} %{1}: %{2}
	%endif
	%assign %%__argsize %{2}_size
	%if (%%__argsize < 1)
		%fatal invalid arg size for %{$__nx_name} %{1}: %{2}
	%endif
	%ifndef %{2}_storage
		%fatal unknown storage tag for %{$__nx_name} %{1}
	%endif

	%assign %%__qty %{3}
	%if (%%__qty < 1)
		%fatal invalid arg qty  for %{$__nx_name} %{1}: %{3}
	%endif

	%assign %%__amt (%%__argsize * %%__qty)

	%xdefine %[%{$__nx_name}.%{1}]_size %%__amt
	%xdefine %[%{$__nx_name}.%{1}]_type %[%{2}]_type
	%xdefine %[%{$__nx_name}.%{1}]_reserve %[%{2}]_reserve
	%xdefine %[%{$__nx_name}.%{1}]_storage %[%{2}]_storage
	%[%{$__nx_name}.%{1}] equ %[%{$__nx_size}]

	%ifctx __NX_CTX_UNION
		;// update current union size
		%if (%%__amt > %$__nx_usize)
			%assign %$__nx_usize %%__amt
		%endif
	%else
		;// update running size
		%assign %$__nx_size (%$__nx_size + %%__amt)
	%endif
%endmacro

%imacro NASMX_ENDSTRUC 0.nolist
	%ifnctx __NX_CTX_STRUC
		%ifnctx __NX_CTX_UNION
			%fatal corrupt context
		%endif
	%endif
	%if ((%$__nx_size-%$__nx_osize) > %$__nx_usize)
		%assign %$__nx_usize %$__nx_size-%$__nx_osize
	%endif
	%if (%$__nx_isnamed == 1)
		%{$__nx_name}_size equ %$__nx_usize
	%endif
	%assign %%__usize %$__nx_usize
	%pop
	%ifctx __NX_CTX_STRUC
		%assign %$__nx_size (%$__nx_size + %%__usize)
	%elifctx __NX_CTX_UNION
		%assign %$__nx_size (%$__nx_size + %%__usize)
		%if ((%$__nx_size-%$__nx_osize) > %$__nx_usize)
			%assign %$__nx_usize %$__nx_size-%$__nx_osize
		%endif
		%assign %$__nx_size %$__nx_osize
	%endif
%endmacro

%idefine NASMX_ENDUNION NASMX_ENDSTRUC

%imacro NASMX_ISTRUC 0-*.nolist __NX_ROOT, __NX_ROOT, __NX_CTX_ISTRUC
	%ifnidni %{3},__NX_CTX_ISTRUC
		%ifnidni %{3},__NX_CTX_IUNION
			%fatal unknown arg: %{3}
		%endif
	%endif
	%ifctx __NX_CTX_ISTRUC
		%define %%__is_nested 1
	%elifctx __NX_CTX_IUNION
		%define %%__is_nested 1
	%else
		%define %%__is_nested 0
	%endif
	%ifnidni %{1},__NX_ROOT
		%define %%__is_named 1
	%else
		%define %%__is_named 0
	%endif
	%if (%%__is_nested == 0)
		%if (%%__is_named == 0)
			%fatal root structure or union requires name
		%endif
		%ifidni %2, __NX_ROOT
			%fatal root structure or union requires base type
		%endif
		%push %3
		%xdefine %$__nx_istrucstart %{1}
		%xdefine %$__nx_name %{2}
		%[%$__nx_istrucstart]:
	%else
		%if (%%__is_named == 0)
			%xdefine %%__tname %{$__nx_name}
		%else
			%xdefine %%__tname %{$__nx_name}.%{1}
			%assign %%__off %%__tname
			%assign %%__times %%__off-($-%[%$__nx_istrucstart])
			%if (%%__times > 0)
				times %%__times db 0
			%endif
		%endif
		%push  %3
		%xdefine %$__nx_istrucstart %[%$$__nx_istrucstart]
		%xdefine %$__nx_name %%__tname
	%endif
	%define %$__nx_isnamed %%__is_named
%endmacro

%idefine NASMX_IUNION NASMX_ISTRUC

%imacro NASMX_AT 1-2+.nolist
	%assign %%__ofs %{$__nx_name}.%{1}
	%assign %%__count %%__ofs-($-%$__nx_istrucstart)
	%if (%%__count > 0)
		times %%__count db 0
	%endif
	%{$__nx_name}.%{1}_storage %2
%endmacro

%imacro NASMX_IENDSTRUC 0.nolist
	;// assert: finish up struc if needed
	%if (%$__nx_isnamed == 1)
		%assign %%__times %{$__nx_name}_size-($-%$__nx_istrucstart)
		%if (%%__times > 0)
			times %%__times db 0
		%endif
	%endif
	%pop
%endmacro

%idefine NASMX_IENDUNION NASMX_IENDSTRUC


;//////////////////////////////////////////////////////////////
;// MACRO nx_isalnum
;//
;// Purpose
;//    This macro is designed to return in the current context
;//    stack variable %$__nx_isalnum_bool if the parameter is
;//    alphanumeric.
;//
;// Usage:
;//    nx_isalnum chr
;//    nx_isalnum 'x'
;//
%macro nx_isalnum 1
    %ifstr %1
        %define %%__arg_chr %1
    %else
        %defstr %%__arg_chr %1
    %endif
    %assign %%__count 62
    %assign %%__idx 3
    %assign %$__nx_isalnum_bool 0
    %rep %%__count
        %assign %%__idx %%__idx + 1
        %substr %%__chr __NASMX_NUMALPHATOKEN__ %%__idx, 1
        %ifidn %%__chr, %%__arg_chr
            %assign %$__nx_isalnum_bool %%__idx
            %exitrep
        %endif
    %endrep
%endmacro

;//////////////////////////////////////////////////////////////
;// MACRO nx_ishexchr
;//
;// Purpose
;//    This macro is designed to return in the current context
;//    stack variable %$__nx_ishexnum_bool if the parameter is
;//    hexadecimal.
;//
;// Usage:
;//    nx_ishexchr chr
;//    nx_ishexchr 'f'
;//
%macro nx_ishexchr 1
    %ifstr %1
        %define %%__arg_chr %1
    %else
        %defstr %%__arg_chr %1
    %endif
    %assign %%__count 22
    %assign %%__idx 3
    %assign %$__nx_ishexchr_bool 0
    %rep %%__count
        %assign %%__idx %%__idx + 1
        %substr %%__chr __NASMX_NUMALPHATOKEN__ %%__idx, 1
        %ifidn %%__chr, %%__arg_chr
            %assign %$__nx_ishexchr_bool %%__idx
            %exitrep
        %endif
    %endrep
%endmacro

;//////////////////////////////////////////////////////////////
;// MACRO nx_isfloattok
;//
;// Purpose
;//    This macro is designed to return in the current context
;//    stack variable %$__nx_isfloattok_bool if the parameter is
;//    a float token.
;//
;// Usage:
;//    %define PI 3.14159
;//    nx_isfloattok PI
;//    nx_isfloattok string
;//
%macro nx_isfloattok 1
    %ifstr %1
        %define %%__arg_str %1
    %else
	%defstr %%__arg_str %1
    %endif
    %strlen %%__arg_len %%__arg_str
    %assign %$__nx_isfloattok 0
    %assign %%__found 0
    %assign %%__arg_idx 0
    %rep %%__arg_len
        %assign %%__arg_idx %%__arg_idx + 1
        %substr %%__arg_chr %%__arg_str %%__arg_idx, 1
        %assign %%__found 0
        %assign %%__idx 0
        %rep 15
            %assign %%__idx %%__idx + 1
            %substr %%__chr __NASMX_NUMALPHATOKEN__ %%__idx, 1
            %ifidn %%__chr, %%__arg_chr
              %assign %%__found %%__idx
              %exitrep
        %endif
        %endrep
        %if %%__found == 0
            %exitrep
        %endif
    %endrep
    %assign %$__nx_isfloattok_bool %%__found
%endmacro

;//////////////////////////////////////////////////////////////
;// MACRO nx_ishextok
;//
;// Purpose
;//    This macro is designed to return in the current context
;//    stack variable %$__nx_ishextok_bool if the parameter is
;//    a hex token.  Note that this macro also returns true if
;//    arg is an integer making it rather useful when we need
;//    to move the number to a register
;//
;// Usage:
;//    nx_ishextok token
;//    nx_ishextok 0xFFF0
;//
%macro nx_ishextok 1
    %ifstr %1
        %define %%__arg_str %1
    %else
        %defstr %%__arg_str %1
    %endif
    %strlen %%__arg_len %%__arg_str
    %assign %%__arg_idx 0
    %assign %%__has_x 0
    %rep %%__arg_len
        %assign %%__arg_idx %%__arg_idx + 1
        %substr %%__arg_chr %%__arg_str %%__arg_idx, 1
        %assign %%__found 0
        %ifidni %%__arg_chr, 'x'
            %if %%__has_x == 0
                %if %%__arg_idx < %%__arg_len
                    %assign %%__has_x 1
                    %assign %%__found 1
                %else
                    %exitrep
                %endif
            %else
                %exitrep
            %endif
        %else
            %assign %%__idx 1
            %rep 24
                %assign %%__idx %%__idx + 1
                %substr %%__chr __NASMX_NUMALPHATOKEN__ %%__idx, 1
                %ifidn %%__chr, %%__arg_chr
                    %assign %%__found %%__idx
                    %exitrep
                %endif
            %endrep
        %endif
        %if %%__found == 0
            %exitrep
        %endif
    %endrep
    %assign %$__nx_ishextok_bool %%__found
%endmacro

;//////////////////////////////////////////////////////////////
;// MACRO nx_strtok
;//
;// Purpose
;//    To parse a string and return delimited tokens
;//    Functionality is similar to the C strtok function
;//
;// Usage:                        ; After call:
;//    nx_strtok two tokens, ' '  ; %$__nx_token = two   , %$__nx_token_len = 3
;//    nx_strtok NULL, ' :?;'     ; %$__nx_token = tokens, %$__nx_token_len = 6
;//    nx_strtok NULL, ' :?;'     ; %$__nx_token = ''    , %$__nx_token_len = 0
;//
;// Notes:
;//    The return value is stored on the context stack as
;//    %$__nx_token thus it must only be called within a valid context.
;//    The length is also returned as %$__nx_token_len.  Check this value
;//    for 0 after call this macro before processing the return value.
;//
;//    Call more than once to get the next token in the sequence, if any.
;//    This function must be called at least once with a valid string
;//    or token to parse before passing in NULL (0) in subsequent calls.
;//
;//    Unfortunately, a side effect of using macros is that a comma which
;//    appears in a token actually causes the single token to split into 2
;//    tokens making it impossible to use this macro like:
;//         %define mytoken $32,000
;//         nx_strtok mytoken, ','
%imacro nx_strtok 2.nolist
	%ifnum %1
		%assign %%__nx_null %1
		%if %%__nx_null == 0
			%define %%__arg_str %$__nxtok_next
			%assign %%__arg_len %$__nxtok_len
		%else
			%defstr %%__arg_str %1
			%strlen %%__arg_len %%__arg_str
		%endif
	%else
		%ifstr %1
			%define %%__arg_str %1
		%else
			%defstr %%__arg_str %1
		%endif
		%strlen %%__arg_len %%__arg_str
	%endif

	%if %%__arg_len == 0
		%define %$__nxtok_next ''
		%assign %$__nxtok_len 0
		%define %$__nx_token ''
		%assign %$__nx_token_len 0
	%else
		%ifstr %2
			%define %%__sep_str %2
		%else
			%defstr %%__sep_str %2
		%endif
		%strlen %%__sep_len %%__sep_str

		%assign %$__nx_token_len 0
		%assign %%__arg_idx 1
		%rep %%__arg_len
			%substr %%__arg_chr %%__arg_str %%__arg_idx
			%assign %%__sep_idx 1
			%rep %%__sep_len
				%substr %%__sep_chr %%__sep_str %%__sep_idx
				%ifidn %%__arg_chr, %%__sep_chr
					%assign %$__nx_token_len %%__arg_idx-1
					%exitrep
				%else
					%assign %%__sep_idx 1+%%__sep_idx
				%endif
			%endrep
			%if %$__nx_token_len != 0
				%exitrep
			%else
				%assign %%__arg_idx 1+%%__arg_idx
			%endif
		%endrep
		%if %$__nx_token_len == 0
			%deftok %$__nx_token %%__arg_str
			%assign %$__nx_token_len %%__arg_len
			%define %$__nxtok_next ''
			%assign %$__nxtok_len 0
		%else
			%substr %%__tokstr %%__arg_str 1,%$__nx_token_len
			%deftok %$__nx_token %%__tokstr
			%if %$__nx_token_len < %%__arg_len
				%rep %%__arg_len - %$__nx_token_len
					%substr %%__arg_chr %%__arg_str %%__arg_idx
					%assign %%__nx_found 0
					%assign %%__sep_idx 1
					%rep %%__sep_len
						%substr %%__sep_chr %%__sep_str %%__sep_idx
						%ifidn %%__arg_chr, %%__sep_chr
							%assign %%__nx_found 1
							%exitrep
						%else
							%assign %%__sep_idx 1+%%__sep_idx
						%endif
					%endrep
					%if %%__nx_found == 0
						%exitrep
					%else
						%assign %%__arg_idx 1+%%__arg_idx
					%endif
				%endrep
				%if %%__arg_idx <= %%__arg_len
					%assign %$__nxtok_len %%__arg_len-%%__arg_idx+1
					%substr %$__nxtok_next %%__arg_str %%__arg_idx,%$__nxtok_len
				%else
					%define %$__nxtok_next ''
					%assign %$__nxtok_len 0
				%endif
			%else
				%define %$__nxtok_next ''
				%assign %$__nxtok_len 0
			%endif
		%endif
	%endif

%endmacro


;//////////////////////////////////////////////////////////////
;// MACRO nx_strchr
;//
;// Purpose
;//    This macro is designed to find position of char within string.
;//    It will update the current context stack variable %$__nx_strchr_pos
;//    with the index position of the chr or 0 if chr not found
;// Usage:
;//    nx_strchr string, chr
;//    nx_strchr [wc + hInstance], 'x'
;//
%macro nx_strchr 2
	%ifdef %$__nx_strchr_string
		%undef %$__nx_strchr_string
	%endif
	%ifstr %1
		%xdefine %$__nx_strchr_string %{1}
	%else
		%defstr %$__nx_strchr_string %{1}
	%endif
	%strlen %%__count %[%$__nx_strchr_string]
	%assign %%__idx 0
	%assign %%__found 0
	%rep %%__count
		%assign %%__idx %%__idx + 1
		%substr %%__chr %[%$__nx_strchr_string] %%__idx
		%ifidn %%__chr,%{2}
			%assign %%__found %%__idx
			%exitrep
		%endif
	%endrep
	%assign %$__nx_strchr_pos %%__found
%endmacro


;//////////////////////////////////////////////////////////////
;// MACRO nx_strstr
;//
;// Purpose
;//    This macro is designed to find position of substr within string.
;//    It will update the current context stack variable %$__nx_strstr_pos
;//    with the index position of the substr or 0 if not found
;// Usage:
;//    nx_strstr string, substr
;//    nx_strstr [rpb + hInstance], rbp
;//
%macro nx_strstr 2
	%ifdef %$__nx_strstr_string
		%undef %$__nx_strstr_string
	%endif
	%ifstr %1
		%xdefine %$__nx_strstr_string %{1}
	%else
		%defstr %$__nx_strstr_string %{1}
	%endif
	%ifstr %2
		%xdefine %$__nx_strstr_substring %{2}
	%else
		%defstr %$__nx_strstr_substring %{2}
	%endif
	%strlen %%__count %[%$__nx_strstr_string]
	%substr %%__subchr %[%$__nx_strstr_substring] 1
	%strlen %%__sublen %[%$__nx_strstr_substring]
	%assign %%__idx 0
	%assign %%__found 0
	%rep %%__count
		%assign %%__idx %%__idx + 1
		%if ((%%__count - %%__idx + 1) < %%__sublen)
			%exitrep
		%endif
		%substr %%__chr %[%$__nx_strstr_string] %%__idx
		%ifidni %%__chr,%%__subchr
			%substr %%__tmpstr %$__nx_strstr_string %%__idx, %%__sublen
			%ifidni %%__tmpstr, %$__nx_strstr_substring
				%assign %%__found %%__idx
				%exitrep
			%endif
		%endif
	%endrep
	%assign %$__nx_strstr_pos %%__found
	%undef %$__nx_strstr_string
	%undef %$__nx_strstr_substring
%endmacro


;// small helper macro used to parse __nxsig@@ defines
%macro nx_parse_tokens 1-100.nolist
	%ifdef __nx_token_0
		%undef __nx_token_0
	%endif
	%xdefine __nx_token_0 %0
	%assign %%__ctr 1
	%rep %0
		%ifdef __nx_token_%[%%__ctr]
			%undef __nx_token_%[%%__ctr]
		%endif
		%xdefine __nx_token_%[%%__ctr] %1
		%assign %%__ctr 1 + %%__ctr
		%rotate 1
	%endrep
%endmacro


;//////////////////////////////////////////////////////////////
;// MACRO IMPORT
;//
;// Purpose
;//    To define the signature and required syntax for
;//    an external function
;//
;// usage:
;//    IMPORT printf
;//    IMPORT GetSystemWindowsDirectoryA, 8
;//    IMPORT cdecl, myfunc, ptrdiff_t, dword, float_t
;//
%imacro IMPORT 1-100.nolist
	%push __NX_CTX_IMPORT__

	%assign %$__bytecount 0
	%ifndef __NASMX_DECLSPEC_CALLCONV_%[%1]__
		%assign %$__callconv __NASMX_DECLSPEC_CALLCONV_DEFAULT__
		%define %$__nx_proc %1
		%if %0 > 1
			%ifnum %2
				%ifidn __OUTPUT_FORMAT__,win32
					%assign %$__bytecount %2
				%endif
				%assign %$__repargs 0
			%else
				%assign %$__repargs %0-1
			%endif
		%else
			%assign %%__cc_enum nx_declspec_cc_enum(__NASMX_DECLSPEC_CALLCONV_DEFAULT__)
			%if %%__cc_enum == __NASMX_DECLSPEC_CC_ENUM_STDCALL__
				;// assert: no bytecount in import implies cdecl for win32
				%assign %$__callconv __NASMX_DECLSPEC_CALLCONV_CDECL__
;			%elif %%__cc_enum == __NASMX_DECLSPEC_CC_ENUM_FASTCALL__
;				;// assert: no bytecount in import implies cdecl for win32/win64 fastcall
;				%assign %$__callconv __NASMX_DECLSPEC_CALLCONV_CDECL__
			%endif
			%assign %$__repargs 0
		%endif
	%else
		%if %0 < 2
			%fatal usage: IMPORT [CALLCONV, ] FUNCNAME [,ARG1,ARG2,...]
		%endif
		%assign %$__callconv __NASMX_DECLSPEC_CALLCONV_%[%1]__
		%define %$__nx_proc %2
		%assign %$__repargs %0-2
	%endif

	%ifdef __nx_import_param_list
		%undef __nx_import_param_list
	%endif
	%assign %%__cc_enum nx_declspec_cc_enum(%[%$__callconv])
	%if %%__cc_enum != __NASMX_DECLSPEC_CC_ENUM_NAKED__
		;// assert: calc byte count for sig check
		%rep %$__repargs
			%rotate -1
			%ifnum %1
				%assign %$__bytecount %1
				%exitrep
			%else
				nx_strtok %1, ' '
				%assign %%__param_size sizeof(%$__nx_token)
				%if %%__param_size < sizeof(size_t)
					%assign %%__param_size sizeof(size_t)
				%endif
				%if %%__param_size > (sizeof(size_t) * 2)
					%error %1 exceeds allowable parameter size
				%endif
				%assign %$__bytecount %$__bytecount + %%__param_size
				%xdefine __nx_import_argtype %$__nx_token %+ _type
				%ifndef __nx_import_param_list
					%defstr __nx_import_param_list __nx_import_argtype
				%else
					%defstr __nx_import_argstr __nx_import_argtype
					%strcat __nx_import_param_list __nx_import_argstr,',',__nx_import_param_list
					%undef __nx_import_argstr
				%endif
				%undef __nx_import_argtype
			%endif
		%endrep
	%endif
	nx_callconv_decorate %$__callconv,%$__nx_proc,%$__bytecount
	%ifdef __nxsig@@%[%$__nxtok_decorated_name]
		%warning redefining import symbol %$__nx_proc as %[%$__nxtok_decorated_name]
	%endif

	%ifndef __nx_import_param_list
		%xdefine __nxsig@@%[%$__nxtok_decorated_name] %[%$__callconv],%[%$__bytecount]
	%else
		%deftok __nx_import_params __nx_import_param_list
		%xdefine __nxsig@@%[%$__nxtok_decorated_name] %[%$__callconv],%[%$__bytecount],__nx_import_params
		%undef __nx_import_params
		%undef __nx_import_param_list
	%endif
	%ifnidn %$__nx_proc,%$__nxtok_decorated_name
		%xdefine %[%$__nx_proc] %[%$__nxtok_decorated_name]
	%endif

	%pop
%endmacro


;//////////////////////////////////////////////////////////////
;// MACRO PROTO
;//
;// usage:
;//    PROTO [CALLCONV,] FUNCNAME [,ARGS...]
;//
;// examples:
;// Using default calling convention:
;//    PROTO WndProc, ptrdiff_t, dword, size_t, size_t
;// Specifying a calling convention
;//    PROTO cdecl, myfunc, ptrdiff_t, dword, float_t
;//
;// notes:
;//    This macro must be used when defining procedures
;//    that will be callable from other modules via
;//    the import macro as the function name is made
;//    global here.
;//
%imacro PROTO 1-100.nolist
	%push __NX_CTX_PROTO__
	%ifndef __NASMX_DECLSPEC_CALLCONV_%[%1]__
		%assign %$__callconv __NASMX_DECLSPEC_CALLCONV_DEFAULT__
		%define %$__proc %1
		%assign %$__repcnt %0-1
	%else
		%if %0 < 2
			%fatal usage: PROTO [CALLCONV, ] FUNCNAME [,ARG1,ARG2,...]
		%endif
		%assign %$__callconv __NASMX_DECLSPEC_CALLCONV_%[%1]__
		%define %$__proc %2
		%assign %$__repcnt %0-2
	%endif
	%ifdef __nx_proto_param_list
		%undef __nx_proto_param_list
	%endif
	%assign %$__bytecount 0
	%assign %%__cc_enum nx_declspec_cc_enum(%$__callconv)
	%if %%__cc_enum != __NASMX_DECLSPEC_CC_ENUM_NAKED__
		;// assert: calc byte count for sig check during invoke 
		%rep %$__repcnt
			%rotate -1
			%ifnum %1
				%assign %$__bytecount %1
				%exitrep
			%else
				nx_strtok %1, ' '
				%assign %%__param_size %[%$__nx_token]_size
				%if %%__param_size < sizeof(size_t)
					%assign %%__param_size sizeof(size_t)
				%endif
				%if %%__param_size > (sizeof(size_t) * 2)
					%error %1 exceeds allowable parameter size
				%endif
				%assign %$__bytecount %$__bytecount + %%__param_size
				%xdefine __nx_proto_argtype %$__nx_token %+ _type
				%ifndef __nx_proto_param_list
					%defstr __nx_proto_param_list __nx_proto_argtype
				%else
					%defstr __nx_proto_argstr __nx_proto_argtype
					%strcat __nx_proto_param_list __nx_proto_argstr,',',__nx_proto_param_list
					%undef __nx_proto_argstr
				%endif
				%undef __nx_proto_argtype
			%endif
		%endrep
	%endif

	%ifndef %[%$__proc]
		%ifdef __nxsig@@%[%$__proc]
			%fatal global symbol %[%$__proc] already defined
		%endif
		nx_callconv_decorate %$__callconv,%$__proc,%$__bytecount
		%ifndef __nx_proto_param_list
			%xdefine __nxsig@@%[%$__nxtok_decorated_name] %[%$__callconv],%[%$__bytecount]
		%else
			%deftok __nx_proto_params __nx_proto_param_list
			%xdefine __nxsig@@%[%$__nxtok_decorated_name] %[%$__callconv],%[%$__bytecount],__nx_proto_params
			%undef __nx_proto_params
			%undef __nx_proto_param_list
		%endif
		%ifnidn %$__proc,%$__nxtok_decorated_name
			%xdefine %[%$__proc] %[%$__nxtok_decorated_name]
		%endif
	%endif

	%ifndef __nxsig_declared_%[%$__proc]
		global %[%$__proc]
		%xdefine __nxsig_declared_%[%$__proc] global
	%endif

	%pop
%endmacro


;//////////////////////////////////////////////////////////////
;// MACRO ENTRY
;//
;// usage:
;//    ENTRY progname
;//
;// examples:
;//    ENTRY Demo1
;//
;// notes:
;//    program entry is assumed to be cdecl unless system default
;//    callconv specification is pascal.
;//    When building on Win64 or using pascal callconv you must
;//    change the makefile by removing the leading underscore
;//    from the linker /entry line
%imacro ENTRY 1.nolist
	%assign %%__cc_enum nx_declspec_cc_enum(__NASMX_DECLSPEC_CALLCONV_DEFAULT__)
	%if __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ == __NASMX_DECLSPEC_SYSTYPE_WINDOWS__
		;// main(argc, argv)
		%assign %%__bytecount __NASMX_ABI_BITS__ / 4
	%else
		;// main(argc, argv, envp)
		%assign %%__bytecount (( __NASMX_ABI_BITS__ / 8 ) * 3)
	%endif
	
	%if %%__cc_enum > __NASMX_DECLSPEC_CC_ENUM_PASCAL__
		%assign %%__cc_prefix nx_declspec_cc_prefix(__NASMX_DECLSPEC_CALLCONV_DEFAULT__)
		%if %%__cc_prefix > 0
			%xdefine %[%1] _main
			%xdefine __nxsig@@_main  __NASMX_DECLSPEC_CALLCONV_CDECL__, %%__bytecount
		%else
			%xdefine %[%1] main
			%xdefine __nxsig@@main  __NASMX_DECLSPEC_CALLCONV_DEFAULT__, %%__bytecount
		%endif
	%else
		%xdefine %[%1] main
		%xdefine __nxsig@@main  __NASMX_DECLSPEC_CALLCONV_DEFAULT__, %%__bytecount
	%endif
	%xdefine __nxsig_declared_%[%1] global
	global %[%1]
%endmacro

;////////////////////////////////////////////////////////////////////////
;//
;// Internal macro used by invoke to detect argv addressing mode
;// The context variable %$__nx_argv_mode_indirect will be set to
;// the string index where '[' is found and is also used as a flag
;// indicating retrieve the variable contents, not it's address.
;// The context variable %$__nx_argv_mode_bp will be set to the
;// the string index where the sign ( + or - ) is found and will
;// be used as a flag indicating that argv mode is used.
;// The context variables %$__nx_argv_mode_param_type, %$__nx_argv_mode_param_isfloat,
;// %$__nx_argv_mode_param_size, and %$__nx_argv_mode_param_castsize are set according
;// to the parameter list index supplied as the second argument to this macro.
%xdefine __NX_ARGV_MODE_SIGN_ADD__ 1
%xdefine __NX_ARGV_MODE_SIGN_SUB__ 2
%macro nx_analyze_argv_mode 2
	%ifnum %2
		%ifdef __nx_token_%[%2]
			%assign %$__nx_argv_mode_param_type __nx_token_%[%2]
			%assign %$__nx_argv_mode_param_isfloat ((%$__nx_argv_mode_param_type >> 15) & 0x01)
			%assign %$__nx_argv_mode_param_size %$__nx_argv_mode_param_type & 0x7F
		%else
			%assign %$__nx_argv_mode_param_type sizeof(size_t)
			%assign %$__nx_argv_mode_param_isfloat 0
			%assign %$__nx_argv_mode_param_size sizeof(size_t)
		%endif
	%else
		%fatal internal error - unknown parameter
	%endif

	%assign %$__nx_argv_mode_bp 0
	%assign %$__nx_argv_mode_sign 0
	%assign %$__nx_argv_mode_castsize sizeof(size_t)
	%assign %$__nx_argv_mode_fp_isnum 0
	%assign %$__nx_argv_mode_fp_num 0
	nx_strchr %{1}, '['
	%assign %$__nx_argv_mode_indirect %$__nx_strchr_pos
	%if %$__nx_argv_mode_indirect == 0
		nx_strstr %{1}, __BP
		%if %$__nx_strstr_pos > 0
			;// assert: potentially loading variable address offset from reg bp
			;// quick check to help ensure that bp is a token
			%assign %$__nx_isalnum_bool 0
			%defstr %%__str %[%1]
			%if %$__nx_strstr_pos > 1
				%substr %%__sub %%__str %$__nx_strstr_pos-1, 1
				nx_isalnum %%__sub
			%endif
			%if %$__nx_isalnum_bool == 0
				%strlen %%__len %%__str
				%if %%__len > %$__nx_strchr_pos + 2
					%substr %%__sub %%__str %$__nx_strstr_pos+3, 1
					nx_isalnum %%__sub
				%endif
			%endif
			%if %$__nx_isalnum_bool == 0
				;// we must be given a positive or negative offset in order to 
				;// generate code for local variable addressing
				nx_strchr %{1}, '+'
				%if %$__nx_strchr_pos == 0
					nx_strchr %{1}, '-'
					%if %$__nx_strchr_pos > 0
						%assign %$__nx_argv_mode_sign __NX_ARGV_MODE_SIGN_SUB__
					%endif
				%else
					%assign %$__nx_argv_mode_sign __NX_ARGV_MODE_SIGN_ADD__
				%endif
				%assign %$__nx_argv_mode_bp %$__nx_strchr_pos
			%endif
			%substr %%__off %%__str %$__nx_argv_mode_bp+1, -1
			%deftok %$__nx_argv_mode_bp_offset %%__off
		%elif %$__nx_argv_mode_param_isfloat > 0
			;// assert: check for float type and process
			nx_isfloattok %1
			%if %$__nx_isfloattok_bool != 0
				%assign %$__nx_argv_mode_fp_isnum 1
				%if %$__nx_argv_mode_param_size == 4
					%assign %$__nx_argv_mode_fp_num __float32__(%1)
				%else
					%assign %$__nx_argv_mode_fp_num __float64__(%1)
				%endif
			%else
				nx_strtok %1, ' '
				%ifndef __nx_reg_%[%$__nx_token]
					%iftoken %1
						%fatal expected floating point at arg %1
					%endif
					;// must check for casts which indicate
					;// __float() or __double() may have been used
					%ifdef %[%$__nx_token]_size
						%assign %%__size %[%$__nx_token]_size
						nx_strtok 0, ';'
					%else
						%assign %%__size %$__nx_argv_mode_param_size
						nx_strtok %1, ';'
					%endif
					%if %%__size > %$__nx_argv_mode_param_size
						%warning potential loss of precision from double to float for arg %1
					%endif
					%assign %$__nx_argv_mode_fp_isnum 1
					%assign %$__nx_argv_mode_fp_num %$__nx_token
				%endif
			%endif
		%endif
	%elif %$__nx_argv_mode_indirect > 3
		;// check for cast ( eg: dword[rbp+4] )
		%defstr %%__str %[%1]
		%substr %%__sub %%__str 1, %$__nx_argv_mode_indirect-1
		%deftok %%__tok %%__sub
		%ifdef %[%%__tok]_size
			%assign %$__nx_argv_mode_castsize %[%%__tok]_size
		%endif
	%endif
%endmacro

;//////////////////////////////////////////////////////////////
;// MACRO INVOKE
;//
;// usage:
;//    INVOKE func [, arg1] [,arg2]
;//
;// examples:
;//    invoke malloc, 64
;//
;// notes:
;//    when using bp offsets you should use qualifiers to ensure
;//    they are passed properly, ie: invoke malloc, dword [argv(.x)]
;//
%imacro INVOKE 1-100.nolist
	%ifndef NASMX_ENDPROC
		;// assert: we cannot check for context in case we are in a switch
		;// so we use nasmx_endproc
		%fatal 'invoke must be contained within proc/endproc directives.'
	%endif
	%ifndef __NX_LOCALS_PRESENT__
		;// require locals directive to ensure stack
		;// frame initialized properly prior to invoke.
		%fatal 'missing locals directive'
	%endif

	%push _NX_CTX_INVOKE
	%assign %$args 0
	%assign %$__nx_intargs 0           ;// init int args counter
	%assign %$__nx_floatargs 0         ;// init float args counter

	;// Fix for v1.0rc2: an imported definition may be zero (0)
	;// for CDECL varargs or not properly defined, thus we must
	;// calculate a valid bytecount based upon given parameters
	;// in order to get a proper stack frame built if callstack
	;// not defined.
	%ifdef __nxsig@@%[%1]
		nx_parse_tokens __nxsig@@%[%1]
		%assign %$__nx_callconv __nx_token_1
		%assign %$__nx_protobytecount __nx_token_2
		%ifndef __nxsig_declared_%[%1]
			%ifndef __nxsig_defined_%[%1]
				EXTERN	%[%1]
				%xdefine __nxsig_declared_%[%1] extern
			%endif
		%endif
	%else
		%assign %$__nx_callconv __NASMX_DECLSPEC_CALLCONV_DEFAULT__
		%assign %$__nx_protobytecount 0
	%endif

	%assign %%__repcnt %0 - 1
	%assign %$__nx_bytecount (( __NASMX_ABI_BITS__ / 8 ) * %%__repcnt)
	%if %$__nx_bytecount < %$__nx_protobytecount
		;// assert: possibly due to double param in
		;// 32bit mode or int128 in 64bit mode
		%assign %$__nx_bytecount %$__nx_protobytecount
	%endif

	%assign %%__nx_argrotate nx_declspec_cc_argrotate(%$__nx_callconv)
	%if %%__nx_argrotate > 0
		%assign %$__nx_argrotate 1
	%else
		%assign %$__nx_argrotate -1
	%endif
	%assign %$__nx_retpopsargs nx_declspec_cc_retpopsargs(%$__nx_callconv)
	%assign %%__cc_enum nx_declspec_cc_enum(%$__nx_callconv)
	%xdefine %$__nx_proc %1
	%if %%__cc_enum == __NASMX_DECLSPEC_CC_ENUM_FASTCALL__
		;// for fastcall, whenever CALLSTACK optimization is enabled,
		;// we use the stack built previously during locals/endlocals directive.
		%assign %$__nx_callstack_adjust 0
		%assign %$__nx_invoke_callstack_adjust %$__nx_bytecount
		%ifdef __NX_PROLOGUE_CALLSTACK_ADJUST__
			;// Verify that the CALLSTACK is sufficient for the
			;// number of parameter arguments we have been given.
			%assign %$__nx_callstack_adjust __NX_PROLOGUE_CALLSTACK_ADJUST__
			%if __NASMX_ABI_BITS__ < 64
				%if %$__nx_callstack_adjust < %$__nx_bytecount
					%assign %$__nx_callstack_adjust 0
					%warning insufficient callstack size for procedure %[%$__nx_proc], generating inefficient code...
				%endif
			%else
				%if __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ == __NASMX_DECLSPEC_SYSTYPE_WINDOWS__
					%if %$__nx_callstack_adjust < %$__nx_bytecount
						;// Windows: always requires a stack adjustment
						%assign %$__nx_callstack_adjust 0
						%warning insufficient callstack size for procedure %[%$__nx_proc], generating inefficient code...
					%else
						;// A callstack defined for Win64 must be at least 32 bytes
						%if %$__nx_callstack_adjust < 32
							%assign %$__nx_callstack_adjust 0
							%warning insufficient callstack size for procedure %[%$__nx_proc], generating inefficient code...
						%else
							%assign %$__nx_invoke_callstack_adjust 0
						%endif
					%endif
				%else
					;// Linux: required when register allocation exhausted
					%if %%__repcnt > 6
						%assign %$__nx_invoke_callstack_adjust (( %%__repcnt - 6 ) * ( __NASMX_ABI_BITS__ / 8 ))
						%if %$__nx_invoke_callstack_adjust > %$__nx_callstack_adjust
							%assign %$__nx_callstack_adjust 0
							%warning insufficient callstack size for procedure %[%$__nx_proc], generating inefficient code...
						%else
							%assign %$__nx_invoke_callstack_adjust 0
						%endif
					%else
						%assign %$__nx_invoke_callstack_adjust 0
					%endif
				%endif
			%endif
		%else
			%if __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ != __NASMX_DECLSPEC_SYSTYPE_WINDOWS__
				%if __NASMX_ABI_BITS__ > 32
					;// Make space for x64 args that could possibly get placed onto the stack
					%if %%__repcnt > 6
						;// TODO: Make more efficient by examining param types ( non-trivial ).
						%assign %$__nx_invoke_callstack_adjust (( %%__repcnt - 6 ) * ( __NASMX_ABI_BITS__ / 8 ))
					%else
						%assign %$__nx_invoke_callstack_adjust 0
					%endif
				%endif
			%endif
		%endif

		%assign %$__align 0
		%if %$__nx_invoke_callstack_adjust > 0
			;// assert: callstack not defined or insufficient, build it here
			%if __NASMX_ABI_BITS__ > 32
				%if __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ == __NASMX_DECLSPEC_SYSTYPE_WINDOWS__
					;// Windows requires at least 32 bytes of shadow storage
					%if %$__nx_invoke_callstack_adjust < 32
						%assign %$__nx_invoke_callstack_adjust 32
					%endif
				%endif
			%endif
			;// first determine if an alignment adjustment is needed
			%assign %$__align __NASMX_ABI_BITS__ / 4
			%assign %%__off (((%$__nx_invoke_callstack_adjust + %$__align) / %$__align) * %$__align)
			%assign %%__tmp (%$__nx_invoke_callstack_adjust + %$__align)
			%if (%%__off != %%__tmp)
				;// for fastcalls that retpopsargs we need to account
				;// for the extra space when stack restored later on
				%assign %$__align __NASMX_ABI_BITS__ / 8
				%assign %$__nx_invoke_callstack_adjust %%__off
			%else
				%assign %$__align 0
			%endif
			sub __SP, %$__nx_invoke_callstack_adjust
		%endif

		;// the following variable is used as a flag to catch a very bad logic state
		;// in 64 bit code when using RAX as a paramater since RAX is used to spill
		;// parameters to the stack, potentially causing a very tough bug to find!
		%assign %%__reg_ax_used 0

		%assign %$__param_type_ctr 2    ; arg types follow cconv,bytecount
		%assign %$args 0
		%rep %%__repcnt
			%rotate 1

			;// set up internal vars used to determine if argv address mode used
			%assign %$__param_type_ctr %$__param_type_ctr + 1
			nx_analyze_argv_mode %1, %$__param_type_ctr

			%assign %%__argtostack 1  ;// assume we need to store on stack
			%if %$__nx_argv_mode_param_isfloat == 0
				%if %$__nx_intargs < __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__
					%assign %%__argtostack 0
					%substr %%__regstr __NASMX_DECLSPEC_FASTCALL_REGINTARGS_%[%$__nx_argv_mode_castsize]__ (%$__nx_intargs * 3 + 1), 3
					%deftok %$arg_cur %%__regstr
					%undef %%__regstr
					%ifnum %1
						%if %1 == -1
							or %$arg_cur, %1
						%elif %1 == 0
							xor %$arg_cur, %$arg_cur
						%else
							mov %$arg_cur, %1
						%endif
					%elifstr %1
						jmp %%endstr_%$args
						%%str_%$args: declare(NASMX_TCHAR) NASMX_TEXT(%1), 0
						%%endstr_%$args:
						lea %$arg_cur,[rel %%str_%$args]
					%else
						%if %$__nx_argv_mode_indirect > 0
							mov  %$arg_cur, %1
						%elif %$__nx_argv_mode_bp > 0
							mov  %$arg_cur, __BP
							%if %$__nx_argv_mode_sign == __NX_ARGV_MODE_SIGN_ADD__
								add %$arg_cur, %$__nx_argv_mode_bp_offset
							%elif %$__nx_argv_mode_sign == __NX_ARGV_MODE_SIGN_SUB__
								sub %$arg_cur, %$__nx_argv_mode_bp_offset
							%else
								%fatal Unknown addressing mode for %{1}
							%endif
						%elifnidni %$arg_cur, %1
							;// if register parameter then guard against
							;// invalid register to register moves
							%ifdef __nx_reg_%[%1]
								%if %[%1]_size != __NASMX_ABI_BITS__ / 8
									%fatal %1 cannot be used as parameter for __NASMX_ABI_BITS__ bit mode
								%endif
							%endif
							mov  %$arg_cur, %1
						%endif
					%endif
					%if __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ == __NASMX_DECLSPEC_SYSTYPE_WINDOWS__
						;// Windows fastcall uses one index for param count which
						;// indexes either int or float for purpose of preloading regs
						%assign %$__nx_floatargs 1 + %$__nx_floatargs
						;// TODO: optimize register spill for when required only
						mov [__SP + %$args], %$arg_cur
						;// Unlike Linux/BSD x64 all current 32-bit and 64-bit versions
						;// of Windows increase arg stack offset.
						%assign %$args %$args + sizeof(size_t)
					%else
						;// increase arg stack offset only for 32-bit non-Windows systems.
						%if __NASMX_ABI_BITS__ < 64
							%assign %$args %$args + sizeof(size_t)
						%endif
					%endif
					%assign %$__nx_intargs 1 + %$__nx_intargs
					%undef %$arg_cur
				%endif
			%elif %$__nx_floatargs < __NASMX_DECLSPEC_FASTCALL_MAXFLOATREGS__
				%assign %%__argtostack 0
				%if __NASMX_ABI_BITS__ > 32
					;// assert: use 64-bit registers
					%if __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ == __NASMX_DECLSPEC_SYSTYPE_WINDOWS__
						%if   %$args == 0
							%xdefine %$arg_cur xmm0
						%elif %$args == 8
							%xdefine %$arg_cur xmm1
						%elif %$args == 16
							%xdefine %$arg_cur xmm2
						%elif %$args == 24
							%xdefine %$arg_cur xmm3
						%endif
					%else
						%if   %$__nx_floatargs == 0
							%xdefine %$arg_cur xmm0
						%elif %$__nx_floatargs == 1
							%xdefine %$arg_cur xmm1
						%elif %$__nx_floatargs == 2
							%xdefine %$arg_cur xmm2
						%elif %$__nx_floatargs == 3
							%xdefine %$arg_cur xmm3
						%elif  %$__nx_floatargs == 4
							%xdefine %$arg_cur xmm4
						%elif %$__nx_floatargs == 5
							%xdefine %$arg_cur xmm5
						%elif %$__nx_floatargs == 6
							%xdefine %$arg_cur xmm6
						%elif %$__nx_floatargs == 7
							%xdefine %$arg_cur xmm7
						%endif
					%endif

					%if %$__nx_argv_mode_indirect > 0
						nx_strtok %1, ' '
						%ifdef %[%$__nx_token]_size
							nx_strtok 0, ';'
							movsd %$arg_cur, %$__nx_token
						%else
							movsd %$arg_cur, %1
						%endif
						movsd [__SP + %$args], %$arg_cur
					%elif %$__nx_argv_mode_bp > 0
						nx_strtok %1, ' '
						%ifdef __nx_reg_%[%$__nx_token]
							movsd %$arg_cur, [%1]
						%elifdef %[%$__nx_token]_size
							nx_strtok 0, ';'
							movsd %$arg_cur, [ %[%$__nx_token] ]
						%else
							movsd %$arg_cur, [%1]
						%endif
						movsd [__SP + %$args], %$arg_cur
					%else
						%if %$__nx_argv_mode_fp_isnum != 0
							mov rax, %$__nx_argv_mode_fp_num
							mov qword [__SP + %$args], rax
							%assign %%__reg_ax_used 1
						%else
							mov qword [__SP + %$args], %1
						%endif
						movsd %$arg_cur, [__SP + %$args]
					%endif
				%else
					;// 32-bit fastcall float param
					%if %$__nx_argv_mode_param_size > 4
						;// assert: callee expecting double type which
						;// is always maintained on the stack
						%if %$__nx_argv_mode_fp_isnum != 0
							%assign %%__tmp (%$__nx_argv_mode_fp_num & 0x00000000FFFFFFFF)
							mov dword [__SP + %$args], %%__tmp
							%assign %%__tmp ((%$__nx_argv_mode_fp_num >> 32) & 0x00000000FFFFFFFF)
							mov dword [__SP+%$args+4], %%__tmp
						%else
							fld %1
							fstp qword [esp]
						%endif
						%assign %$args %$args + 8
					%else
						%if   %$args == 0
							%xdefine %$arg_cur ecx
						%elif %$args == 4
							%xdefine %$arg_cur edx
						%else
							%xdefine %$arg_cur __AX
							%assign %%__reg_ax_used 1
						%endif
						%if %$__nx_argv_mode_fp_isnum != 0
							mov %$arg_cur, %$__nx_argv_mode_fp_num
						%else
							mov %$arg_cur, %1
						%endif
						mov [__SP + %$args], %$arg_cur
						%assign %$args %$args + 4
					%endif
				%endif
				%if __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ == __NASMX_DECLSPEC_SYSTYPE_WINDOWS__
					;// Windows fastcall uses one index for either
					;// int or float params for purpose of preloading regs
					%assign %$__nx_intargs 1 + %$__nx_intargs
				%endif
				%assign %$__nx_floatargs 1 + %$__nx_floatargs
				%undef %$arg_cur
			%endif
			%if %%__argtostack != 0
				;// assert: no available registers
				%if %$__nx_argv_mode_param_isfloat == 0
					%ifnum %1
						%if __NASMX_ABI_BITS__ > 32
							%assign %%__reg_ax_used 1
							mov __AX, %{1}
							mov size_t [__SP + %$args], __AX
						%else
							mov size_t [__SP + %$args], %{1}
						%endif
					%elifstr %1
						jmp %%endstr_%$args
						%%str_%$args: declare(NASMX_TCHAR) NASMX_TEXT(%1), 0
						%%endstr_%$args:
						lea __AX,[rel %%str_%$args]
						mov [__SP + %$args], __AX
						%assign %%__reg_ax_used 1
					%else
						%if %$__nx_argv_mode_indirect > 0
							%if %$__nx_argv_mode_castsize < 4
								%xdefine %$arg_cur AX
							%elif %$__nx_argv_mode_castsize < 8
								%xdefine %$arg_cur EAX
							%else
								%xdefine %$arg_cur RAX
							%endif
							%assign %%__reg_ax_used 1
							mov %$arg_cur, %{1}
							mov [__SP + %$args], __AX
							%undef %$arg_cur
						%elif %$__nx_argv_mode_bp > 0
							%assign %%__reg_ax_used 1
							mov  __AX, __BP
							%if %$__nx_argv_mode_sign == __NX_ARGV_MODE_SIGN_ADD__
								add __AX, %$__nx_argv_mode_bp_offset
							%elif %$__nx_argv_mode_sign == __NX_ARGV_MODE_SIGN_SUB__
								sub __AX, %$__nx_argv_mode_bp_offset
							%else
								%fatal Unknown addressing mode for %{1}
							%endif
							mov [__SP + %$args], __AX
						%else
							%ifidni %1, __AX
								%if %%__reg_ax_used > 0
									%warning use of _AX as arg resulted in inconsistent logic state
								%endif
							%endif
							mov size_t [__SP + %$args], %{1}
						%endif
					%endif
				%else
					;// assert: processing float param
					%if %$__nx_argv_mode_indirect > 0
						%if %$__nx_argv_mode_castsize > %$__nx_argv_mode_param_size
							%warning potential loss of precision during conversion of double to float for %[%1]
						%endif
						fld __nx_error_token_size_%[%$__nx_argv_mode_castsize] %1
						fstp __nx_error_token_size_%[%$__nx_argv_mode_param_size] [__SP + %$args]
					%elif %$__nx_argv_mode_bp > 0
						fld __nx_error_token_size_%[%$__nx_argv_mode_param_size] [%1]
						fstp __nx_error_token_size_%[%$__nx_argv_mode_param_size] [__SP + %$args]
					%else
						%if %$__nx_argv_mode_fp_isnum != 0
							;// assert: arg is floating point constant
							%if %$__nx_argv_mode_param_size == 4
								;// assert: callee expecting float type
								mov dword [__SP + %$args], %$__nx_argv_mode_fp_num
							%else
								;// assert: callee expecting double type
								%assign %%__tmp (%$__nx_argv_mode_fp_num & 0x00000000FFFFFFFF)
								mov dword [__SP + %$args], %%__tmp
								%assign %%__tmp ((%$__nx_argv_mode_fp_num >> 32) & 0x00000000FFFFFFFF)
								mov dword [__SP+%$args+4], %%__tmp
							%endif
						%else
							mov __nx_error_token_size_%[%$__nx_argv_mode_param_size] [__SP + %$args], %1
						%endif
					%endif
				%endif
				%if %$__nx_argv_mode_param_size < sizeof(size_t)
					%assign %$args %$args + sizeof(size_t)
				%else
					%assign %$args %$args + %$__nx_argv_mode_param_size
				%endif
			%endif
		%endrep
		;// stack is set up, call the proc
		call	%$__nx_proc
		%if %$__nx_bytecount > 0
			%if %$__nx_callstack_adjust == 0
				%if %$__nx_retpopsargs == 0
					%if %$__nx_invoke_callstack_adjust > 0
						add	__SP, %$__nx_invoke_callstack_adjust
					%endif
				%elif %$__align > 0
					add	__SP, %$__align
				%endif
			%endif
		%endif
	%else
		;// arg types follow cconv,bytecount when defined
		%if %$__nx_argrotate > 0
			%assign %$__param_type_ctr 2    
		%else
			%assign %$__param_type_ctr %0+2
		%endif

		;// the following variable is used as a flag to catch a very bad logic state
		;// when using __AX as a paramater since __AX is used internally to spill certain
		;// parameters to the stack, potentially causing a very tough bug to find!
		%assign %%__reg_ax_used 0

		%rep %%__repcnt
			%rotate %$__nx_argrotate

			;// set up internal vars used to determine if argv address mode used
			%assign %$__param_type_ctr %$__param_type_ctr + %$__nx_argrotate
			nx_analyze_argv_mode %{1}, %$__param_type_ctr

			%if %$__nx_argv_mode_param_isfloat == 0
				%ifstr	%1
					jmp	%%endstr_%$args
					%%str_%$args: declare(NASMX_TCHAR) NASMX_TEXT(%{1}), 0
					%%endstr_%$args:
					push size_t %%str_%$args
				%elifnum %1
					push size_t %1
				%elif %$__nx_argv_mode_indirect > 0
					push size_t %1
				%elif %$__nx_argv_mode_bp > 0
					%assign %%__reg_ax_used 1
					mov  __AX, __BP
					%if %$__nx_argv_mode_sign == __NX_ARGV_MODE_SIGN_ADD__
						add __AX, %$__nx_argv_mode_bp_offset
					%elif %$__nx_argv_mode_sign == __NX_ARGV_MODE_SIGN_SUB__
						sub __AX, %$__nx_argv_mode_bp_offset
					%else
						%fatal Unknown addressing mode for %{1}
					%endif
					push __AX
				%else
					%ifidni %1, __AX
						%if %%__reg_ax_used > 0
							%warning use of _AX as arg resulted in inconsistent logic state
						%endif
					%endif
					push size_t %1
				%endif
				%assign %$args %$args + __BITS__ / 8
			%else
				;// process float param
				%if %$__nx_argv_mode_param_size < sizeof(size_t)
					;// if passing a float in 64-bit mode
					sub __SP, sizeof(size_t)
					%assign %$args %$args + sizeof(size_t)
				%else
					;// may be passing double in 32-bit mode
					sub  __SP, %$__nx_argv_mode_param_size
					%assign %$args %$args + %$__nx_argv_mode_param_size
				%endif
				%if %$__nx_argv_mode_indirect > 0
					%if %$__nx_argv_mode_castsize > %$__nx_argv_mode_param_size
						%warning potential loss of precision during conversion of double to float for %[%1]
					%endif
					fld __nx_error_token_size_%[%$__nx_argv_mode_castsize] %1
					fstp __nx_error_token_size_%[%$__nx_argv_mode_param_size] [__SP]
				%elif %$__nx_argv_mode_bp > 0
					fld __nx_error_token_size_%[%$__nx_argv_mode_param_size] [%1]
					fstp __nx_error_token_size_%[%$__nx_argv_mode_param_size] [__SP]
				%else
					%if %$__nx_argv_mode_fp_isnum != 0
						%if %$__nx_argv_mode_param_size == 4
							;// assert: callee expecting float type
							mov dword [__SP], %$__nx_argv_mode_fp_num
						%else
							;// assert: callee expecting double type
							%assign %%__tmp (%$__nx_argv_mode_fp_num & 0x00000000FFFFFFFF)
							mov dword [__SP], %%__tmp
							%assign %%__tmp ((%$__nx_argv_mode_fp_num >> 32) & 0x00000000FFFFFFFF)
							mov dword [__SP+4], %%__tmp
						%endif
					%else
						mov [__SP], %1
					%endif
				%endif
			%endif
		%endrep
		call %$__nx_proc
		%if %$args > 0
			%if %$__nx_retpopsargs == 0
				add	__SP, %$args
			%endif
		%endif
	%endif

	;// cleanup global arg token defs
	%assign %%__ctr 1
	%rep 16
		%ifdef __nx_token_%[%%__ctr]
			%undef __nx_token_%[%%__ctr]
		%endif
		%assign %%__ctr %%__ctr + 1
	%endrep

	;// restore context stack
	%pop
%endmacro


;//////////////////////////////////////////////////////////////
;// MACRO PROC
;//
;// usage:
;//    PROC [CALLCONV,] funcname [ arg, arg, ...]
;//
;// examples:
;//    Using default calling convention:
;//       proc funcname, arg1, arg2
;//    Specifying calling convention:
;//       proc cdecl, fprintf
;//
;// notes:
;//   The calling convention, if specified, must match either
;//   the prototype or import specification.
;//   Also remember that, when invoked during a fastcall, the
;//   register parameters are not automatically spilled, only
;//   the space assigned on the stack is allocated.
;//   This allows the user to determine whether or not to use
;//   the spill area if needed.
;//
%imacro PROC 1-100.nolist
	%ifdef NASMX_ENDPROC
		%fatal 'missing endproc directive.'
	%endif
	%push __NX_CTX_PROC__
	%assign %$__nx_locnt 0             ;// init locals offset
	%assign %$__nx_arg_bias 0          ;// init 64-bit bias
	%assign %$__nx_uses_stack_adjust 0 ;// init regs used save area
	%assign %$__nx_intargs 0           ;// init int args counter
	%assign %$__nx_floatargs 0         ;// init float args counter
	%assign %$__nx_uses_count 0        ;// init regs used counter
	%assign %$__nx_argcount 0          ;// init proc args counter
	%assign %$__nx_bytecount 0         ;// init total args byte count
	%assign %$__nx_protobytecount -1   ;// >= 0 if defined
	%ifndef __NASMX_DECLSPEC_CALLCONV_%[%1]__
		;// check for prototyped func
		%ifdef __nxsig@@%[%1]
			nx_parse_tokens __nxsig@@%[%1]
			%assign %$__nx_callconv __nx_token_1
			%assign %$__nx_protobytecount __nx_token_2
		%else
			%assign %$__nx_callconv __NASMX_DECLSPEC_CALLCONV_DEFAULT__
		%endif
		%define %$__nx_proc %1
		%assign %$__nx_repcnt %0-1
	%else
		%if %0 < 2
			%fatal 'usage: PROC [CALLCONV, ] FUNCNAME [,ARG1,ARG2,...]'
		%endif
		%assign %$__nx_callconv __NASMX_DECLSPEC_CALLCONV_%[%1]__
		%define %$__nx_proc %2
		%assign %$__nx_repcnt %0-2
		;// check for prototyped func
		%ifdef __nxsig@@%[%2]
			nx_parse_tokens __nxsig@@%[%2]
			%if %$__nx_callconv != __nx_token_1
				%fatal calling convention mismatch for %$__nx_proc
			%endif
			%assign %$__nx_protobytecount __nx_token_2
		%endif
	%endif

	%assign %$__nx_cc_enum nx_declspec_cc_enum(%[%$__nx_callconv])
	
	;// v1.0rc2 - we must always calculate a true bytecount such that
	;// the implementation of main(), or imported functions that are
	;// defined with a 0 bytecount, are defined properly
	%ifdef __nx_proto_param_list
		%undef __nx_proto_param_list
	%endif
	%rep %0-%$__nx_repcnt
		%rotate 1
	%endrep
	;// The signature must be defined in full before use because once the
	;// procedure has been defined we are not able to easily redefine it.
	;// Due to dependency on the X64_SPILL_OFFSET we cannot simply use one
	;// rep loop to generate equate offsets
	;// Thus the rep loop is required twice:
	;//     First, to build the parameter type list and x64 spill area size
	;//     Then, later on, to build the parameter offset equates.
	%rep %$__nx_repcnt
		nx_strtok %1, ' '
		;// assert: calculate bytecount in this loop to account
		;// for param types double/int128 rather than the simple logic of
		;// %assign %$__nx_bytecount %$__nx_repcnt * sizeof(size_t)
		;// Also build the parameter type list here in case the procedure
		;// has not been prototyped or imported.
		%xdefine __nx_proto_argtype %$__nx_token %+ _type
		%assign %%__param_type __nx_proto_argtype
		%assign %%__param_size (%%__param_type & 0x7F)
		%if %%__param_size < sizeof(size_t)
			%assign %$__nx_bytecount %$__nx_bytecount + sizeof(size_t)
		%elif %%__param_size <= __NASMX_ABI_BITS__ / 4
			%assign %$__nx_bytecount %$__nx_bytecount + %%__param_size
		%else
			%error parameter exceeds allowable size: %1
		%endif
		%ifndef __nx_proto_param_list
			%defstr __nx_proto_param_list __nx_proto_argtype
		%else
			%defstr __nx_proto_argstr __nx_proto_argtype
			%strcat __nx_proto_param_list __nx_proto_param_list,',',__nx_proto_argstr
			%undef __nx_proto_argstr
		%endif
		%undef __nx_proto_argtype
		%rotate 1
	%endrep

	%ifndef __nxsig@@%[%$__nx_proc]
		;// assert: we must generate an appropriate label for the function call
		;// here in order for the parameters to be equated properly
		nx_callconv_decorate %$__nx_callconv,%$__nx_proc,%$__nx_bytecount
		%ifdef __nx_proto_param_list
			%deftok __nx_proto_params __nx_proto_param_list
			%xdefine __nxsig@@%[%$__nxtok_decorated_name] %[%$__nx_callconv],%[%$__nx_bytecount],__nx_proto_params
		%else
			%xdefine __nxsig@@%[%$__nxtok_decorated_name] %[%$__nx_callconv],%[%$__nx_bytecount]
		%endif
		%ifnidn %$__nx_proc,%$__nxtok_decorated_name
			%xdefine %[%$__nx_proc] %[%$__nxtok_decorated_name]
		%endif
	%endif

	%ifdef __nxsig_defined_%[%$__nx_proc]
		%fatal procedure %[%$__nx_proc] already defined
	%else
		%xdefine __nxsig_defined_%[%$__nx_proc]
	%endif

	;// emit the procedure entry point
	%[%$__nx_proc]:

	;// The following define can/should be used by the user to properly
	;// return from a function ( ie: jmp NASMX_ENDPROC ) or via use of
	;// the RETURN macro
	%xdefine NASMX_ENDPROC __nx_endproc@@%[%$__nx_proc]

	%if %$__nx_cc_enum != __NASMX_DECLSPEC_CC_ENUM_NAKED__
		%assign %$__nx_argv __NASMX_ABI_BITS__ / 4
		%if __NASMX_ABI_BITS__ > 32
			%ifdef __NX_PROLOGUE_X64_SPILL_OFFSET__
				%undef __NX_PROLOGUE_X64_SPILL_OFFSET__
			%endif
			%if __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ != __NASMX_DECLSPEC_SYSTYPE_WINDOWS__
				%if %$__nx_cc_enum == __NASMX_DECLSPEC_CC_ENUM_FASTCALL__
					;// assert: create Linux spill area based on param byte count.
					;// This may result in slightly wasted stack space but is
					;// required to avoid alignment issues for params that
					;// exceed allocated registers and are located on the
					;// stack above RBP.
					%assign %$__nx_arg_bias %$__nx_bytecount

					;// keep stack aligned
					%assign %%__align __NASMX_ABI_BITS__ / 4
					%assign %%__off (((%$__nx_arg_bias + %%__align) / %%__align) * %%__align)
					%assign %%__tmp (%$__nx_arg_bias + %%__align)
					%if (%%__off != %%__tmp)
						%assign %$__nx_arg_bias %%__off
					%endif
					%xdefine __NX_PROLOGUE_X64_SPILL_OFFSET__ %[%$__nx_arg_bias]
				%endif
			%endif
			%ifndef __NX_PROLOGUE_X64_SPILL_OFFSET__
				;// assert: must always be defined for x64
				%xdefine __NX_PROLOGUE_X64_SPILL_OFFSET__ 0
			%endif
		%endif

		;// assert: calc byte count for sig check and argv() offsets
		;// we must reverse arg push logic to get correct results
		%assign %$__nx_bytecount 0
		%assign %$__cc_argrotate nx_declspec_cc_argrotate(%[%$__nx_callconv])
		%if %$__cc_argrotate != 0
			%assign %$__nx_argrotate -1
			%rotate -1
		%else
			%assign %$__nx_argrotate 1
			%rep %0-%$__nx_repcnt
				%rotate 1
			%endrep
		%endif

		%rep %$__nx_repcnt
			;// obtain param type and size and validate against current ABI
			nx_strtok %1, ' '
			%assign %%__isfloat is_typeof_float(%[%$__nx_token])
			%assign %%__param_size sizeof(%[%$__nx_token]) ; _size
			nx_strtok 0, ' '
			;// only allow exceeding of size_t for type double
			;// in 32-bit mode and 128-bit params in 64-bit mode
			%if %%__param_size > (sizeof(size_t) * 2)
			    %fatal %[%$__nx_token] invalid parameter size: %%__param_size
			%endif
			%if %%__param_size < sizeof(size_t)
				%assign %%__param_size sizeof(size_t)
			%endif
			%assign %$__nx_bytecount %$__nx_bytecount + %%__param_size

			%if __NASMX_ABI_BITS__ > 32
				%if %$__nx_cc_enum == __NASMX_DECLSPEC_CC_ENUM_FASTCALL__
					%if __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ == __NASMX_DECLSPEC_SYSTYPE_WINDOWS__
						;// assert: Windows 64-bit register spill area
						;// located above return address, thus bias
						;// not affected
						%assign %$__nx_intargs 1 + %$__nx_intargs
						%if %%__isfloat == 0
							%if %%__param_size > sizeof(size_t)
								%if %$__nx_intargs == __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__
									%error in procedure %$__nx_proc %$__nx_token exceeds available registers
								%endif
							%endif
						%endif
						.%[%$__nx_token] EQU %[%$__nx_argv]
						%assign %$__nx_argv %%__param_size + %$__nx_argv
					%else
						;// assert: Linux 64-bit register spill area
						;// located below frame pointer thus we must
						;// account for bias of args from frame while
						;// accounting for param types supplied
						%if %%__isfloat == 0
							%assign %$__nx_intargs 1 + %$__nx_intargs
							%if %$__nx_intargs > __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__
								.%[%$__nx_token] EQU __NX_PROLOGUE_X64_SPILL_OFFSET__ + %[%$__nx_argv]
								%assign %$__nx_argv %%__param_size + %$__nx_argv
							%else
								%if %$__nx_intargs == __NASMX_DECLSPEC_FASTCALL_MAXINTREGS__
									%if %%__param_size > sizeof(size_t)
										%error in procedure %$__nx_proc %$__nx_token exceeds available register size
									%endif
								%endif
								.%[%$__nx_token] EQU %[%$__nx_locnt]
								%assign %$__nx_locnt %%__param_size + %$__nx_locnt
							%endif
						%else
							%assign %$__nx_floatargs 1 + %$__nx_floatargs
							%if %$__nx_floatargs > __NASMX_DECLSPEC_FASTCALL_MAXFLOATREGS__
								.%[%$__nx_token] EQU __NX_PROLOGUE_X64_SPILL_OFFSET__ + %$__nx_argv
								%assign %$__nx_argv %%__param_size + %$__nx_argv
							%else
								.%[%$__nx_token] EQU %[%$__nx_locnt]
								%assign %$__nx_locnt %%__param_size + %$__nx_locnt
							%endif
						%endif
					%endif
				%else
					.%[%$__nx_token] EQU %[%$__nx_argv]
					%assign %$__nx_argv %$__nx_argv + %%__param_size
				%endif
			%else
				.%[%$__nx_token] EQU %[%$__nx_argv]
				%assign %$__nx_argv %$__nx_argv + %%__param_size
			%endif
			%rotate %$__nx_argrotate
		%endrep

		%if %$__nx_protobytecount > 0
			;// assert: we can only validate against prototypes that
			;// have a byte count since vararg functions can only be
			;// defined with zero (0) bytecount
			%if %$__nx_bytecount != %$__nx_protobytecount
				;// In the program entry main procedure the user may, or may not,
				;// be using command line or environment params.
				%ifnidni %$__nx_proc, main
					;// unfortunately we must check callconv for cdecl on win32
					;// and fastcall on win64 as currently the import files do
					;// not contain bytecounts to verify against.
					;// TODO: When import files are updated in the future we can
					;// revisit this code.
					%ifidni __OUTPUT_FORMAT__,win32
						%if %$__nx_cc_enum != __NASMX_DECLSPEC_CC_ENUM_CDECL__
							%warning prototype arg mismatch for %$__nx_proc
						%endif
					%elifidni __OUTPUT_FORMAT__,win64
						%if %$__nx_cc_enum != __NASMX_DECLSPEC_CC_ENUM_FASTCALL__
							%warning prototype arg mismatch for %$__nx_proc
						%endif
					%else
						%warning prototype arg mismatch for %$__nx_proc
					%endif
				%endif
			%endif
		%endif

		;// assert: always push __BP to get an aligned stack
		%define __NX_PROLOGUE_PRESENT__
		push __BP
		mov  __BP, __SP

		%ifidni %$__nx_proc, main
			;// assert: ensure we start out with an aligned stack
			%assign %%__nx_align ((__NASMX_ABI_BITS__ / 4 ) - 1)
			mov   __AX, __SP
			and   __AX, %%__nx_align
			jz    .nx_main_aligned
			sub   __SP, (__NASMX_ABI_BITS__ / 4 )
			%if __NASMX_ABI_BITS__ > 32
				and spl,BYTE 0xF0
			%else
				and   __SP, -8
			%endif
			.nx_main_aligned:
		%endif
	%endif
%endmacro


;//////////////////////////////////////////////////////////////
;// MACRO USES
;//
;// usage:
;//    USES reg [,reg ,reg, ...]
;//
;// examples:
;//    USES rbx, r15
;//
;// notes:
;//    This macro can only be used within the proc prologue
;//    Fatals are used rather than errors as this macro is
;//    extremely critical to proper function definition and
;//    stack alignment if used.
;//
%imacro USES 1-8.nolist
	%ifndef NASMX_ENDPROC
		%fatal 'missing proc directive.'
	%endif
	%ifnctx __NX_CTX_PROC__
		%fatal 'current context unknown.'
	%endif
	%ifdef __NX_LOCALS_PRESENT__
		%fatal 'uses directive must come before locals directive.'
	%endif
	%ifdef __NX_USES_PRESENT__
		%fatal 'uses directive previously set for this procedure.'
	%endif
	%define __NX_USES_PRESENT__
	%ifndef __NX_PROLOGUE_PRESENT__
		;// assert: at this point we have absolutely no way of knowing whether
		;// the function will contain locals or not; thus, using this macro
		;// forces our hand and requires us to set up the function prologue.
		;// The good news is that this aligns our stack properly.
		%define __NX_PROLOGUE_PRESENT__
		push __BP
		mov  __BP, __SP
	%endif

	%assign %%__size __NASMX_ABI_BITS__ / 8
	%assign %%__sys_size __NASMX_ABI_BITS__
	%assign %$__nx_uses_count 0
	%rep %0
		%ifndef %[%1]_size
			%fatal unknown register: %1
		%endif
		%assign %%__reg_size %[%1]_size
		%if %%__reg_size < %%__size
			%fatal extended version of register %1 required for %%__sys_size bit mode
		%elif %%__reg_size > %%__size
			%fatal %1 not available in %%__sys_size bit mode
		%endif

		%assign %$__nx_locnt %$__nx_locnt + %%__size
		%assign %$__nx_uses_count 1 + %$__nx_uses_count
		%xdefine __nx_uses_reg_%[%$__nx_uses_count] %[%1]
		mov [__BP - %$__nx_locnt ], %[%1]
		%rotate 1
	%endrep

	;// during endproc stack adjustment to include difference
	;// between locnt and uses_adjust
	%assign %$__nx_uses_stack_adjust %%__size  * %$__nx_uses_count
	%xdefine __NX_PROLOGUE_USES_ADJUST__ %[%$__nx_uses_stack_adjust]

%endmacro


;//////////////////////////////////////////////////////////////
;// MACRO LOCALS
;//
;// usage:
;//    locals [none]
;// where
;//    none - allows us to avoid using stackframe if user
;//           writing a small leaf function or will handle
;//           function prologue separately.  Also indicates
;//           that endlocals directive not required.
;//
;// notes:
;//    This macro can only be used within the proc prologue
;//    and must precede any assembly code. Invoke checks to
;//    verify that this macro was called to ensure that the
;//    prologue was initialized properly.
;//
%imacro LOCALS 0-1.nolist
	%ifndef NASMX_ENDPROC
		%error 'missing proc directive.'
	%endif
	%ifndef __NX_LOCALS_PRESENT__
		%define __NX_LOCALS_PRESENT__
	%endif
	%if %0 == 1
		%ifidni %1,none
			%define __NX_LOCALS_NONE__
		%else
			%warning unknown locals directive: %1
		%endif
	%endif

	%ifndef __NX_PROLOGUE_USES_ADJUST__
		%xdefine __NX_PROLOGUE_USES_ADJUST__ 0
	%endif

	%ifdef __NX_LOCALS_NONE__
		%ifdef __NX_PROLOGUE_CALLSTACK_ADJUST__
			%assign %$__nx_locnt %$__nx_locnt + __NX_PROLOGUE_CALLSTACK_ADJUST__
		%endif
		%if %$__nx_locnt > 0
			;// assert: the following block of code added for NASMX 1.0rc2
			;// to ensure that stack is aligned properly for functions
			;// which do not define any locals, save odd count registers,
			;// or linux x64 procs with unaligned spill area
			%assign %%__align __NASMX_ABI_BITS__ / 4
			%assign %%__off (((%$__nx_locnt + %%__align) / %%__align) * %%__align)
			%assign %%__tmp (%$__nx_locnt + %%__align)
			%if (%%__off != %%__tmp)
				%assign %$__nx_locnt (%$__nx_locnt + (__NASMX_ABI_BITS__ / 8))
			%endif
			sub  __SP, %$__nx_locnt
		%endif
	%endif
%endmacro


%imacro LOCAL 2-3.nolist 1
	%ifndef __NX_LOCALS_PRESENT__
		%error 'missing locals directive.'
	%endif
	%ifdef __NX_LOCALS_NONE__
		%error 'conflicts with directive: locals none'
	%endif
	%ifndef %[%2]_size
		%error %1 unknown size: %2
	%endif
	%assign %%__size %[%2]_size
	%assign %$__nx_locnt %$__nx_locnt + %%__size * %3

	;// ensure natural alignment of stack local variables
	;// TODO: A nice, but not trivial, optimization to reduce
	;// stack usage even further would be to reorganize equate
	;// offsets of local variables similar to a real compiler.
	%if %%__size <= 4
		%assign %%__align 4
	%elif %%__size <= 8
		%assign %%__align 8
	%elif %%__size > 16
		%assign %%__align __NASMX_ABI_BITS__ / 8
	%else
		%assign %%__align __NASMX_ABI_BITS__ / 4
	%endif
	%assign %%__off (((%$__nx_locnt + %%__align) / %%__align) * %%__align)
	%assign %%__tmp (%$__nx_locnt + %%__align)
	%if (%%__off != %%__tmp)
		%assign %$__nx_locnt %%__off
	%endif
	.%[%1] EQU %$__nx_locnt
%endmacro


%imacro ENDLOCALS 0.nolist
	%ifndef __NX_LOCALS_PRESENT__
		%error 'missing locals directive.'
	%endif
	%ifndef __NX_LOCALS_NONE__
		%ifdef __NX_PROLOGUE_CALLSTACK_ADJUST__
			%assign %$__nx_locnt %$__nx_locnt + __NX_PROLOGUE_CALLSTACK_ADJUST__
		%endif
		%assign %%__align __NASMX_ABI_BITS__ / 4
		%if %$__nx_locnt > 0
			;// keep stack aligned
			%assign %%__off (((%$__nx_locnt + %%__align) / %%__align) * %%__align)
			%assign %%__tmp (%$__nx_locnt + %%__align)
			%if (%%__off != %%__tmp)
				%assign %$__nx_locnt %%__off
			%endif
			sub  __SP, %$__nx_locnt
		%endif
	%endif
%endmacro

;// the following provide accessor macros to the procedure
;// parameter arguments and local variables
%if __NASMX_ABI_BITS__ > 32
	%if __NASMX_DECLSPEC_SYSTYPE_DEFAULT__ != __NASMX_DECLSPEC_SYSTYPE_WINDOWS__
		;// Linux x64 argv offsets defined as positive offsets from spill area
		%idefine argv(v) rbp + ( v - __NX_PROLOGUE_X64_SPILL_OFFSET__ )
	%else
		%idefine argv(v) rbp + v
	%endif
	%idefine  var(v) rbp - v
%else
	%idefine argv(v) ebp + v
	%idefine  var(v) ebp - v
%endif

%idefine OFFSET

%imacro RETURN 0-1.nolist
	%if %0 > 0
		%ifnum %1
			%if %1 == -1
				or __AX, %1
			%elif %1 == 0
				xor __AX, __AX
			%else
				mov __AX, %1
			%endif
		%else
			%ifnidni %1, __AX
				mov __AX, %[%1]
			%endif
		%endif
	%endif
	jmp  NASMX_ENDPROC
%endmacro

%imacro LEAVE 0.nolist
	%ifndef __NX_LEAVE_PRESENT__
		%define __NX_LEAVE_PRESENT__
		%ifdef NASMX_ENDPROC
			;// reset __SP to end of saved registers, if any
			%if __NASMX_ABI_BITS__ > 32
				%ifdef __NX_PROLOGUE_X64_SPILL_OFFSET__
					%assign %$__nx_locnt %$__nx_locnt - __NX_PROLOGUE_X64_SPILL_OFFSET__
				%endif
			%endif
			%ifdef __NX_PROLOGUE_USES_ADJUST__
				%if %$__nx_locnt > __NX_PROLOGUE_USES_ADJUST__
					%assign %$__nx_locnt %$__nx_locnt - __NX_PROLOGUE_USES_ADJUST__
				%endif
			%endif
			%ifdef __NX_LOCALS_PRESENT__
				%if %$__nx_locnt > 0
					add  __SP, %$__nx_locnt
				%endif
			%endif
			;// restore any non-volatile registers
			%ifdef __NX_USES_PRESENT__
				%assign %%__nx_reg %$__nx_uses_count
				%rep %$__nx_uses_count
					pop __nx_uses_reg_%[%%__nx_reg]
					%undef __nx_uses_reg_%[%%__nx_reg]
					%assign %%__nx_reg %%__nx_reg - 1
				%endrep
			%endif
		%endif
		%ifdef __NX_PROLOGUE_PRESENT__
			mov __SP, __BP
			pop __BP
		%endif
	%endif
%endmacro

%imacro RET 0-1.nolist
	%ifdef NASMX_ENDPROC
		%ifndef __NX_LEAVE_PRESENT__
			leave
		%endif
		%define __NX_RETURN_PRESENT__
	%endif

	%if %0 == 1
		%ifnum %1
			%if %1 > 0
				ret %1
			%else
				ret
			%endif
		%elifid %1
			%if %[%1] > 0
				ret %[%1]
			%else
				ret
			%endif
		%else
			%warning 'invalid operand supplied to ret opcode'
			ret
		%endif
	%else
		ret
	%endif
%endmacro

%imacro ENDPROC 0.nolist
	%ifndef NASMX_ENDPROC
		%fatal 'missing proc directive.'
	%endif

	;// provide a proc end jmp label
	%[NASMX_ENDPROC]:

	;// assert: we can not be certain at what context level we are at,
	;// thus we pop all contexts until we get to __NX_CTX_PROC__
	%rep 32
		;// Bugfix:
		;// Avoid problem with exitrep not exiting
		;// the loop immediately.
		%ifctx __NX_CTX_PROC__
			%exitrep
		%else
			%pop
		%endif
	%endrep

	%ifndef __NX_RETURN_PRESENT__
		%assign %%__cc_retpopsargs nx_declspec_cc_retpopsargs(%$__nx_callconv)
		%if %%__cc_retpopsargs == 0
			ret
		%else
			;// x64 retpopcnt may be different than bytecount
			%assign %$__nx_retpopcnt (%$__nx_argv - (__NASMX_ABI_BITS__ / 4))
			%if %$__nx_retpopcnt > 0
				ret %[%$__nx_retpopcnt]
			%else
				ret
			%endif
		%endif
	%endif
	%pop
	%undef NASMX_ENDPROC
	%ifdef __NX_LEAVE_PRESENT__
		%undef __NX_LEAVE_PRESENT__
	%endif
	%ifdef __NX_RETURN_PRESENT__
		%undef __NX_RETURN_PRESENT__
	%endif
	%ifdef __NX_USES_PRESENT__
		%undef __NX_USES_PRESENT__
	%endif
	%ifdef __NX_LOCALS_PRESENT__
		%undef __NX_LOCALS_PRESENT__
	%endif
	%ifdef __NX_LOCALS_NONE__
		%undef __NX_LOCALS_NONE__
	%endif
	%ifdef __NX_PROLOGUE_PRESENT__
		%undef __NX_PROLOGUE_PRESENT__
	%endif
	%ifdef __NX_PROLOGUE_USES_ADJUST__
		%undef __NX_PROLOGUE_USES_ADJUST__
	%endif
	%ifdef __NX_PROLOGUE_X64_SPILL_OFFSET__
		%undef __NX_PROLOGUE_X64_SPILL_OFFSET__
	%endif
%endmacro


%imacro RJMP 2.nolist
; This macro preforms the opposite conditional jump
; than what the value suggests, it's used internally
; by other macros
	%ifidni %1, ==
		jne %2
	%elifidni %1, >
		jle %2
	%elifidni %1, <
		jge %2
	%elifidni %1, >=
		jl %2
	%elifidni %1, <=
		jg %2
	%elifidni %1, !=
		je %2
	%elifidni %1, !>
		jg %2
	%elifidni %1, !<
		jl %2
	%elifidni %1, CARRY
		jnc %2
	%elifidni %1, BELOW
		jnb %2
	%elifidni %1, ABOVE
		jna %2
	%elifidni %1, PARITY
		jnp %2
	%elifidni %1, SIGNED
		jns %2
	%elifidni %1, OVERFLOW
		jno %2
	%elifidni %1, !CARRY
		jc %2
	%elifidni %1, !BELOW
		jb %2
	%elifidni %1, !ABOVE
		ja %2
	%elifidni %1, !PARITY
		jp %2
	%elifidni %1, !SIGNED
		js %2
	%elifidni %1, !OVERFLOW
		jo %2
	%endif
%endmacro


%imacro SJMP 2.nolist
	%ifidni %1, ==
		je %2
	%elifidni %1, >
		jg %2
	%elifidni %1, <
		jl %2
	%elifidni %1, >=
		jge %2
	%elifidni %1, <=
		jle %2
	%elifidni %1, !=
		jne %2
	%elifidni %1, !>
		jng %2
	%elifidni %1, !<
		jnl %2
	%elifidni %1, CARRY
		jc %2
	%elifidni %1, BELOW
		jb %2
	%elifidni %1, ABOVE
		ja %2
	%elifidni %1, PARITY
		jp %2
	%elifidni %1, SIGNED
		js %2
	%elifidni %1, OVERFLOW
		jo %2
	%elifidni %1, !CARRY
		jnc %2
	%elifidni %1, !BELOW
		jnb %2
	%elifidni %1, !ABOVE
		jna %2
	%elifidni %1, !PARITY
		jnp %2
	%elifidni %1, !SIGNED
		jns %2
	%elifidni %1, !OVERFLOW
		jno %2
	%endif
%endmacro



;//////////////////////////////////////////////////////////////////////////////
;//
;// The following macros provide a convenient way
;// to simulate high level constructs.  They allow
;// nesting of statements of arbitrary depth.
;// Note that the address labels generated are dot-
;// prepended to not break NASMX locals!

%imacro IF 3.nolist
%push __NX_CTX_IF__
	; we must obtain any outer context label sequence
	; to enable the break macro to succeed.
	%ifndef __NX_OPER_LABEL_SEQ__
		%xdefine __NX_OPER_LABEL_SEQ__ 0
	%endif
	%assign %$__nx_oper_label_seq %[__NX_OPER_LABEL_SEQ__]
	; sequence label numbers for the IF macro must be
	; separate from looping sequence label numbers.
	%ifndef __NX_OPER_IF_LABEL_SEQ__
		%xdefine __NX_OPER_IF_LABEL_SEQ__ 0
	%endif
	%assign %$__nx_oper_if_label_seq %[__NX_OPER_IF_LABEL_SEQ__] + 1
	%undef __NX_OPER_IF_LABEL_SEQ__
	%xdefine __NX_OPER_IF_LABEL_SEQ__ %[%$__nx_oper_if_label_seq]

	%assign %$__nx_oper_else 0
	%assign %$__nx_oper_if_label_next 1
	cmp %1, %3
	RJMP %2, .__NX_OPER_IF_LABEL_%[%$__nx_oper_if_label_seq]_%[%$__nx_oper_if_label_next]
%endmacro


%imacro ELSIF 3.nolist
	%ifctx __NX_CTX_IF__
		%if %$__nx_oper_else > 0
			%error "ELSIF cannot follow ELSE block."
		%endif
		jmp .__NX_OPER_IF_LABEL_%[%$__nx_oper_if_label_seq]
		.__NX_OPER_IF_LABEL_%[%$__nx_oper_if_label_seq]_%[%$__nx_oper_if_label_next]:
		%assign %$__nx_oper_if_label_next 1+%$__nx_oper_if_label_next
		cmp %1, %3
		RJMP %2, .__NX_OPER_IF_LABEL_%[%$__nx_oper_if_label_seq]_%[%$__nx_oper_if_label_next]
	%else
		%error "ELSIF directive must be within an IF block."
	%endif
%endmacro


%imacro ELSE 0.nolist
	%ifctx __NX_CTX_IF__
		%if %$__nx_oper_else > 0
			%error "There can only be one ELSE per IF block."
		%else
			%assign %$__nx_oper_else 1
			jmp .__NX_OPER_IF_LABEL_%[%$__nx_oper_if_label_seq]
			.__NX_OPER_IF_LABEL_%[%$__nx_oper_if_label_seq]_%[%$__nx_oper_if_label_next]:
			%assign %$__nx_oper_if_label_next 1+%$__nx_oper_if_label_next
		%endif
	%else
		%error "ELSE directive must be within an IF block."
	%endif
%endmacro


%imacro ENDIF 0.nolist
	%ifctx __NX_CTX_IF__
		; must have a forward label in case elsif/else macros used.
		.__NX_OPER_IF_LABEL_%[%$__nx_oper_if_label_seq]_%[%$__nx_oper_if_label_next]:
		.__NX_OPER_IF_LABEL_%[%$__nx_oper_if_label_seq]:
		%pop
	%else
		%error "ENDIF directive must be within an IF block."
	%endif
%endmacro


%imacro BREAK 0.nolist
	jmp .__NX_OPER_LABEL_EXIT_%[%$__nx_oper_label_seq]
%endmacro


%imacro CONTINUE 0.nolist
	jmp .__NX_OPER_LABEL_CONTINUE_%[%$__nx_oper_label_seq]
%endmacro


%imacro SWITCH 1.nolist
%push __NX_CTX_SWITCH__
	%ifndef __NX_OPER_LABEL_SEQ__
		%xdefine __NX_OPER_LABEL_SEQ__ 0
	%endif
	%assign %$__nx_oper_label_seq %[__NX_OPER_LABEL_SEQ__] + 1
	%undef __NX_OPER_LABEL_SEQ__
	%xdefine __NX_OPER_LABEL_SEQ__ %[%$__nx_oper_label_seq]
	%ifnidni __AX, %1
		mov __AX, %1
	%endif
	%assign %$__nx_oper_label_next 1
	%assign %$__nx_oper_default 0
%endmacro


%imacro CASE 1.nolist
	%ifctx __NX_CTX_SWITCH__
		%if %$__nx_oper_default > 0
			%error "CASE must appear before DEFAULT in SWITCH block."
		%endif
		.__NX_OPER_LABEL_%[%$__nx_oper_label_seq]_%[%$__nx_oper_label_next]:
		%assign %$__nx_oper_label_next 1+%$__nx_oper_label_next
		cmp __AX, %1
		jnz near .__NX_OPER_LABEL_%[%$__nx_oper_label_seq]_%[%$__nx_oper_label_next]
	%else
		%error "CASE directive must be within a SWITCH block."
	%endif
%endmacro


%imacro DEFAULT 0.nolist
	%ifctx __NX_CTX_SWITCH__
		%if %$__nx_oper_default > 0
			%error "There can only be one DEFAULT per SWITCH block."
		%endif
		%assign %$__nx_oper_default 1
		.__NX_OPER_LABEL_%[%$__nx_oper_label_seq]_%[%$__nx_oper_label_next]:
		%assign %$__nx_oper_label_next 1+%$__nx_oper_label_next
	%else
		%error "DEFAULT directive must be within a SWITCH block."
	%endif
%endmacro


%imacro ENDSWITCH 0.nolist
	%ifctx __NX_CTX_SWITCH__
		.__NX_OPER_LABEL_%[%$__nx_oper_label_seq]_%[%$__nx_oper_label_next]:
		.__NX_OPER_LABEL_EXIT_%[%$__nx_oper_label_seq]:
		%pop
	%else
		%error "Missing SWITCH directive."
	%endif
%endmacro


%imacro REPEAT 0.nolist
%push __NX_CTX_REPEAT__
	%ifndef __NX_OPER_LABEL_SEQ__
		%xdefine __NX_OPER_LABEL_SEQ__ 0
	%endif
	%assign %$__nx_oper_label_seq %[__NX_OPER_LABEL_SEQ__] + 1
	%undef __NX_OPER_LABEL_SEQ__
	%xdefine __NX_OPER_LABEL_SEQ__ %[%$__nx_oper_label_seq]
	%assign %$__nx_oper_label_next 1
	.__NX_OPER_LABEL_%[%$__nx_oper_label_seq]_%[%$__nx_oper_label_next]:
%endmacro


%imacro UNTIL 3.nolist
	%ifctx __NX_CTX_REPEAT__
		.__NX_OPER_LABEL_CONTINUE_%[%$__nx_oper_label_seq]:
		cmp %1, %3
		RJMP %2, .__NX_OPER_LABEL_%[%$__nx_oper_label_seq]_%[%$__nx_oper_label_next]
		.__NX_OPER_LABEL_EXIT_%[%$__nx_oper_label_seq]:
		%pop
	%elifctx __NX_CTX_DO__
		%warning "DO/UNTIL combination is deprecated.  Use REPEAT/UNTIL."
		.__NX_OPER_LABEL_CONTINUE_%[%$__nx_oper_label_seq]:
		cmp %1, %3
		RJMP %2, .__NX_OPER_LABEL_%[%$__nx_oper_label_seq]_%[%$__nx_oper_label_next]
		.__NX_OPER_LABEL_EXIT_%[%$__nx_oper_label_seq]:
		%pop
	%else
		%error "Missing REPEAT directive."
	%endif
%endmacro


%imacro DO 0.nolist
%push __NX_CTX_DO__
	%ifndef __NX_OPER_LABEL_SEQ__
		%xdefine __NX_OPER_LABEL_SEQ__ 0
	%endif
	%assign %$__nx_oper_label_seq %[__NX_OPER_LABEL_SEQ__] + 1
	%undef __NX_OPER_LABEL_SEQ__
	%xdefine __NX_OPER_LABEL_SEQ__ %[%$__nx_oper_label_seq]
	%assign %$__nx_oper_label_next 1
	.__NX_OPER_LABEL_%[%$__nx_oper_label_seq]_%[%$__nx_oper_label_next]:
%endmacro


; There is some trickery with this macro: If the DO context exists we assume
; that this is a finishing while statement; If no DO context is found we assume
; that the user is beginning a while loop.  This implies that you cannot use a
; WHILE loop directly inside a DO loop without some other intervening context.
; If your code requires this then you must use REPEAT/UNTIL for the outer loop.
%imacro WHILE 3.nolist
	%ifctx __NX_CTX_DO__
		.__NX_OPER_LABEL_CONTINUE_%[%$__nx_oper_label_seq]:
		cmp %1, %3
		SJMP %2, .__NX_OPER_LABEL_%[%$__nx_oper_label_seq]_%[%$__nx_oper_label_next]
		.__NX_OPER_LABEL_EXIT_%[%$__nx_oper_label_seq]:
		%pop
	%else
		%push __NX_CTX_WHILE__
		%ifndef __NX_OPER_LABEL_SEQ__
			%xdefine __NX_OPER_LABEL_SEQ__ 0
		%endif
		%assign %$__nx_oper_label_seq %[__NX_OPER_LABEL_SEQ__] + 1
		%undef __NX_OPER_LABEL_SEQ__
		%xdefine __NX_OPER_LABEL_SEQ__ %[%$__nx_oper_label_seq]
		%assign %$__nx_oper_label_next 1
		.__NX_OPER_LABEL_CONTINUE_%[%$__nx_oper_label_seq]:
		cmp %1, %3
		RJMP %2, .__NX_OPER_LABEL_EXIT_%[%$__nx_oper_label_seq]
	%endif
%endmacro


%imacro ENDWHILE 0.nolist
	%ifctx __NX_CTX_WHILE__
		jmp .__NX_OPER_LABEL_CONTINUE_%[%$__nx_oper_label_seq]
		.__NX_OPER_LABEL_EXIT_%[%$__nx_oper_label_seq]:
		%pop
	%else
		%error "Missing WHILE directive."
	%endif
%endmacro


;// The following is used to wrap test code when debugging
;%define __NX_DEBUG__  ;;//// * * * BEGIN NASMX DEBUG FRAME * * * ///////////////
%ifdef __NX_DEBUG__

%xdefine __NX_DEBUG_SOURCE_LINE_START__ __LINE__

;;// * * PLACE TEST CODE HERE * *

%xdefine __NX_DEBUG_SOURCE_LINE_END__ __LINE__
%ifndef __NX_DEBUG_SOURCE_LINE_START__
   %fatal NXDEBUG: BREAK line: __LINE__ section(????,__NX_DEBUG_SOURCE_LINE_END__) __NX_DEBUG_SOURCE_LINE_START__ undefined!
%endif
%assign __NX_DEBUG_SOURCE_LINE_COUNT__ __NX_DEBUG_SOURCE_LINE_END__ - __NX_DEBUG_SOURCE_LINE_START__
%fatal NXDEBUG: BREAK line: __LINE__ section(__NX_DEBUG_SOURCE_LINE_START__,__NX_DEBUG_SOURCE_LINE_END__) __NX_DEBUG_SOURCE_LINE_COUNT__ lines
%endif  ;;///////////////// * * * END NASMX DEBUG FRAME * * * /////////////////

%endif	; ifndef __NASMX_INCLUDED__

;// To speed up assembly we simply copy the values we use in our source
;// from the nasmx/inc/win32 include directory rather than including the
;// entire fileset as this really strains the preprocessor...
;//     %include 'win32\windows.inc'
;//     %include 'win32\kernel32.inc'
;//     %include 'win32\user32.inc'
;//     %include 'win32\msvcrt.inc'
;//
;// If you will be using additional Windows defines and structures for
;// your programs we recommend that you create your own include header
;// or simply include the above mentioned files in your source code.

;/////////////////////////////////////////////////////
;// WINDOWS.INC DEFINITIONS
;/////////////////////////////////////////////////////
%define MB_OK 0h
%define NULL 0
%define FALSE 0
%define TRUE 1
%define COLOR_BTNFACE 15
%define CS_HREDRAW 2h
%define CS_VREDRAW 1h
%define IDOK 1
%define IDCANCEL 2
%define IDI_APPLICATION 32512

%define IDC_ARROW 32512
%define IDC_IBEAM 32513
%define IDC_WAIT 32514
%define IDC_CROSS 32515
%define IDC_UPARROW 32516
%define IDC_SIZE 32640
%define IDC_ICON 32641

%define CBS_DROPDOWN 2h
%define CBS_SORT 100h

%define LBS_SORT 2h
%define LBS_HASSTRINGS 40h

%define SS_CENTER 1h
%define SS_NOPREFIX 80h

%define MB_ICONHAND 10h
%define MB_ICONQUESTION 20h
%define MB_ICONEXCLAMATION 30h
%define MB_ICONASTERISK 40h
%define MB_ICONERROR MB_ICONHAND
%define MB_ICONINFORMATION MB_ICONASTERISK
%define MB_ICONSTOP MB_ICONHAND
%define MB_ICONWARNING MB_ICONEXCLAMATION

%define STD_INPUT_HANDLE -10
%define STD_OUTPUT_HANDLE -11
%define STD_ERROR_HANDLE -12

%define SW_SHOWNORMAL 1
%define WS_OVERLAPPED 0h
%define WS_EX_TOOLWINDOW 00000080h
%define WS_CHILD 40000000h
%define WS_CAPTION 0C00000h
%define WS_SYSMENU 80000h
%define WS_VISIBLE 10000000h
%define WS_THICKFRAME 40000h
%define WS_GROUP 20000h
%define WS_TABSTOP 10000h
%define WS_MINIMIZEBOX 20000h
%define WS_MAXIMIZEBOX 10000h
%define WS_OVERLAPPEDWINDOW WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX
%define WS_EX_WINDOWEDGE 00000100h
%define WS_EX_CLIENTEDGE 00000200h
%define WS_EX_OVERLAPPEDWINDOW WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE

%define WM_CREATE 1h
%define WM_DESTROY 2h
%define WM_SETTEXT 0Ch
%define WM_GETTEXT 0Dh
%define WM_GETTEXTLENGTH 0Eh
%define WM_CLOSE 10h
%define WM_INITDIALOG 110h
%define WM_COMMAND 111h
%define WM_ERASEBKGND 14h
%define WM_PAINT 0Fh
%define WM_TIMER 113h
%define WM_KEYDOWN 100h
%define VK_ESCAPE 1Bh
%define SRCCOPY 0CC0020h

;/////////////////////////////////////////////////////
;// WINDOWS.INC STRUCTURES
;/////////////////////////////////////////////////////

NASMX_STRUC WNDCLASSEX
    NASMX_RESERVE cbSize,        uint32_t, 1
    NASMX_RESERVE style,         uint32_t, 1
    NASMX_RESERVE lpfnWndProc,   ptrdiff_t, 1
    NASMX_RESERVE cbClsExtra,    int32_t, 1
    NASMX_RESERVE cbWndExtra,    int32_t, 1
    NASMX_RESERVE hInstance,     ptrdiff_t, 1
    NASMX_RESERVE hIcon,         ptrdiff_t, 1
    NASMX_RESERVE hCursor,       ptrdiff_t, 1
    NASMX_RESERVE hbrBackground, ptrdiff_t, 1
    NASMX_RESERVE lpszMenuName,  ptrdiff_t, 1
    NASMX_RESERVE lpszClassName, ptrdiff_t, 1
    NASMX_RESERVE hIconSm,       ptrdiff_t, 1
NASMX_ENDSTRUC

NASMX_STRUC MSG
    NASMX_RESERVE hwnd,    ptrdiff_t, 1
    NASMX_RESERVE message, uint32_t, 1
    NASMX_RESERVE wParam,  size_t, 1
    NASMX_RESERVE lParam,  size_t, 1
    NASMX_RESERVE time,    uint32_t, 1
	NASMX_STRUC pt, POINT
		NASMX_RESERVE x, int32_t, 1
		NASMX_RESERVE y, int32_t, 1
	NASMX_ENDSTRUC
	%ifidn __OUTPUT_FORMAT__,macho
	    NASMX_RESERVE lPrivate, int32_t, 1
	%elifidn __OUTPUT_FORMAT__,macho32
	    NASMX_RESERVE lPrivate, int32_t, 1
	%elifidn __OUTPUT_FORMAT__,macho64
	    NASMX_RESERVE lPrivate, int64_t, 1
	%endif
NASMX_ENDSTRUC

NASMX_STRUC RECT
    NASMX_RESERVE left,   int32_t, 1
    NASMX_RESERVE top,    int32_t, 1
    NASMX_RESERVE right,  int32_t, 1
    NASMX_RESERVE bottom, int32_t, 1
NASMX_ENDSTRUC

NASMX_STRUC BITMAPINFOHEADER
    NASMX_RESERVE biSize, int32_t, 1
    NASMX_RESERVE biWidth, int32_t, 1
    NASMX_RESERVE biHeight, int32_t, 1
    NASMX_RESERVE biPlanes, short_t, 1
    NASMX_RESERVE biBitCount, short_t, 1
    NASMX_RESERVE biCompression, int32_t, 1
    NASMX_RESERVE biSizeImage, int32_t, 1
    NASMX_RESERVE biXPelsPerMeter, int32_t, 1
    NASMX_RESERVE biYPelsPerMeter, int32_t, 1
    NASMX_RESERVE biClrUsed, int32_t, 1
    NASMX_RESERVE biClrImportant, int32_t, 1
NASMX_ENDSTRUC

NASMX_STRUC PAINTSTRUCT
    NASMX_RESERVE hdc,         ptrdiff_t, 1
    NASMX_RESERVE fErase,      int32_t, 1
	NASMX_STRUC rcPaint, RECT
		NASMX_RESERVE left,    int32_t, 1
		NASMX_RESERVE top,     int32_t, 1
		NASMX_RESERVE right,   int32_t, 1
		NASMX_RESERVE bottom,  int32_t, 1
	NASMX_ENDSTRUC
    NASMX_RESERVE fRestore,    int32_t, 1
    NASMX_RESERVE fIncUpdate,  int32_t, 1
    NASMX_RESERVE rgbReserved, int8_t, 32 
NASMX_ENDSTRUC

NASMX_STRUC PALETTEENTRY
    NASMX_RESERVE peRed,   int8_t, 1
    NASMX_RESERVE peGreen, int8_t, 1
    NASMX_RESERVE peBlue,  int8_t, 1
    NASMX_RESERVE peFlags, int8_t, 1
NASMX_ENDSTRUC

;/////////////////////////////////////////////////////
;// KERNEL32.INC PROTOTYPES
;/////////////////////////////////////////////////////
IMPORT ExitProcess, 4
IMPORT GetConsoleMode, 8
IMPORT GetLastError, 0
IMPORT GetProcessHeap, 0
IMPORT GetStdHandle, 4
IMPORT HeapAlloc, 12
IMPORT HeapFree, 12
IMPORT ReadFile, 20
IMPORT SetConsoleMode, 8
IMPORT WaitForSingleObject, 8
IMPORT WriteFile, 20

;/////////////////////////////////////////////////////
;// USER32.INC PROTOTYPES
;/////////////////////////////////////////////////////
IMPORT DestroyWindow, ptrdiff_t hWnd
IMPORT EndDialog, ptrdiff_t hDlg, size_t nResult
IMPORT GetClientRect, ptrdiff_t hWnd, ptrdiff_t lpRect
IMPORT PostQuitMessage, dword nExitCode
IMPORT ShowWindow, ptrdiff_t hWnd, dword nCmdShow
IMPORT TranslateMessage, ptrdiff_t lpMsg
IMPORT UpdateWindow, ptrdiff_t hWnd
IMPORT InvalidateRect, ptrdiff_t hWnd, ptrdiff_t lpRect, dword bErase
IMPORT SetTimer, ptrdiff_t hWnd, size_t nIDEvent, dword uElapse, ptrdiff_t lpTimerFunc
IMPORT KillTimer, ptrdiff_t hWnd, size_t nIDEvent

IMPORT CreateDCA, 16
IMPORT CreateDIBSection, 24
IMPORT SelectObject, 8
IMPORT DeleteObject, 4
IMPORT DeleteDC, 4
IMPORT CreateCompatibleDC, 4
IMPORT BeginPaint, 8
IMPORT BitBlt, 36
IMPORT EndPaint, 8
IMPORT CreateCompatibleBitmap, 12

;/////////////////////////////////////////////////////
;// MSVCRT.INC PROTOTYPES
;/////////////////////////////////////////////////////
;//
;// C runtime functions must be handled with care when
;// switching between 32 and 64 bit mode since in 32 bit
;// mode they are defined as cdecl while in 64 bit mode
;// they are defined as default ( fastcall ).
;// VarArg functions such as printf should never be defined
;// ( prototyped ) to include a bytecount
%ifidn __BITS__,64
IMPORT printf
IMPORT scanf
%else
IMPORT cdecl, printf
IMPORT cdecl, scanf
IMPORT cdecl, _getch
%endif

%ifdef UNICODE

;/////////////////////////////////////////////////////
;// KERNEL32.INC UNICODE PROTOTYPES
;/////////////////////////////////////////////////////
IMPORT GetModuleHandleW, 4
%define GetModuleHandle GetModuleHandleW

;/////////////////////////////////////////////////////
;// USER32.INC UNICODE PROTOTYPES
;/////////////////////////////////////////////////////
;//
;// The following shows how to properly prototype an imported
;// windows function using NASMX syntax.
;// TODO: This will eventually be done for all imported functions.
IMPORT MessageBoxW, ptrdiff_t hWnd, ptrdiff_t lpText, ptrdiff_t lpCaption, uint32_t uType
%define MessageBox MessageBoxW
IMPORT CreateWindowExW, uint_t dwExStyle, ptrdiff_t lpClassName, ptrdiff_t lpWindowName, uint_t dwStyle,\
                        int_t x, int_t y, int_t nWidth, int_t nHeight,\
                        ptrdiff_t hWndParent, ptrdiff_t hMenu, ptrdiff_t hInstance, ptrdiff_t lpParam
%define CreateWindowEx CreateWindowExW
IMPORT DefWindowProcW, ptrdiff_t hWnd, uint_t Msg, ptrdiff_t wParam, ptrdiff_t lParam
%define DefWindowProc DefWindowProcW
IMPORT DialogBoxParamW, 20
%define DialogBoxParam DialogBoxParamW
IMPORT DispatchMessageW, 4
%define DispatchMessage DispatchMessageW
IMPORT GetMessageW, 16
%define GetMessage GetMessageW
IMPORT LoadIconW, 8
%define LoadIcon LoadIconW
IMPORT PostMessageW, ptrdiff_t hWnd, dword Msg, size_t wParam, size_t lParam
%define PostMessage PostMessageW
IMPORT RegisterClassExW, 4
%define RegisterClassEx RegisterClassExW
IMPORT SendDlgItemMessageW, 20
%define SendDlgItemMessage SendDlgItemMessageW
%ifidni __OUTPUT_FORMAT__,win64
IMPORT wsprintfW
%else
IMPORT cdecl, wsprintfW
%endif
%define wsprintf wsprintfW

%else  ;// ANSI

;/////////////////////////////////////////////////////
;// KERNEL32.INC ANSI PROTOTYPES
;/////////////////////////////////////////////////////
IMPORT GetModuleHandleA, 4
%define GetModuleHandle GetModuleHandleA

;/////////////////////////////////////////////////////
;// USER32.INC ANSI PROTOTYPES
;/////////////////////////////////////////////////////
IMPORT CreateWindowExA, uint_t dwExStyle, ptrdiff_t lpClassName, ptrdiff_t lpWindowName, uint_t dwStyle,\
						int_t x, int_t y, int_t nWidth, int_t nHeight,\
						ptrdiff_t hWndParent, ptrdiff_t hMenu, ptrdiff_t hInstance, ptrdiff_t lpParam
%define CreateWindowEx CreateWindowExA
IMPORT DefWindowProcA, ptrdiff_t hWnd, uint_t Msg, ptrdiff_t wParam, ptrdiff_t lParam
%define DefWindowProc DefWindowProcA
IMPORT DialogBoxParamA, 20
%define DialogBoxParam DialogBoxParamA
IMPORT DispatchMessageA, 4
%define DispatchMessage DispatchMessageA
IMPORT GetMessageA, 16
%define GetMessage GetMessageA
IMPORT LoadIconA, 8
%define LoadIcon LoadIconA
IMPORT MessageBoxA, ptrdiff_t hWnd, ptrdiff_t lpText, ptrdiff_t lpCaption, uint32_t uType
%define MessageBox MessageBoxA
IMPORT PostMessageA, ptrdiff_t hWnd, dword Msg, size_t wParam, size_t lParam
%define PostMessage PostMessageA
IMPORT RegisterClassExA, 4
%define RegisterClassEx RegisterClassExA
IMPORT SendDlgItemMessageA, 20
%define SendDlgItemMessage SendDlgItemMessageA
%ifidni __OUTPUT_FORMAT__,win64
IMPORT wsprintfA
%else
IMPORT cdecl, wsprintfA
%endif
%define wsprintf wsprintfA

%endif  ;// UNICODE

%endif  ;// __NASMX_DEMOS_INC__
