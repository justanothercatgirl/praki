set term pngcairo size 1000, 800
set tmargin at screen 0.95

f1(x) = a1*x+b1
fit f1(x) '.plot' using 1:2:3 yerr via a1, b1

set output ''
set label "" at graph 0.5, graph 1.025 center
set xlabel ""
set ylabel ""

plot 	'.plot' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,	\
	f1(x) title "" lc rgb "red",				\

