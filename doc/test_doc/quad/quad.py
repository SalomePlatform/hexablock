used_quads = []

for i in xrange( doc.countUsedQuad() ):
    quad_i = doc.getUsedQuad(i)
    used_quads.append(quad_i)	 
