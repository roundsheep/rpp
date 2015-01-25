#ifndef rbase_h__
#define rbase_h__

#ifdef _MSC_VER

//#define _CRT_SECURE_NO_DEPRECATE
//#pragma warning(disable:4127)
//#pragma warning(disable:4390)
//#pragma warning(disable:4710)
#pragma warning(disable:4996)
#pragma warning(disable:4345)

#include <windows.h>
#include <wchar.h>
#else
#endif

#ifndef int64
#ifdef _MSC_VER
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
typedef long long int64;
typedef unsigned long long uint64;
#endif

typedef int64 int8;

#endif

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef ushort
typedef unsigned short ushort;
#endif

#ifndef uchar
typedef unsigned char uchar;
#endif

#ifndef rbool
typedef int rbool;
#endif

#ifndef wchar
#ifdef _MSC_VER
typedef wchar_t wchar;
#else
typedef ushort wchar;
#endif
#endif

//#define NEW new(nothrow)

#ifndef null
#if _MSC_VER >= 1700 
#define null nullptr
#else
#define null NULL
#endif
#endif

#include <utility>

#define elif else if
#define ifn(a) if(!(a))

#define r_size(a) ((int)sizeof(a))
#define r_move(a) std::move(a)
#define r_max(a,b) ((a)>(b)?(a):(b))
#define r_min(a,b) ((a)>(b)?(b):(a))
#define r_cond(cond,a,b) ((cond)?(a):(b))
#define r_ceil_div(a,b) (0==(a)%(b)?(a)/(b):(a)/(b)+1)

#define r_to static_cast

#define r_to_int(a) ((int)(a))
#define r_to_uint(a) ((uint)(a))
#define r_to_char(a) ((char)(a))
#define r_to_uchar(a) ((uchar)(a))
#define r_to_int8(a) ((int8)(a))
#define r_to_double(a) ((double)(a))

#define r_to_pint(a) ((int*)(a))
#define r_to_puint(a) ((uint*)(a))
#define r_to_pchar(a) ((char*)(a))
#define r_to_puchar(a) ((uchar*)(a))
#define r_to_pint8(a) ((int8*)(a))
#define r_to_pdouble(a) ((double*)(a))
#define r_to_prstr(a) ((rstr*)(a))

#define AND(a,b) ((a)&(b))
#define OR(a,b) ((a)|(b))
#define XOR(a,b) ((a)^(b))
#define NOT(a) (~(a))

#endif
