/*
 * otlib - order type library
 * (c) Manfred Scheucher 2018
 */

#ifndef __BIGLAMBDA_H_INCLUDED__
#define __BIGLAMBDA_H_INCLUDED__


class SmallLambda;


class BigLambda 
{
private:
	int n_ = 0;
	int* o_ = 0;

public:
	BigLambda()
	{
	}

	inline int n()
	{
		return n_;
	}

	inline void resize(int n)
	{
		assert(0 < n and n <= MAXN);
		if(n != n_)
		{
			delete[] o_;
			o_ = new int[n*n*n];
			n_ = n;
		}
	}

	~BigLambda()
	{
		delete[] o_;
	}

	inline int get(int a,int b,int c)
	{
		assert(0 <= a && a < n_);
		assert(0 <= b && b < n_);
		assert(0 <= c && c < n_);
		return o_[n_*(n_*a+b)+c];
	}

	inline void set(int a,int b,int c,int v)
	{
		assert(0 <= a && a < n_);
		assert(0 <= b && b < n_);
		assert(0 <= c && c < n_);
		o_[n_*(n_*a+b)+c] = v;
	}

	inline std::string toString()
	{
		std::stringstream ss;
		for(int a=0;a<n_;a++)
		{
			for(int b=0;b<a;b++)
			{
				for(int c=0;c<b;c++)
				{
					if (c > 0) ss << " ";
					ss << get(a,b,c);
				}
				ss << '\n';
			}
		}
		return ss.str();
	}

	void toSmallLambda(SmallLambda* sl);

	inline void selectPoints(BigLambda* bl, int m, int* selection)
	{
		assert(1 <= m && m <= n_);
		bl->resize(m);
		for(int i=0;i<m;i++)
		{
			int pi=selection[i];
			assert(pi >= 0 && pi < n_);
			
			for(int j=0;j<m;j++)
			{
				int pj=selection[j];
				for(int k=0;k<m;k++)
				{
					int pk=selection[k];
					bl->set(i,j,k,this->get(pi,pj,pk));
				}
			}
		}
	}

	int getExtremals(int* extremals=0);
};

#endif
