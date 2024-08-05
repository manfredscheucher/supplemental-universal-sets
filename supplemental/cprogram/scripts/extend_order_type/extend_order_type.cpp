/*
 * otlib - order type library
 * (c) Manfred Scheucher 2018
 */

#include "defines.h"

#include "SmallLambda.h"
#include "BigLambda.h"

#include "SmallLambdaBinaryReader.h"
#include "SmallLambdaBinaryWriter.h"



//-----------------------------------------------------------------------------
int recursive_fill(int n,BigLambda* bl_ext,std::set<SmallLambda>* sl_ext_set,int i,int j)
{
	for(int o=-1;o<=+1;o+=2)
	{
		if(i == 0 and o == +1) continue;

		bl_ext->set(i,j,n,+o);
		bl_ext->set(n,i,j,+o);
		bl_ext->set(j,n,i,+o);
		bl_ext->set(n,j,i,-o);
		bl_ext->set(j,i,n,-o);
		bl_ext->set(i,n,j,-o);

		// check signotope axioms
		int valid = 1;
		int a,b,c,d=n;
		for(int k=0;k<n;k++)
		{
			if(k==i || k==j) continue;
			if(k < i)
			{
				a = k; b = i; c = j;
			}
			else
			{
				if(k < j)
				{
					a = i; b = k; c = j;
				}
				else 
				{
					a = i; b = j; c = k;
				}
			}
			int s_abc = bl_ext->get(a,b,c);
			int s_abd = bl_ext->get(a,b,d);
			int s_acd = bl_ext->get(a,c,d);
			int s_bcd = bl_ext->get(b,c,d);
			int change1 = (s_abc * s_abd == -1);
			int change2 = (s_abd * s_acd == -1);
			int change3 = (s_acd * s_bcd == -1);
			if(change1 + change2 + change3 > 1) // at most one sign change
			{
				valid = 0;
				break;
			}
		}
		if(!valid) continue;

		if(j == n-1)
		{
			if(i == n-2)
			{
				SmallLambda sl_ext;
				bl_ext->toSmallLambda(&sl_ext);

				// sort around last point
				SmallLambda sl_ext_relabeled;
				int perm[MAXN];
				sl_ext.getNaturalLabeling(perm,n); 
				assert(perm[0] == n);
				sl_ext.relabelTo(&sl_ext_relabeled,perm);

				SmallLambda sl_ext_lexmin;
				sl_ext_relabeled.toLexMin(&sl_ext_lexmin,perm);
				if(perm[0] == 0) sl_ext_set->insert(sl_ext_lexmin); 
			}
			else
			{
				recursive_fill(n,bl_ext,sl_ext_set,i+1,i+2);
			}
		}
		else
		{
			recursive_fill(n,bl_ext,sl_ext_set,i,j+1);
		}
	}

	// reset
	bl_ext->set(i,j,n,0);
	bl_ext->set(n,i,j,0);
	bl_ext->set(j,n,i,0);
	bl_ext->set(n,j,i,0);
	bl_ext->set(j,i,n,0);
	bl_ext->set(i,n,j,0);
}


//-----------------------------------------------------------------------------
int main_inner(int n,std::string ot_file,int part,int parts,long long* valid_ct, long long* total_ct)
{
	std::cout << "["<<part<<"/" <<parts<< "] started\n";

	SmallLambda sl;
	SmallLambda sl_relabeled;
	BigLambda bl;
	BigLambda bl_ext;
	
	SmallLambdaBinaryReader reader(n,ot_file,1,/*compressed*/1);
	
	SmallLambdaBinaryWriter writer(n+1,ot_file+".ext"+std::to_string(part)+"_"+std::to_string(parts)+".bin",1,/*compressed*/1);

	long long current_ct = 0;

	while(reader.readNext(&sl))
	{
		current_ct++;
		if(current_ct % parts != part) continue;
		if (*total_ct < 1024 || *total_ct % 1024 == 0) 
		{
			std::cout << "["<<part<<"/" <<parts<< "] " << "n " << n << "\tct " << *total_ct << "\textensions " << *valid_ct << "\n";
		}
		*total_ct = *total_ct+1;

		std::set<SmallLambda> sl_ext_set;
		
		int extremals[MAXN];
		int perm[MAXN];
		int k = sl.getExtremals(extremals);
		for(int i=0;i<k;i++)
		{
			sl.getNaturalLabeling(perm,extremals[i]);
			sl.relabelTo(&sl_relabeled,perm);

			sl_relabeled.toBigLambda(&bl);
			for(int j=1;j<n;j++)
			{
				for(int k=j+1;k<n;k++)
				{
					assert(bl.get(0,j,k) == -1);
				}	
			}
		
			bl_ext.resize(n+1);
			for(int i=0;i<n+1;i++)
			{
				for(int j=0;j<n+1;j++)
				{
					for(int k=0;k<n+1;k++)
					{
						int o = (i<n && j<n && k<n) ? bl.get(i,j,k) : 0;
						bl_ext.set(i,j,k,o);
					}
				}
			}
			recursive_fill(n,&bl_ext,&sl_ext_set,0,1);
		}

		for(auto sl:sl_ext_set)
		{
			*valid_ct = *valid_ct+1;
			writer.writeNext(&sl);
		}
		//std::cout << "extended #"<< current_ct << " to " << sl_ext_set.size() << "\n";
	}
	std::cout << "["<<part<<"/" <<parts<< "] finished\n";
}


//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	if(argc != 6)
	{
		std::cout << "usage: " << argv[0] << " [n] [order types file] [parts] [from part] [to part]\n"; 
		exit(0); 
	}

	int n = atoi(argv[1]); 
	std::string ot_file = argv[2];
	int parts = atoi(argv[3]); 
	int part0 = atoi(argv[4]);
	int part1 = atoi(argv[5]);

	assert(0 <= part0 && part0 < part1 && part1 <= parts);


	std::cout << "n: " << n << "\n";

	int num_threads = part1-part0;
    std::thread* threads = new std::thread[num_threads];
    long long* valid = new long long[num_threads];
    long long* total = new long long[num_threads];
	std::cout << "starting threads: " << num_threads << "\n";

	for (int i = 0; i<num_threads; i++) 
	{
		threads[i] = std::thread(main_inner,n,ot_file,part0+i,parts,&valid[i],&total[i]);
	}

	long long sum_valid = 0;
	long long sum_total = 0;
	for (int i = 0; i<num_threads; i++) 
	{
		threads[i].join();
		sum_valid += valid[i];
		sum_total += total[i];
	}
	delete[] threads;

	std::cout << "all threads done.\n";
	std::cout << "total solutions: " << sum_valid << "/" << sum_total << "\n";
}
