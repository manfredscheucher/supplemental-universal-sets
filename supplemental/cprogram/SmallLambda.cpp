/*
 * otlib - order type library
 * (c) Manfred Scheucher 2018
 */

#include "defines.h"
#include "SmallLambda.h"
#include "BigLambda.h"


void SmallLambda::toBigLambda(BigLambda* bl)
{
    int n = n_;
    bl->resize(n);

    for(int i=1;i<n;i++) 
    {
        // assert natural labeling
        assert(get(0,i) == i-1); // if needed, implement more general as in SmallLambda.py with natural labeling
    }
    
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			for(int k=0;k<n;k++)
			{
				bl->set(i,j,k,0);
			}
		}
	}

	SmallLambda tmpsl;
    tmpsl.resize(n);
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
            tmpsl.set(i,j,this->get(i,j)); // copy
        }
    }

    for(int k=0;k<n;k++)
    {
        int pk = k;
        for(int i=k;i<n;i++)
        {
            int pi = i;
            for(int j=i;j<n;j++)
            {
                int pj = j;
                int o = tmpsl._getTripleOrientationExtremal(pi,pj,pk);
                if(o == +1)
                {
                    tmpsl.set(pi,pj,tmpsl.get(pi,pj)-1);
                }
                else if(o == -1)
                {
                    tmpsl.set(pj,pi,tmpsl.get(pj,pi)-1);
                }
                bl->set(pi,pj,pk,+o);
                bl->set(pj,pk,pi,+o);
                bl->set(pk,pi,pj,+o);
                bl->set(pi,pk,pj,-o);
                bl->set(pj,pi,pk,-o);
                bl->set(pk,pj,pi,-o);
            }
        }
    }
}
