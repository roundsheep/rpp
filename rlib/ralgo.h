#ifndef ralgo_h__
#define ralgo_h__

#include "rbuf.h"

//binary search
template <class V, class T>
int r_find_pos_b(const V& v,const T& a)
{
	int left=0;
	int right=v.count()-1;
	int mid;
	while(left<=right)
	{
		mid=(left+right)/2;
		if(v[mid]<a)
			left=mid+1;
		elif(a<v[mid])
			right=mid-1;
		else
			return mid;
	}
	return v.count();
}

template<class T>
rbool r_not_equal(const T& a,const T& b)
{
	return a<b||b<a;
}

template<class T>
void r_swap(T& a,T& b)
{
	T temp(r_move(a));
	a=r_move(b);
	b=r_move(temp);
}

template <class V>
void r_reverse(V& v,int begin=0,int end=0)
{
	if(end<=0)
	{
		end=v.count();
	}
	if(begin<0)
	{
		begin=0;
	}
	V temp=v.sub(begin,end);
	int i=end-1;
	int j=0;
	for(;i>=begin;--i,++j)
		v[i]=temp[j];
}

template <class V,class T>
rbool r_is_all_element(const V& v,const T& a)
{
	for(int i=0;i<v.count();i++)
		if(v[i]!=a)
			return false;
	return true;
}

template <class T>
void r_qsort_in(rbuf<T>& v,int left,int right)
{
	if(left<right)
	{
		T pivot=v[right];
		int i=left-1;
		for(int j=left;j<right;j++)
		{
			if(v[j]<=pivot)
			{
				i++;
				r_swap<T>(v[i],v[j]);
			}
		}
		i++;
		r_swap<T>(v[i],v[right]);
		r_qsort_in<T>(v,left,i-1);
		r_qsort_in<T>(v,i+1,right);
	}
}

template <class T>
void r_qsort(rbuf<T>& v,int begin=0,int end=0)
{
	if(end<=0)
	{
		end=v.count();
	}
	if(begin<0)
	{
		begin=0;
	}
	if(begin+1<end)
		r_qsort_in(v,begin,end-1);
}

template <class V,class T>
T* r_find(const V& v,const T& a)
{
	T* p;
	for(p=v.begin();p!=v.end();p=v.next(p))
		if(*p==a)
			return p;
	return null;
}

template <class V,class T>
int r_find_pos(const V& v,T a,int start=0)
{
	T* p;
	for(p=v.begin()+start;p!=v.end();p=v.next(p))
		if(*p==a)
			return v.pos(p);
	return v.count();
}

template <class V,class T>
int r_find_pos_rev(const V& v,const T& a)
{
	for(int i=v.count()-1;i>=0;--i)
		if(v[i]==a)
			return i;
	return v.count();
}

template <class V>
int r_find_pos(const V& v,const V& m,int start=0)
{
	if(m.empty()||start>=v.count())
	{
		return v.count();
	}
	for(int i=start;i<v.count()-m.count()+1;i++)
	{
		if(v[i]!=m[0])
		{
			continue;
		}
		int j;
		for(j=0;j<m.count();j++)
		{
			if(v[i+j]!=m[j])
			{
				break;
			}
		}
		if(j>=m.count())
		{
			return i;
		}
	}
	return v.count();
}

template <class V>
rbuf<V> r_split(const V& v,const V& m,int start=0)
{
	rbuf<V> result;
	V temp;
	int i;
	while(start<v.count())
	{
		int pos=r_find_pos(v,m,start);
		if(pos>=v.count())
			break;
		temp.clear();
		for(i=start;i<pos;i++)
			temp+=v[i];
		if(!temp.empty())
			result.push(temp);
		start=pos+m.count();
	}
	temp.clear();
	for(i=start;i<v.count();i++)
		temp+=v[i];
	if(!temp.empty())
		result.push_move(temp);
	return r_move(result);
}

template <class V,class T>
rbuf<V> r_split(const V& v,const T& a,int start=0)
{
	V m(1,a);
	return r_split(v,m,start);
}

//允许空元素
template <class V>
rbuf<V> r_split_e(const V& v,const V& m,int start=0)
{
	rbuf<V> result;
	V temp;
	int i;
	while(start<v.count())
	{
		int pos=r_find_pos(v,m,start);
		if(pos>=v.count())
			break;
		temp.clear();
		for(i=start;i<pos;i++)
			temp+=v[i];
		result.push(temp);
		start=pos+m.count();
	}
	temp.clear();
	for(i=start;i<v.count();i++)
		temp+=v[i];
	result.push(temp);
	return r_move(result);
}

template <class V,class T>
rbuf<V> r_split_e(const V& v,const T& a,int start=0)
{
	V m(1,a);
	return r_split_e(v,m,start);
}

#endif
