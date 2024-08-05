# 
# otlib - order type library
# (c) Manfred Scheucher 2018
#  

from sys import *

n = int(argv[1])
otfile = argv[2]
graphfile = argv[3]
PHASE = int(argv[4])
parts = int(argv[5])
blocksize = int(argv[6])
assert(blocksize <= parts)
assert(PHASE in [1,2,3])

for i in range(0,parts,blocksize):
    j = min(parts,i+blocksize)
    prefix = "_test_universal_sets_"+str(n)+"_"+str(PHASE)+"_"+str(parts)+"_"+str(i)+"-"+str(j)
    with open(prefix+".job","w") as f:     
        f.write("#!/bin/sh\n")
        f.write("#$ -cwd\n")
        f.write("#$ -N "+prefix+"\n")
        f.write("#$ -o "+prefix+".out\n")
        f.write("#$ -e "+prefix+".err\n")
        f.write("#$ -j n\n")
        f.write("#$ -pe mp "+str(blocksize)+"\n")
        f.write("#$ -l h_rt=720000\n")
        f.write("#$ -m abe\n")
        f.write("#$ -M scheuch@math.tu-berlin.de\n")
        f.write("./test_universal_sets "+str(n)+" "+otfile+" "+graphfile+" "+str(PHASE)+" "+str(parts)+" "+str(i)+" "+str(j)+"\n")

