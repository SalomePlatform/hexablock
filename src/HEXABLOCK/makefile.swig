
module    = hexablock_swig
uf        = model
wrapped   = $(module)_wrap
wrap_src  = $(wrapped).cc
lib_ext   = so
shared    = _$(module).$(lib_ext)
opt_cpp   = -fPIC
pyvers    = 2.5   ## Saclay
pyvers    = 2.6

all : $(shared)
	@echo "  -- Librairie dynamique $(shared) a jour"

$(wrap_src) : $(module).i 
	@echo "  -- Appel de swig ..."
	swig -python -c++ -o $(wrap_src) $(module).i
####swig -python -c++ -o $(wrap_src) -module $(module) $(module).i

$(wrapped).o : $(wrap_src)
	@echo "  -- Compilation module genere par swig ..."
	g++ $(opt_cpp) -I/usr/include/python$(pyvers) -c $(wrap_src)

$(shared) : $(wrapped).o lib$(uf).a
	@echo "  -- Edition de liens librairie dynamique ..."
	g++ -shared $(wrapped).o -L. -l$(uf) -lpython$(pyvers) -o $(shared)

$(wrap_src) : Hex.hxx
$(wrap_src) : HexDocument.hxx
$(wrap_src) : HexEltBase.hxx
$(wrap_src) : HexVertex.hxx
$(wrap_src) : HexEdge.hxx
$(wrap_src) : HexQuad.hxx
$(wrap_src) : HexHexa.hxx
$(wrap_src) : HexElements.hxx
$(wrap_src) : HexCrossElements.hxx
$(wrap_src) : HexBiCylinder.hxx
