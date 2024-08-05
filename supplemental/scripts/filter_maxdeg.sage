from sys import *
from itertools import *

for l in open(argv[1]): 
    G=Graph(l)
    if max(G.degree()) == len(G)-1:
        print l,
