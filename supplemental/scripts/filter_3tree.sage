from sys import *
from itertools import *

def is_3_tree(G):
    if len(G) == 3: 
        return True
    else:
        for v in G:
            if G.degree(v) == 3:
                H = copy(G)
                H.delete_vertex(v)
                if is_3_tree(H):
                    return True
        return False

for l in open(argv[1]): 
    G = Graph(l)
    if is_3_tree(G):
        print l,
