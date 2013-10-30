

Program = Hannibal
Genproc = "assemble_tape.py"

all : $(Genproc)

$(Genproc) : $(Program).exe 
	./$(Program).exe

$(Program).exe :  $(Program).cc hexa_base.hxx
	g++ -o $(Program).exe $(Program).cc

clean :
	rm -f

