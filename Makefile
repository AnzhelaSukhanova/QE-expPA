BUILD=$(HOME)/boolector/build
BOOLECTOR=$(HOME)/boolector/src
DEPS=$(HOME)/boolector/deps
CPPFLAGS=-I $(BOOLECTOR)
LDFLAGS=-L $(BUILD)/lib -L $(DEPS)/minisat/build/release/lib -L $(DEPS)/lingeling -L $(DEPS)/btor2tools/build -lboolector -llgl -lminisat -lbtor2parser -lpthread

all: main

main: main.o simp_tools.o btor_tools.o steps.o
	$(CXX) $^ $(LDFLAGS) -o $@
	rm -f *.o

main.o:
	$(CXX) -c src/main.c $(CPPFLAGS)
	
simp_tools.o:
	$(CXX) -c src/simp_tools.c
	
btor_tools.o:
	$(CXX) -c src/btor_tools.c $(CPPFLAGS)
	
steps.o:
	$(CXX) -c src/steps.c $(CPPFLAGS)

clean:
	rm -f main *.o
