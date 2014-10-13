#ifndef rset_h__
#define rset_h__

#include "rbase.h"

template<typename T>
struct rset_i
{
	T key;
	rset_i* father;
	rset_i* left;
	rset_i* right;
	int col;
};

template<typename T>
struct rset
{
	rset_i<T>* m_root;
	rset_i<T> m_nil;
	int m_count;

	enum
	{
		c_black,
		c_red
	};

	~rset<T>()
	{
		clear();
	}

	rset<T>()
	{
		init();
	}

	rset<T>(const rset<T>& a)
	{
		init();
		//用递归拷贝更快
		copy_x(&a.m_nil,a.m_root);
		/*for(T* p=a.begin();p!=a.end();p=a.next(p))
			insert(*p);*/
	}

	void operator=(const rset<T>& a)
	{
		clear();
		copy_x(&a.m_nil,a.m_root);
		/*for(T* p=a.begin();p!=a.end();p=a.next(p))
			insert(*p);*/
	}

	void copy_x(const rset_i<T>* xnil,rset_i<T>* x)
	{
		if(x==xnil)
		{
			return;
		}
		copy_x(xnil,x->left);
		copy_x(xnil,x->right);
		insert(x->key);
	}

	void init()
	{
		m_nil.father=&m_nil;
		m_nil.left=&m_nil;
		m_nil.right=&m_nil;
		m_nil.col=c_black;

		m_root=&m_nil;
		m_count=0;
	}

	void clear()
	{
		clear_x(m_root);
		init();
	}

	void clear_x(rset_i<T>* x)
	{
		if(x==&m_nil)
		{
			return;
		}
		clear_x(x->left);
		clear_x(x->right);
		delete x;
	}

	rbool empty() const
	{
		return m_count==0;
	}

	int count() const
	{
		return m_count;
	}

	void left_rotate(rset_i<T>* x)
	{
		rset_i<T>* y;
		if(x->right==&m_nil)
		{
			return;
		}
		y=x->right;
		x->right=y->left;
		if(y->left!=&m_nil)
		{
			y->left->father=x;
		}
		y->father=x->father;
		if(x->father==&m_nil)
		{
			m_root=y;
		}
		elif(x->father->left==x)
		{
			x->father->left=y;
		}
		else
		{
			x->father->right=y;
		}
		y->left=x;
		x->father=y;
	}

	void right_rotate(rset_i<T>* x)
	{
		rset_i<T>* y;
		if(x->left==&m_nil)
		{
			return;
		}
		y=x->left;
		x->left=y->right;
		if(y->right!=&m_nil)
		{
			y->right->father=x;
		}
		y->father=x->father;
		if(x->father==&m_nil)
		{
			m_root=y;
		}
		elif(x->father->left==x)
		{
			x->father->left=y;
		}
		else
		{
			x->father->right=y;
		}
		y->right=x;
		x->father=y;
	}

	void insert_fixup(rset_i<T>* z)
	{
		rset_i<T>* y;
		while(z->father->col==c_red) 
		{ 
			if(z->father==z->father->father->left) 
			{ 
				y=z->father->father->right; 
				if(y->col==c_red) 
				{ 
					z->father->col=c_black; 
					y->col=c_black; 
					z->father->father->col=c_red; 
					z=z->father->father; 
				} 
				else 
				{
					if(z==z->father->right) 
					{ 
						z=z->father; 
						left_rotate(z); 
					} 
					z->father->col=c_black; 
					z->father->father->col=c_red; 
					right_rotate(z->father->father); 
				}
			} 
			else 
			{ 
				y=z->father->father->left; 
				if(y->col==c_red) 
				{ 
					z->father->col=c_black; 
					y->col=c_black; 
					z->father->father->col=c_red; 
					z=z->father->father; 
				} 
				else 
				{
					if(z==z->father->left) 
					{ 
						z=z->father; 
						right_rotate(z); 
					} 
					z->father->col=c_black; 
					z->father->father->col=c_red; 
					left_rotate(z->father->father);
				}
			} 
		} 
		m_root->col=c_black; 
	}

	void insert_p(rset_i<T>* z)
	{
		rset_i<T>* y=&m_nil;
		rset_i<T>* x=m_root;
		while(x!=&m_nil)
		{
			y=x;
			if(z->key<x->key)
			{
				x=x->left;
			}
			else
			{
				x=x->right;
			}
		}
		z->father=y;
		if(y==&m_nil)
		{
			m_root=z;
		}
		elif(z->key<y->key)
		{
			y->left=z;
		}
		else
		{
			y->right=z;
		}
		z->left=&m_nil;
		z->right=&m_nil;
		z->col=c_red;
		insert_fixup(z);
	}

	void insert(const T& a)
	{
		rset_i<T>* p=new rset_i<T>;
		p->key=a;
		insert_p(p);
		m_count++;
	}

	void insert_c(const T& a)
	{
		if(exist(a))
		{
			return;
		}
		insert(a);
	}

	/*void test(rset_i<T>* z)
	{
		if(z!=&m_nil)
		{
			test(z->left);
			z->key.printl();
			test(z->right);
		}
	}*/

	void erase_fixup(rset_i<T>* x)
	{
		rset_i<T>* w;
		while(x!=m_root&&x->col==c_black)
		{
			if(x==x->father->left)
			{
				w=x->father->right;
				if(w->col==c_red)
				{
					w->col=c_black;
					x->father->col=c_red;
					left_rotate(x->father);
					w=x->father->right;
				}
				if(w->left->col==c_black&&w->right->col==c_black)
				{
					w->col=c_red;
					x=x->father;
				}
				else 
				{
					if(w->right->col==c_black)
					{
						w->left->col=c_black;
						w->col=c_red;
						right_rotate(w);
						w=x->father->right;
					}
					w->col=x->father->col;
					x->father->col=c_black;
					w->right->col=c_black;
					left_rotate(x->father);
					x=m_root;
				}
			}
			else
			{
				w=x->father->left;
				if(w->col==c_red)
				{
					w->col=c_black;
					x->father->col=c_red;
					right_rotate(x->father);
					w=x->father->left;
				}
				if(w->right->col==c_black&&w->left->col==c_black)
				{
					w->col=c_red;
					x=x->father;
				}
				else
				{
					if(w->left->col==c_black)
					{
						w->right->col=c_black;
						w->col=c_red;
						left_rotate(w);
						w=x->father->left;
					}
					w->col=x->father->col;
					x->father->col=c_black;
					w->left->col=c_black;
					right_rotate(x->father);
					x=m_root;
				}
			}
		}
		x->col=c_black;
	}

	void transplant(rset_i<T>* u,rset_i<T>* v)
	{
		if(u->father==&m_nil)
		{
			m_root=v;
		}
		elif(u==u->father->left)
		{
			u->father->left=v;
		}
		else
		{
			u->father->right=v;
		}
		v->father=u->father;
	}

	void erase_p(rset_i<T>* z)
	{
		rset_i<T>* y=z;
		rset_i<T>* x;
		int y_original_col=y->col;
		if(z->left==&m_nil)
		{
			x=z->right;
			transplant(z,z->right);
		}
		elif(z->right==&m_nil)
		{
			x=z->left;
			transplant(z,z->left);
		}
		else
		{
			y=minimum(z->right);
			y_original_col=y->col;
			x=y->right;
			if(y->father==z)
			{
				x->father=y;
			}
			else
			{
				transplant(y,y->right);
				y->right=z->right;
				y->right->father=y;
			}
			transplant(z,y);
			y->left=z->left;
			y->left->father=y;
			y->col=z->col;
		}
		if(y_original_col==c_black)
		{
			erase_fixup(x);
		}
	}

	void erase(const T& a)
	{
		rset_i<T>* x=search(m_root,a);
		if(x!=&m_nil)
		{
			erase_p(x);
			delete x;
			m_count--;
		}
	}

	bool exist(const T& a) const
	{
		return search(m_root,a)!=&m_nil;
	}

	rset_i<T>* successor(rset_i<T>* x) const
	{
		rset_i<T>* y;
		if(x->right!=&m_nil)
		{
			return minimum(x->right);
		}
		y=x->father;
		while(y!=&m_nil&&x==y->right)
		{
			x=y;
			y=y->father;
		}
		return y;
	}

	rset_i<T>* predecessor(rset_i<T>* x) const
	{
		rset_i<T>* y;
		if(x->left!=&m_nil)
		{
			return maximum(x->left);
		}
		y=x->father;
		while(y!=&m_nil&&x==y->left)
		{
			x=y;
			y=y->father;
		}
		return y;
	}

	rset_i<T>* minimum(rset_i<T>* x) const
	{
		while(x->left!=&m_nil)
		{
			x=x->left;
		}
		return x;
	}

	rset_i<T>* maximum(rset_i<T>* x) const
	{
		while(x->right!=&m_nil)
		{
			x=x->right;
		}
		return x;
	}

	rset_i<T>* search(rset_i<T>* x,const T& a) const
	{
		while(x!=&m_nil&&r_not_equal<T>(a,x->key))
		{
			if(a<x->key)
			{
				x=x->left;
			}
			else
			{
				x=x->right;
			}
		}
		return x;
	}

	T* find(const T& a) const
	{
		rset_i<T>* x=search(m_root,a);
		if(x==&m_nil)
		{
			return null;
		}
		return (T*)x;
	}

	T* next(void* p) const
	{
		rset_i<T>* x=successor((rset_i<T>*)p);
		if(x==&m_nil)
		{
			return null;
		}
		return (T*)x;
	}
	
	T* prev(void* p) const
	{
		rset_i<T>* x=predecessor((rset_i<T>*)p);
		if(x==&m_nil)
		{
			return null;
		}
		return (T*)x;
	}

	T* begin() const
	{
		rset_i<T>* x=minimum(m_root);
		if(x==&m_nil)
		{
			return null;
		}
		return (T*)x;
	}

	static T* end()
	{
		return null;
	}

	T* index(int n) const
	{
		T* p=begin();
		for(;n;n--)
		{
			if(p==null)
			{
				return null;
			}
			p=next(p);
		}
		return p;
	}

	/*T& operator[](int n)
	{
		T* p=index(n);
		return *p;
	}*/
};

#endif