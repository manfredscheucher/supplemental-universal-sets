from sys import *

for l in open(argv[1]): 
    G=Graph(l)
    assert(len(G.edges(labels=False)) == 3*len(G)-6)
    for (u,v) in G.edges(labels=False):
        print u,v,
    print
