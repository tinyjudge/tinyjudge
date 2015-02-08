CXX=g++ -O2

HEADER=$(shell ls *.h)
SRC=$(shell ls *.cc)

tjudge:$(SRC) $(HEADER)
	$(CXX) -o $@ $(SRC)

clean:
	rm tjudge*
