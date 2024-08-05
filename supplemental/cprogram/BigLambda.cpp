/*
 * otlib - order type library
 * (c) Manfred Scheucher 2018
 */

#include "defines.h"
#include "BigLambda.h"
#include "SmallLambda.h"


int BigLambda::getExtremals(int* extremals)
{
    SmallLambda sl;
    toSmallLambda(&sl);
    return sl.getExtremals(extremals);
}


void BigLambda::toSmallLambda(SmallLambda* sl)
{
    sl->resize(n_);
    for(int a=0;a<n_;a++)
    {
        for(int b=0;b<n_;b++)
        {
            int l = 0;
            for(int c=0;c<n_;c++)
            {
                if(get(a,b,c) > 0) l++;
            }
            sl->set(a,b,l);
        }	
    }
}

