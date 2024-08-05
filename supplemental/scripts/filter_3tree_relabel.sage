from sys import *
from itertools import *

def is_3_tree(G):
    if len(G) == 3: 
        V3 = list(sorted(G.vertices()))
        if V3 in F:
            yield V3
    else:
        for v in G:
            if G.degree(v) == 3:
                H = copy(G)
                H.delete_vertex(v)
                for label in is_3_tree(H):
                    yield label+[v]


for l in open(argv[1]): 
    G = Graph(l)
    G.set_planar_positions()
    F = [list(sorted([u for (u,v) in f])) for f in G.faces()]
    for label in is_3_tree(G):
        H = copy(G)
        H.relabel({v:label.index(v) for v in H})
        H.set_planar_positions(set_embedding=True)
        FH = [list(sorted([u for (u,v) in f])) for f in H.faces()]
        if [0,1,2] in FH:  
            print H.graph6_string()
            break
            