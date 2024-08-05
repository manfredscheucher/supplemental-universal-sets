from sys import *
from itertools import *

for l in open(argv[1]): 
    G = Graph(l)
    G.is_planar(set_embedding=1)
    valid = 0
    for a in G:
    	for b,c in combinations(G.neighbors(a),2):
    		if a<b and a<c and G.has_edge(b,c): # abc is triangle
    			H = copy(G)
    			H.delete_vertex(a)
    			H.delete_vertex(b)
    			H.delete_vertex(c)
    			S = H.connected_components_subgraphs()
    			if len(S) == 2 and S[0].is_isomorphic(graphs.CompleteGraph(4)) and S[1].is_isomorphic(graphs.CompleteGraph(4)):
    				valid = 1
    				break
    if valid: print l,
