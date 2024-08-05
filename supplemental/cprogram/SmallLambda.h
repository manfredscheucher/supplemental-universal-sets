/*
 * otlib - order type library
 * (c) Manfred Scheucher 2018
 */

#ifndef __SMALLLAMBDA_H_INCLUDED__
#define __SMALLLAMBDA_H_INCLUDED__


class BigLambda;

class SmallLambda 
{
private:
	int n_ = 0;
	int* l_ = 0;


public:
	SmallLambda()
	{
	}


	SmallLambda(const SmallLambda& other)
	{
		other.copyTo(this);
	}


    bool operator<(const SmallLambda& other) const
    {
		assert(n_ == other.n_);
		for(int a=0;a<n_;a++)
		{
			for(int b=0;b<n_;b++)
			{
				if(get(a,b) < other.get(a,b)) return 1;
				if(get(a,b) > other.get(a,b)) return 0;
			}
		}
        return 0;
    }


	inline int n()
	{
		return n_;
	}


	void resize(int n)
	{
		assert(0 < n and n <= MAXN);
		if(n != n_)
		{
			delete[] l_;
			l_ = new int[n*n];
			n_ = n;
		}
	}


	~SmallLambda()
	{
		delete[] l_;
	}


	inline int get(int a,int b) const
	{
		assert(0 <= a && a < n_);
		assert(0 <= b && b < n_);
		return l_[n_*a+b];
	}


	inline void set(int a,int b,int v)
	{
		assert(0 <= a && a < n_);
		assert(0 <= b && b < n_);
		l_[n_*a+b] = v;
	}


	inline std::string toString()
	{
		std::stringstream ss;
		ss << n_ << "\n";
		for(int a=0;a<n_;a++)
		{
			for(int b=0;b<n_;b++)
			{
				ss <<' '<< get(a,b);
			}
			ss << '\n';
		}
		return ss.str();
	}


	inline int getExtremals(int* extremals=0)
	{
		int ct = 0;
		int i0 = -1;
		int i=-1;
		while((i=_nextExtremal(i)) != i0)
		{
			if(i0 == -1) i0 = i;
			if(extremals!=0) extremals[ct] = i;
			ct++;
		}
		return ct;
	}


	inline void mirrorTo(SmallLambda* othr)
	{
		othr->resize(n_);
		for(int a=0;a<n_;a++)
		{
			for(int b=0;b<n_;b++)
			{
				othr->set(b,a,get(a,b));
			}
		}
	}


	inline void copyTo(SmallLambda* othr) const
	{
		othr->resize(n_);
		std::memcpy(othr->l_,l_,n_*n_*sizeof(int));
	}


	inline void relabelTo(SmallLambda* sl,int* perm)
	{
		sl->resize(n_);
		for(int i=0;i<n_;i++)
		{
			for(int j=0;j<n_;j++)
			{
				sl->set(i,j,get(perm[i],perm[j]));
			}
		}
	}


	inline void getNaturalLabeling(int* perm,int i0)
	{
		assert(n_ < MAXN);
		perm[0] = i0;
		for(int j=0;j<n_;j++)
		{
			if(i0 != j)
			{
				perm[get(i0,j)+1] = j;
			}
		}
	}


	inline void toLexMin(SmallLambda* best,int* best_perm=0)
	{
		int perm[MAXN];
		SmallLambda siht;
		SmallLambda tmp;
		this->mirrorTo(&siht);
		this->copyTo(best);
		if(best_perm) 
		{
			for(int i=0;i<n_;i++) best_perm[i]=i;
		}
		
		int extremals[MAXN];
		int k = getExtremals(extremals);
		for(int i=0;i<k;i++)
		{
			this->getNaturalLabeling(perm,extremals[i]);
			if(this->comparePermuted(perm,best,1)<0)
			{
				this->relabelTo(best,perm);
				if(best_perm) std::memcpy(best_perm,perm,n_*sizeof(int));
			} 
			
			siht.getNaturalLabeling(perm,extremals[i]);
			if(siht.comparePermuted(perm,best,1)<0) 
			{
				siht.relabelTo(best,perm);
				if(best_perm) std::memcpy(best_perm,perm,n_*sizeof(int));
			} 
		}
	}


	// returns 0 if both equal
	// returns -1 if self is smaller
	// returns +1 if othr is smaller
	inline int compare(SmallLambda* othr,bool both_natural=0)
	{	
		assert(this->n_ == othr->n_);
		for(int i=0;i<n_;i++)
		{
			if(both_natural && i == 0) continue;
			for(int j=i+1;j<n_;j++)
			{	
				int a = this->get(i,j);
				int b = othr->get(i,j);
				if(a<b) return -1;
				if(a>b) return +1;
			}
		}
		return 0;
	}


	// returns 0 if both equal
	// returns -1 if self is smaller
	// returns +1 if othr is smaller
	inline int comparePermuted(int* perm,SmallLambda* othr,bool both_natural=0)
	{
		assert(this->n_ == othr->n_);
		for(int i=0;i<n_;i++)
		{
			if(both_natural && i == 0) continue;
			for(int j=i+1;j<n_;j++)
			{	
				int a = this->get(perm[i],perm[j]);
				int b = othr->get(i,j);
				if(a<b) return -1;
				if(a>b) return +1;
			}
		}
		return 0;
	}


	void toBigLambda(BigLambda* bl);


private:
	// WARNING: only works if k is an extremal point !!!
	inline int _getTripleOrientationExtremal(int i,int j, int k_extremal)
	{
		if(i == j || i == k_extremal || j == k_extremal) return 0;
		if(get(i,k_extremal) < get(j,k_extremal))
		{
			return +1;
		}
		else if(get(i,k_extremal) > get(j,k_extremal))
		{
			return -1;
		}
		else
		{
			assert(0); // invalid data
		}
	}


	inline int _nextExtremal(int i)
	{
		if(i == -1)
		{
			for(int i=0;i<n_;i++)
			{
				for(int j=0;j<n_;j++)
				{
					if(i != j and get(i,j)==0) return i;
				}
			}
			std::cout << "ERROR@\n" << toString() << "\n";
			assert(0); // invalid data
		}
		else
		{
			for(int j=0;j<n_;j++)
			{
				if(i != j and get(i,j)==0) return j;
			}
			std::cout << "ERROR@\n" << toString() << "\n";
			assert(0); // invalid data
		}
	}
	
};

#endif
