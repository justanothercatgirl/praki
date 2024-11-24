
CXXFLAGS += -std=c++2c -mavx2 -Iinclude
CXXFLAGS += -ggdb

run: main 
	./main
	gnuplot -q plots.gp 2> /dev/null 

main: main.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS)

.PHONY: clean

clean:
	rm -fr plot*.data main *.png

