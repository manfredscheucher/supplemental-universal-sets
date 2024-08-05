#!/usr/bin/python
"""
	This program can be used to find universal point sets
	(c) 2018 Manfred Scheucher 
"""

from itertools import *
from sys import argv

if len(argv) == 1:
	print "This program can be used to generate a CNF"
	print "to find a universal point set."
	print
	print "\tpython",argv[0],"[n] [graph file]"
	print
	exit()

n = int(argv[1])
N = range(n)
N2 = list(combinations(N,2))


triangulations = []
graph_file = argv[2]
for l in open(graph_file):
	G = Graph(l)
	assert(len(G) == n)
	triangulations.append(G)
	

lenT = len(triangulations)
print "loaded triangulations:",len(triangulations)


all_variables = []
all_variables += [('trip',I) for I in permutations(N,3)]
all_variables += [('extr_ab',I) for I in combinations(N,4)]
all_variables += [('extr_cd',I) for I in combinations(N,4)]
all_variables += [('convpos',I) for I in combinations(N,4)]
all_variables += [('ab_sep_cd',I) for I in permutations(N,4)]
all_variables += [('ab_cross_cd',I) for I in permutations(N,4)]
all_variables += [('map',(i,p,v)) for i in range(lenT) for p in N for v in N]
all_variables += [('active',(i,p,q)) for i in range(lenT) for p,q in N2]


all_variables_index = {}

_num_vars = 0
for v in all_variables:
	all_variables_index[v] = _num_vars
	_num_vars += 1

def var(L):	return int(1+all_variables_index[L])

def var_trip(*L): return var(('trip',L))
def var_extr_ab(*L): return var(('extr_ab',L))
def var_extr_cd(*L): return var(('extr_cd',L))
def var_convpos(*L): return var(('convpos',L))
def var_ab_sep_cd(*L): return var(('ab_sep_cd',L))
def var_ab_cross_cd(*L): return var(('ab_cross_cd',L))
def var_map(*L): return var(('map',L))
def var_active(*L): return var(('active',L))


constraints = []


print "(0) triangular convex hull",len(constraints)
for p in range(2,n-1):
	constraints.append([+var_trip(0,1,p)])
	constraints.append([-var_trip(1,p,n-1)])
	constraints.append([+var_trip(0,p,n-1)])


print "(0) sorted around first point",len(constraints)
for I3 in combinations(N,3):
	if 0 in I3:
		constraints.append([var_trip(*I3)]) # points are ordered around p_0 without loss of generality


print "(1) Alternating axioms",len(constraints)
for a,b,c in combinations(N,3):
	for sgn in [int(+1),int(-1)]:
		constraints.append([sgn*var_trip(a,b,c),-sgn*var_trip(b,c,a)])
		constraints.append([sgn*var_trip(a,b,c),-sgn*var_trip(c,a,b)])
		constraints.append([sgn*var_trip(a,b,c),+sgn*var_trip(c,b,a)])
		constraints.append([sgn*var_trip(a,b,c),+sgn*var_trip(b,a,c)])
		constraints.append([sgn*var_trip(a,b,c),+sgn*var_trip(a,c,b)])


print "(2) Signotope Axioms",len(constraints)
# signature functions: forbid invalid configuartions 
for I4 in combinations(N,4):
	I4_triples = list(combinations(I4,3))
	for t1,t2,t3 in combinations(I4_triples,3): # lex. ord. triples t1 < t2 < t3
		for sgn in [int(+1),int(-1)]:
			constraints.append([sgn*var_trip(*t1),-sgn*var_trip(*t2),sgn*var_trip(*t3)])


print "(3) Assert Separation Variables",len(constraints)
for I in permutations(N,4):
	[a,b,c,d] = I
	constraints.append([-var_ab_sep_cd(*I),+var_trip(a,b,c),+var_trip(a,b,d)])
	constraints.append([-var_ab_sep_cd(*I),-var_trip(a,b,c),-var_trip(a,b,d)])

	constraints.append([+var_ab_sep_cd(*I),+var_trip(a,b,c),-var_trip(a,b,d)])
	constraints.append([+var_ab_sep_cd(*I),-var_trip(a,b,c),+var_trip(a,b,d)])


print "(3) Assert Crossing Variables",len(constraints)
for I in permutations(N,4):
	[a,b,c,d] = I
	constraints.append([-var_ab_cross_cd(*I),+var_ab_sep_cd(a,b,c,d)])
	constraints.append([-var_ab_cross_cd(*I),+var_ab_sep_cd(c,d,a,b)])

	constraints.append([+var_ab_cross_cd(*I),-var_ab_sep_cd(a,b,c,d),-var_ab_sep_cd(c,d,a,b)])
	

print "(4) assert mappings",len(constraints)
for i in range(lenT):
	for v in N:
		constraints.append([var_map(i,p,v) for p in N])
		for p,q in combinations(N,2):
			constraints.append([-var_map(i,p,v),-var_map(i,q,v)])

	for p in N:
		constraints.append([var_map(i,p,v) for v in N])
		for v,w in combinations(N,2):
			constraints.append([-var_map(i,p,v),-var_map(i,p,w)])


print "(5) active edges",len(constraints)
for i in range(lenT):
	T = triangulations[i]
	for v,w in permutations(N,2):
		if T.has_edge(v,w):
			for p,q in N2:
				constraints.append([-var_map(i,p,v),-var_map(i,q,w),var_active(i,p,q)])
	

print "(6) forbid crossings",len(constraints)
for i in range(lenT):
	for (p,q),(r,s) in combinations(N2,2):
		if len({p,q,r,s}) == 4:
			constraints.append([-var_active(i,p,q),-var_active(i,r,s),-var_ab_cross_cd(p,q,r,s)])



print "Total number of constraints:",len(constraints)


fp = "instance"+str(n)+"_"+graph_file+".in"
f = open(fp,"w")
f.write("p cnf "+str(_num_vars)+" "+str(len(constraints))+"\n")
for c in constraints:
	f.write(" ".join(str(v) for v in c)+" 0\n")
f.close()

print "Created CNF-file:",fp


import pycosat
sol = pycosat.solve(constraints)

if sol == "UNSAT":
	print "NO SOLUTION"

else:
	chi = ''.join(("+" if var_trip(a,b,c) in sol else "-") for (a,b,c) in combinations(N,3))
	print "chi",chi

	print "mappings:"
	for i in range(lenT):
		print "\ti",i
		for v in N:
			for p in N:
				if var_map(i,v,p) in sol:
					print "\t\t",v,"->",p

