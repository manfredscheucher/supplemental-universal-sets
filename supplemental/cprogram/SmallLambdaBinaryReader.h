/*
 * otlib - order type library
 * (c) Manfred Scheucher 2018
 */

#ifndef __SMALLLAMBDABINARYREADER_H_INCLUDED__
#define __SMALLLAMBDABINARYREADER_H_INCLUDED__


class SmallLambdaBinaryReader
{
public:
	int n_;
	int bytes_;
	bool compressed_;
	std::string path_;
	std::ifstream fs_;


	SmallLambdaBinaryReader(int n,std::string path,int bytes=1,bool compressed=0)
	: fs_(path,std::ifstream::in|std::ifstream::binary)
	{
		n_ = n;
		bytes_ = bytes;
		path_ = path;
		compressed_ = compressed;
		assert(n > 0); 
		assert(bytes_ == 1);
	}


	int readNext(SmallLambda* sl)
	{		
		sl->resize(n_);
	
		for(int i=0;i<n_;i++)
		{
			for(int j=0;j<n_;j++)
			{
				if(compressed_ && j <= i)
				{
					if(j == i) sl->set(i,j,0);
					if(j != i) sl->set(i,j,n_-2-sl->get(j,i));
				}
				else if (compressed_ && i == 0)
				{
					sl->set(i,j,j-1);
				}
				else
				{
					unsigned int x = 0; 
										
					x = fs_.get();

					if(!fs_.good())
					{
						assert(fs_.eof());
						assert( compressed_ || (i==0 && j==1));
						assert(!compressed_ || (i==1 && j==2));
						return 0; 
					}

					assert(x < n_-1);
					sl->set(i,j,x);
				}
			}
		}

		//std::cout << "READ:\n" << sl->toString() << "\n";
		return 1;
	}
};

#endif
