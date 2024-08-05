/*
 * otlib - order type library
 * (c) Manfred Scheucher 2018
 */

#ifndef __SMALLLAMBDABINARYWRITER_H_INCLUDED__
#define __SMALLLAMBDABINARYWRITER_H_INCLUDED__



class SmallLambdaBinaryWriter
{
public:
	int n_;
	int bytes_;
	bool compressed_;
	bool lazy_;
	std::string path_;
	std::ofstream fs_;


	SmallLambdaBinaryWriter(int n,std::string path,int bytes=1,bool compressed=0,bool lazy=1)
	{
		n_ = n;
		bytes_ = bytes;
		path_ = path;
		compressed_ = compressed;
		lazy_ = lazy;
		assert(n > 0); 
		assert(bytes_ == 1);
	}


	int writeNext(SmallLambda* sl)
	{
		assert(sl->n() == n_);

		if(lazy_)
		{
			lazy_ = 0;
			fs_.open(path_,std::ofstream::out|std::ofstream::binary);
		}

		sl->resize(n_);
		
		for(int i=0;i<n_;i++)
		{
			for(int j=0;j<n_;j++)
			{
				if(compressed_ && j <= i)
				{
					assert(i == j or sl->get(i,j) + sl->get(j,i) == n_-2);
					continue;
				}
				else if (compressed_ && i == 0)
				{
					(i == j) ? assert(sl->get(i,j) == 0) : assert(sl->get(i,j) == j-1);
					continue;
				}
				else
				{
					unsigned int x =  sl->get(i,j); 

					fs_.put((unsigned char) x);	
					assert(fs_.good());
				}
			}
		}

		//std::cout << "WRITE:\n" << sl->toString() << "\n";
		return 1;
	}
};

#endif
