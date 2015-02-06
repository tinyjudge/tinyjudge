CXX=g++ -O2 -c
LD=g++

HEADER=$(shell ls *.h)
SRC=$(shell ls *.cc)
OBJ=$(patsubst %.cc,%.o,$(SRC)) 

tjudge:$(OBJ)
	$(LD) -o $@ $^ 

%.o:%.cc $(HEADER)
	$(CXX) -o $@ $<

clean:
	rm *.o tjudge*
