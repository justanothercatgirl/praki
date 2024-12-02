
CFLAGS = -std=c++2c -mavx -Iinclude -ggdb

.PHONY: all run_main clean gnuplot 

run: gnuplot

gnuplot: plots.gp run_main
	gnuplot $<

run_main: main
	./main

main: main.cpp include/*
	$(CXX) -o $@ $< $(CFLAGS)

clean: 
	rm -fr main *.png *.plot
