
CFLAGS = -std=c++2c -mavx -Iinclude -ggdb

.PHONY: all run_main clean gnuplot 

all: gnuplot

gnuplot:  run_main plots.gp
	gnuplot plots2.gp

run_main: main
	./main

main: main.cpp include/*
	$(CXX) -o $@ $< $(CFLAGS)

clean: 
	rm -fr main *.png *.plot
