set term pngcairo size 1000, 800
set tmargin at screen 0.95

f1(x) = a1*x+b1
f2(x) = a2*x+b2
f3(x) = a3*x+b3

set output 'plot_mnk_lconst.png'
unset label
set label "Графики вольтамперных характеристик проводов разного диаметра" at graph 0.5, graph 1.025 center
set xlabel "U, В"
set ylabel "I, А"
set xrange[0:1]
set yrange[0:0.1]
fit f1(x) 'plot_0.5_0.00015.data' using 1:2:3 yerr via a1, b1
fit f2(x) 'plot_0.5_0.00025.data' using 1:2:3 yerr via a2, b2
fit f3(x) 'plot_0.5_4e-04.data' using 1:2:3 yerr via a3, b3
plot 											\
	'plot_0.5_0.00015.data' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,	\
	'plot_0.5_0.00025.data' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,	\
	'plot_0.5_4e-04.data' using 1:2:3   with yerrorbars notitle lc 0 pt 1 lw 2,	\
	f1(x) title "d=0.15 * 10^{-3} м" lc rgb "red",		\
	f2(x) title "d=0.25 * 10^{-3} м" lc rgb "green",	\
	f3(x) title "d=0.40 * 10^{-3} м" lc rgb "blue"

set output 'plot_mnk_dconst.png'
unset label
set label "Графики вольтамперных характеристик проводов разной длины" at graph 0.5, graph 1.025 center
set xlabel "U, В"
set ylabel "I, А"
unset xrange
set yrange[0:0.1]
fit f1(x) 'plot_0.5_0.00015.data' using 1:2:3 yerr via a1, b1
fit f2(x) 'plot_1_0.00015.data' using 1:2:3 yerr via a2, b2
fit f3(x) 'plot_1.5_0.00015.data' using 1:2:3 yerr via a3, b3
plot 											\
	'plot_0.5_0.00015.data' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,	\
	'plot_1_0.00015.data' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,	\
	'plot_1.5_0.00015.data' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,	\
	f1(x) title "L = 0.5 м" lc rgb "red",	\
	f2(x) title "L = 1.0 м" lc rgb "green",	\
	f3(x) title "L = 1.5 м" lc rgb "blue"
	
set output 'plot_rs.png'
unset label
set label "График зависимости сопротивления R от 1/S сечения" at graph 0.5, graph 1.025 center
unset xrange
unset yrange
set xlabel "S^{-1}, м^{-2}"
set ylabel "R, Ом"
fit f1(x) 'plot_r_1s.data' using 1:2:3 yerr via a1, b1
plot 	'plot_r_1s.data' using 1:2:3 with yerrorbars notitle lc rgb "red" pt 1 lw 2, \
	f1(x) title "R" lc 0

set output 'plot_rl.png'
unset label
set label "График зависимости сопротивления R от длины провода L" at graph 0.5, graph 1.025 center
unset xrange
unset yrange
set xlabel "L, м"
set ylabel "R, Ом"
fit f1(x) 'plot_r_l.data' using 1:2:3 yerr via a1, b1
plot 	'plot_r_l.data' using 1:2:3 with yerrorbars notitle lc rgb "red" pt 1 lw 2, \
	f1(x) title "R" lc 0


set output 'plot_diods.png'
unset label
set label "Вольтамперная характеристика красного и синего светодиодов" at graph 0.5, graph 1.025 center
set xlabel "U, В"
set ylabel "I, A"
set xrange[1:2]
plot	'plot_red.data' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,	\
	'plot_green.data' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,	\
	'plot_red.data' with lines lc rgb "red" title "I(U) красный",		\
	'plot_green.data' with lines lc rgb "green" title "I(U) зелёный"

