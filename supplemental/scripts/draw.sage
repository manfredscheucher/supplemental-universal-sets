from itertools import *
from scipy.spatial import ConvexHull
from sys import *
import xml.etree.ElementTree as ET

from scipy import optimize


def tutte_layout(G,outer_face,weights):
	V = G.vertices()
	pos = dict()
	l = len(outer_face)

	a0 = pi/l-pi/2
	for i in range(l):
		ai = a0+pi*2*i/l
		pos[outer_face[i]] = (cos(ai),sin(ai))
	
	n = len(V)
	M = zero_matrix(RR,n,n)
	b = zero_matrix(RR,n,2)

	for i in range(n):
		v = V[i]
		if v in pos:
			M[i,i] = 1
			b[i,0] = pos[v][0]
			b[i,1] = pos[v][1]
		else:
			nv = G.neighbors(v)
			s = 0
			for u in nv:
				j = V.index(u)
				wu = weights[u,v]
				s += wu
				M[i,j] = -wu
			M[i,i] = s

	sol = M.pseudoinverse()*b
	return {V[i]:sol[i] for i in range(n)}

if len(argv) < 3:
	print "usage:"
	print "\tsage",argv[0],"inputgraphfile format"
	exit()

filename = argv[1]
outformat = argv[2]
assert(outformat in ['png','pdf','ipe'])

ct = 0
drawall = 0
for l in open(filename):
	ct += 1
	G2 = Graph(l)
	n = len(G2)

	G2.set_planar_positions()

	outer_face = [0,1,2]
	l = len(outer_face)
	
	F = [list(sorted([u for (u,v) in f])) for f in G2.faces()]
	assert(outer_face in F)

	weights = dict()
	for u,v in G2.edges(labels=None):
		weights[u,v] = weights[v,u] = 1

	G2.set_pos(tutte_layout(G2,outer_face,weights))

	pos0 = G2.get_pos()
	Z0 = [pos0[i][0] for i in range(n)]+[pos0[i][1] for i in range(n)]

	def o3(pos,a,b,c):
		ax,ay = pos[a]
		bx,by = pos[b]
		cx,cy = pos[c]
		return sgn((bx-ax)*(cy-ay)-(cx-ax)*(by-ay))

	def edge_crossing(pos,(a,b),(c,d)):
		if o3(pos,a,b,c)*o3(pos,a,b,d) == 1: return False
		if o3(pos,c,d,a)*o3(pos,c,d,b) == 1: return False
		return True # collinear is considered as "crossing"... 
		# to avaid coll. points 

	def quality(Z):
		X = Z[:n]
		Y = Z[n:]

		pos = {i:[X[i],Y[i]] for i in range(n)}

		a0 = pi/l-pi/2
		for i in range(l):
			ai = a0+pi*2*i/l
			pos[outer_face[i]] = (RR(cos(ai)),RR(sin(ai))) # fix outer vertices
	
		G2.set_pos(pos)

		for e1,e2 in combinations(G2.edges(labels=0),2):
			if not set(e1)&set(e2) and edge_crossing(pos,e1,e2): 
				return 999999

		dist2 = lambda u,v: (pos[u][0]-pos[v][0])^2+(pos[u][1]-pos[v][1])^2

		err1 = 0
		for (u,v) in G2.edges(labels=None):
			err1 += log(dist2(u,v))
		

		err2 = 0
		for f in G2.faces():
			try:
				vol_f = ConvexHull([pos[v] for u,v in f]).volume
				for u,v in f:
					err2 += log(vol_f)-log(dist2(u,v))/2
			except:
				None
		
		err = err1+err2
		print "#",ct,"err",err,"\r",
		stdout.flush()
		return -err


	assert(quality(Z0) < 999999)
	sol = optimize.minimize(quality,Z0)

	Z1 = sol.x
	X1 = Z1[:n]
	Y1 = Z1[n:]
	pos1 = {i:[X1[i],Y1[i]] for i in range(n)}

	a0 = pi/l-pi/2
	for i in range(l):
		ai = a0+pi*2*i/l
		pos1[outer_face[i]] = (cos(ai),sin(ai))
	

	plotfile = str(ct)+"."+outformat

	G2.set_pos(pos1)
	G2.plot().save(plotfile,figsize=4)


	

