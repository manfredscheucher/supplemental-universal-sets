from sys import *

from gurobipy import *

mip = Model("mip1")
#mip.setParam("TimeLimit", 300)
V = {}

def getvar(x):
    if x not in V:
        V[x] = mip.addVar(vtype=GRB.BINARY, name='x'+str(x))
    return V[x]

ct = 0
for l in open(argv[1]):
    ct += 1
    print "load #",ct,"\r",
    stdout.flush()
    assert(l[-1] == "\n")
    n = len(l)-1
    mip.addConstr(sum(getvar(i) for i in range(n) if l[i] == '1') >= 1,"c"+str(ct))

mip.setObjective(sum(V.values()),GRB.MINIMIZE)
mip.write(argv[1]+".instance.lp")

print "start solving"
mip.optimize()

print('minimum cover: %g' % mip.objVal)

for v in mip.getVars():
    if round(v.x) == 1:
        print v.varName[1:]+",",
print
