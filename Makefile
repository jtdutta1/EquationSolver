CXX=g++

all: solve

# solve: 
# 	@$(CXX) -std=c++17 -Wall -Wextra -I./src -o solve src/main.cc src/utils/expression_parser.cc src/utils/argparse.cc

solve: libsolveutils.so
	@$(CXX) -std=c++17 -Wall -Wextra -I./src -o solve src/main.cc -L. -lsolveutils -fPIC
	@echo "Cleaning up"
	rm -rf libsolve*.o utils.o

libsolveutils.so: utils.o
	@$(CXX) -std=c++17 -shared -o libsolveutils.so utils.o -fPIC

utils.o: libsolveargparse.o libsolveexpression.o
	@echo "Linking object files"
	@ld -r -o utils.o libsolveargparse.o libsolveexpression.o

libsolveargparse.o: 
	@$(CXX) -std=c++17 -c -o libsolveargparse.o src/utils/argparse.cc -fPIC

libsolveexpression.o:
	@$(CXX) -std=c++17 -c -o libsolveexpression.o src/utils/expression_parser.cc -fPIC

clean:
	rm -rf a.out solve libsolve*.o libsolve*.so utils.o

run:
	LD_LIBRARY_PATH=. ./solve \(2+2\)1+3