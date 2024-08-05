from sys import *
from itertools import *

for l in open(argv[1]): 
    G = Graph(l)
    G.is_planar(set_embedding=1)
    valid = 1
    for f in G.faces():
        v3 = [v for v,_ in f if G.degree(v)==3]
        if not v3: valid = 0
    if valid:
        print l,G.edges(labels=0)
        