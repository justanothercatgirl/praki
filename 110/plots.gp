set term pngcairo size 1000, 800
set title "МНК t от (x_4 - x_0)^{1/2}"
set output "mnk.png"

f(x) = a1*x+b1
fit f(x) 'mnk.plot' using 1:2:3 yerror via a1, b1

set xlabel "(x_4 - x_0)^{1/2}, м^{1/2}"
set ylabel "t, c"

plot 	'mnk.plot' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,\
	f(x) title "t" lc rgb "#a889e7" lw 2

reset

set output 'compar.png'
set title "Расположение J_i на оси J (пунктир: погрешность с коэффициентом доверия a=75%)"

set multiplot layout 4,1

set linetype 10 dashtype (5, 10)

unset ytics
unset ylabel
unset xlabel

set xrange[0.012:0.022]
set yrange[-1:1]
plot 	'ex2_1.plot' every ::0::0 using 1:2:3 with xerrorbars title "J_1" lc 0 pt 1 lw 2,\
	'ex2_2.plot' every ::0::0 using 1:2:3 with xerrorbars title "J_{1_a}" lt 10 lc 0 pt 1 lw 2 
unset title
plot 	'ex2_1.plot' every ::1::1 using 1:2:3 with xerrorbars title "J_2" lc 0 pt 1 lw 2,\
	'ex2_2.plot' every ::1::1 using 1:2:3 with xerrorbars title "J_{2_a}" lt 10 lc 0 pt 1 lw 2 
plot 	'ex2_1.plot' every ::2::2 using 1:2:3 with xerrorbars title "J_3" lc 0 pt 1 lw 2,\
	'ex2_2.plot' every ::2::2 using 1:2:3 with xerrorbars title "J_{3_a}" lt 10 lc 0 pt 1 lw 2 
set xlabel "J, кг*м^2"

plot 	'ex2_1.plot' every ::3::3 using 1:2:3 with xerrorbars title "J_4" lc 0 pt 1 lw 2,\
	'ex2_2.plot' every ::3::3 using 1:2:3 with xerrorbars title "J_{4_a}" lt 10 lc 0 pt 1 lw 2 

