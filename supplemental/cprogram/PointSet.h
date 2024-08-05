/*
 * otlib - order type library
 * (c) Manfred Scheucher 2018
 */

#ifndef __POINTSET_H_INCLUDED__
#define __POINTSET_H_INCLUDED__


class PointSet
{
private:
	int n_;
	int* x_;
	int* y_;

public:
	PointSet(int n=0)
	{
		n_ = n;
		x_ = 0;
		y_ = 0;
		if(n != 0) resize(n);
	}

	inline void resize(int n)
	{
		if (n != n_) 
		{
			delete[] x_;
			delete[] y_;
			x_ = new int[n];
			y_ = new int[n];
			n_ = n;
		}
	}

	inline void set(int a,int x,int y)
	{
		x_[a] = x;
		y_[a] = y;
	}

	~PointSet()
	{
		delete[] x_;
		delete[] y_;
	}

	inline std::string toString()
	{
		std::stringstream ss;
		for(int a=0;a<n_;a++)
		{
			ss << x_[a] << " " << y_[a];
			ss << '\n';
		}
		return ss.str();
	}

	inline void toBigLambda(BigLambda* bl)
	{
		bl->resize(n_);
		for(int a=0;a<n_;a++)
		{
			for(int b=0;b<n_;b++)
			{
				for(int c=0;c<n_;c++)
				{
					long o = (LONG(x_[b])-x_[a])*(LONG(y_[c])-y_[a]) - (LONG(x_[c])-x_[a])*(LONG(y_[b])-y_[a]);
					bl->set(a,b,c,SGN(o));
				}
			}	
		}
	}

	inline void toSmallLambda(SmallLambda* sl)
	{
		BigLambda bl;
		toBigLambda(&bl);
		bl.toSmallLambda(sl);
	}
};

#endif
