/*
 * otlib - order type library
 * (c) Manfred Scheucher 2018
 */

#include "defines.h"

#include "SmallLambda.h"
#include "BigLambda.h"
#include "PointSet.h"

#include "PointSetBinaryReader.h"
#include "SmallLambdaBinaryReader.h"
#include "SmallLambdaBinaryWriter.h"

#include <core/Solver.h>
#include <core/SolverTypes.h>


// #define REALIZATIONS // comment in to read point set realizations of order types


using namespace Minisat;


int is_universal(BigLambda* bl,std::list<int2> edges)
{
	int n = bl->n();
	Solver solver;
	std::map<int2,Var> varM; // v mapsto p
	std::map<int2,Var> varA; // edge pq is active

	// init variables for mapping
	for(int v=0;v<n;v++)
	{
		for(int p=0;p<n;p++)
		{
			varM[int2(v,p)] = solver.newVar();
		}
	}

	// init variables for active edges
	for(int p=0;p<n;p++)
	{
		for(int q=p+1;q<n;q++)
		{
			varA[int2(p,q)] = solver.newVar();
		}
	}

	// bijective mappnig
	for(int v=0;v<n;v++) 
	{
		vec<Lit> clause;
		for(int p=0;p<n;p++) clause.push(mkLit(varM[int2(p,v)],true));
		solver.addClause(clause);
	}
	for(int p=0;p<n;p++) 
	{
		vec<Lit> clause;
		for(int v=0;v<n;v++) clause.push(mkLit(varM[int2(p,v)],true));
		solver.addClause(clause);
	}
	for(int p=0;p<n;p++) 
	{
		for(int v1=0;v1<n;v1++)
		{
			for(int v2=v1+1;v2<n;v2++)
			{
				solver.addClause(mkLit(varM[int2(p,v1)],false),mkLit(varM[int2(p,v2)],false));
				solver.addClause(mkLit(varM[int2(v1,p)],false),mkLit(varM[int2(v2,p)],false));
			}
		} 
	}

	for(int p=0;p<n;p++)
	{
		for(int q=p+1;q<n;q++)
		{
			for(int r=p+1;r<n;r++)
			{
				if(r == q) continue;
				for(int s=r+1;s<n;s++)
				{
					if(s == q) continue;
					if(bl->get(p,q,r) != bl->get(p,q,s) &&
						bl->get(p,r,s) != bl->get(q,r,s))
					{
						// pq and rs are crossing edges
						solver.addClause(mkLit(varA[int2(p,q)],false),mkLit(varA[int2(r,s)],false));
					}
				}
			}
		}
	}

	for(auto edge : edges)
	{
		int u = edge.first;
		int v = edge.second;
		for(int p=0;p<n;p++)
		{
			for(int q=p+1;q<n;q++)
			{
				solver.addClause(mkLit(varA[int2(p,q)],true),mkLit(varM[int2(u,p)],false),mkLit(varM[int2(v,q)],false));
				solver.addClause(mkLit(varA[int2(p,q)],true),mkLit(varM[int2(u,q)],false),mkLit(varM[int2(v,p)],false));
			}
		}
	}

	int ret = solver.solve();
	return ret;
}


//-----------------------------------------------------------------------------
inline int _compute_inner(BigLambda* bl,int a, int b,int c,int* inner)
{
	int o = bl->get(a,b,c);
	int m = 0;
	int n = bl->n();
	for(int d=0;d<n;d++)
	{
		if(d != a && d != b && d != c &&  bl->get(a,b,d) == o && bl->get(b,c,d) == o && bl->get(c,a,d) == o)
		{
			inner[m] = d;
			m++;
		}
	}
	return m;
}


//-----------------------------------------------------------------------------
int test_property1(BigLambda* bl,int extremals[MAXN])
{
	BigLambda bl_A,bl_AA,bl_B,bl_BB,bl_BBB;
	
	int extremals_A[MAXN];
	int extremals_B[MAXN];
	int extremals_BB[MAXN];
	int A[MAXN];
	int AA[MAXN];
	int B[MAXN];
	int BB[MAXN];
	int BBB[MAXN];

	int n = bl->n();


	// try to partition A+B into 1+10, 4+7, or 7+4 
	// (wlog B has two extermal points which excludes the case 10+1)
	for(int i0=0;i0<3;i0++)
	{
		int a0 = A[0] = extremals[ i0     ];
		int b0 = B[0] = extremals[(i0+1)%3];
		int b1 = B[1] = extremals[(i0+2)%3];
		
		for(int b2 = 0; b2 < n; b2++) // fix third extremal point of B
		{
			if(b2 == a0 || b2 == b0 || b2 == b1) continue;

			B[2] = b2;
			int n_A = 1;
			int n_B = 3;
			int o = bl->get(b0,b1,b2);

			for(int j=0;j<n;j++)
			{
				if(j == a0 || j == b0 || j == b1 || j == b2) continue;

				if(bl->get(b0,b1,j) == o && bl->get(b1,b2,j) == o && bl->get(b2,b0,j) == o)
				{
					B[n_B++] = j;
				}
				else
				{
					A[n_A++] = j;
				}
			}
			
			assert(n_A+n_B == n);
			if(n_A != 1 && n_A != 4 && n_A != 7) continue; // 1+10, 4+7, or 7+4
			
			if(n_A > 1) // 1, 4, or 7
			{
				assert(n_A == 4 || n_A == 7);

				// the sets A and B have to be linearly separated
				int valid = 1;
				for(int i_a1=0; i_a1<n_A; i_a1++)
				{
					int a1 = A[i_a1];
					for(int i_a2=i_a1+1; i_a2<n_A; i_a2++)
					{
						int a2 = A[i_a2];
						if(bl->get(b1,b2,a1) != bl->get(b1,b2,a2) 
							and bl->get(a1,a2,b1) != bl->get(a1,a2,b2))
						{
							valid = 0;
							break;
						}
					}
					if(!valid) break;
				}
				if(!valid) continue;


				bl->selectPoints(&bl_A,n_A,A);
				if(bl_A.getExtremals(extremals_A)!=3) continue;
				
				if(n_A > 4)
				{
					assert(n_A == 7);
					int n_AA = 0;
					for(int i=0;i<n_A;i++)
					{
						if(i != extremals_A[0] && i != extremals_A[1] && i != extremals_A[2])
						{
							AA[n_AA++] = i;
						}
					}
					assert(n_AA == n_A-3);
					bl_A.selectPoints(&bl_AA,n_AA,AA);
					if(bl_AA.getExtremals()!=3) continue;
				}
			}

			if(n_B > 4) // 4, 7, or 10 
			{
				bl->selectPoints(&bl_B,n_B,B);
				assert(bl_B.getExtremals(extremals_B)==3);
				assert(n_B == 7 || n_B == 10);

				int n_BB = 0;
				for(int i=0;i<n_B;i++) 
				{
					if(i != extremals_B[0] && i != extremals_B[1] && i != extremals_B[2])
					{
						BB[n_BB++] = i;
					}
				}
				assert(n_BB == n_B-3);
				bl_B.selectPoints(&bl_BB,n_BB,BB);
				if(bl_BB.getExtremals(extremals_BB)!=3) continue;

				if(n_B > 7)  
				{
					assert(n_B == 10);

					int n_BBB = 0;
					for(int i=0;i<n_BB;i++) 
					{
						if(i != extremals_BB[0] && i != extremals_BB[1] && i != extremals_BB[2])
						{
							BBB[n_BBB++] = i;
						}
					}
					assert(n_BBB == n_BB-3);
					bl_BB.selectPoints(&bl_BBB,n_BBB,BBB);
					if(bl_BBB.getExtremals()!=3) continue;
				}
			}

			// valid separation!
			return 1;
		}
	}
	return 0;
}


//-----------------------------------------------------------------------------
int test_property2(SmallLambda* sl)
{
	int n = sl->n();
	assert(sl->get(  0,  1) == 0);
	assert(sl->get(  1,n-1) == 0);
	assert(sl->get(n-1,  0) == 0);
	int found = 0;
	for(int i=2;i<n-1;i++)
	{
		if( (sl->get(  0,i) == 1 && sl->get(i,n-1) == 1) 
		  ||(sl->get(  1,i) == 1 && sl->get(i,  0) == 1) 
		  ||(sl->get(n-1,i) == 1 && sl->get(i,  1) == 1))
		{
			found++;
		} 		
	}
	assert(found <= 3);
	return found >= 1;
}



//-----------------------------------------------------------------------------
int main_inner(int n,std::string ot_file,std::string graph_file,int PHASE,int part,int parts,long long* valid_ct, long long* total_ct)
{
	std::cout << "["<<part<<"/" <<parts<< "] started\n";

	SmallLambda sl;
	BigLambda bl;
	
	#ifndef REALIZATIONS
		SmallLambdaBinaryReader reader(n,ot_file,1,/*compressed*/1);
	#else
		PointSetBinaryReader reader(n,ot_file,(n<9?1:2));
	#endif
	
	SmallLambdaBinaryWriter writer(n,ot_file+".bad"+std::to_string(part)+"_"+std::to_string(parts)+".bin",1,/*compressed*/1);

	std::vector<std::list<int2>> triangulations;
	std::map<std::list<int2>,long long> triangulations_success;
	{
		// load triangulations
		std::ifstream fs(graph_file);		
		
		while(1)
		{
			int u,v;
			std::list<int2> edges;
			for(int i=0;i<3*n-6;i++)
			{
				if(!(fs >> u))
				{
					assert(i==0);
					assert(fs.eof());
					break;
				};
				assert(fs >> v);
				edges.push_back(int2(u,v));
			}
			if(edges.size()==0) break;
			
			triangulations.push_back(edges);
			triangulations_success[edges] = 0;
		}
	}

	std::cout << "triangulations loaded: " << triangulations.size() << "\n";
	assert(triangulations.size() > 0);


	std::ofstream statfile;
	if(PHASE == 3)
	{
		statfile.open(ot_file+".stat"+std::to_string(part)+"_"+std::to_string(parts)+".txt");
	}

	long long current_ct = 0;

	while(reader.readNext(&sl))
	{			
		current_ct++;
		if(current_ct % parts != part) continue;
		if (*total_ct < 1024 || *total_ct % 1024 == 0) 
		{
			std::cout << "["<<part<<"/" <<parts<< "] " << "n " << n << "\tct " << *total_ct << "\tvalid_ct " << *valid_ct << "\n";
		}	
		*total_ct = *total_ct+1;

		// triangular convex hull is necessary for universal sets
		int extremals[MAXN];
		if(sl.getExtremals(extremals) != 3) continue;

		// property 2: 5-triangulation + stacked vertex in each face
		// forces existance of inner point s.t. all points lie in one triangle
		if(n==11 && !test_property2(&sl)) continue;
		

		sl.toBigLambda(&bl);
		
		// property 1: 1-3-3-3-1 separation is necessary for universal sets
		if(n==11 && !test_property1(&bl,extremals)) continue;

		int valid = 1;
		if(PHASE >= 2)
		{
			int l = triangulations.size();

			for(int t=0;t<l;t++)
			{
				if(!is_universal(&bl,triangulations[t]))
				{
					valid = 0;
					
					if(PHASE == 3)
					{
						statfile << "1";
					} 
					else // PHASE 2
					{
						// priority queue
						triangulations_success[triangulations[t]]++;
						while(t > 0 && triangulations_success[triangulations[t]] > triangulations_success[triangulations[t-1]])
						{
							auto tmp = triangulations[t];
							triangulations[t] = triangulations[t-1];
							triangulations[t-1] = tmp;
							t--;
						}
						break;
					}
				}
				else
				{
					if(PHASE == 3)
					{
						statfile << "0";
					}
				}
			}
			if(PHASE == 3)
			{
				statfile << "\n";
				//statfile.flush();
			}
		}

		if(valid) 
		{
			*valid_ct = *valid_ct+1;
			writer.writeNext(&sl);
			if(n > 11) 
			{
				std::cout << "found example!\n";
				break;
			}
		}
	}
	std::cout << "["<<part<<"/" <<parts<< "] finished\n";
}


//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	if(argc != 8)
	{
		std::cout << "usage: " << argv[0] << " [n] [order types file] [graphs file] [phase] [parts] [from part] [to part]\n"; 
		exit(0); 
	}

	int n = atoi(argv[1]); 
	std::string ot_file = argv[2];
	std::string graph_file = argv[3];
	int PHASE = atoi(argv[4]); 
	int parts = atoi(argv[5]); 
	int part0 = atoi(argv[6]);
	int part1 = atoi(argv[7]);

	assert(1 <= PHASE and PHASE <= 3);
	assert(0 <= part0 && part0 < part1 && part1 <= parts);


	std::cout << "n: " << n << "\n";

	int num_threads = part1-part0;
    std::thread* threads = new std::thread[num_threads];
    long long* valid = new long long[num_threads];
    long long* total = new long long[num_threads];
	std::cout << "starting threads: " << num_threads << "\n";

	for (int i = 0; i<num_threads; i++) 
	{
		threads[i] = std::thread(main_inner,n,ot_file,graph_file,PHASE,part0+i,parts,&valid[i],&total[i]);
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
