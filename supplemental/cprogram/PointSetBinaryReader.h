/*
 * otlib - order type library
 * (c) Manfred Scheucher 2018
 */

#ifndef __POINTSETBINARYREADER_H_INCLUDED__
#define __POINTSETBINARYREADER_H_INCLUDED__


#include "PointSet.h"
#include "SmallLambda.h"


class PointSetBinaryReader
{
public:
	int n_;
	int bytes_;
	std::string path_;
	std::ifstream fs_;

	PointSetBinaryReader(int n,std::string path,int bytes)
	: fs_(path,std::ifstream::in|std::ifstream::binary)
	{
		n_ = n;
		bytes_ = bytes;
		path_ = path;
		assert(n > 0); 
		assert(bytes == 1 || bytes == 2);
	}

	int readNext(PointSet* ps)
	{		
		ps->resize(n_);
		
		unsigned int x,y;
		unsigned char byte1;
		unsigned char byte2;
	

		for(int a=0;a<n_;a++)
		{
			if(bytes_ == 1)
			{
				fs_ >> std::noskipws >> byte1;
				x = byte1;
				fs_ >> std::noskipws >> byte1;
				y = byte1;
			}
			if(bytes_ == 2)
			{
				fs_ >> std::noskipws >> byte1 >> byte2;
				x = byte1 + 256*byte2;
				fs_ >> std::noskipws >> byte1 >> byte2;
				y = byte1 + 256*byte2;
			}
			if(!fs_.good())
			{
				assert(a==0 && fs_.eof());
				return 0; 
			}				
			ps->set(a,x,y);
		}	
		//std::cout << ps->toString() << "\n";

		return 1;
	}

	int readNext(SmallLambda* sl)
	{		
		PointSet ps;
		if(!readNext(&ps)) return 0;
		ps.toSmallLambda(sl);
		return 1;
	}
};


#endif
