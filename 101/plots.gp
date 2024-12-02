set term pngcairo size 1000, 800
set tmargin at screen 0.95
set grid lw 2

f1(x) = a1*x+b1
f2(x) = a2 + b2/(c2*x + d2)
f3(x) = a3 * exp(b3*x - c3) + d3

set label "График зависимости ускорений а от количества грузов N" at graph 0.5, graph 1.025 center
set xlabel "N, ед."
set ylabel "a, м/c^2"
set xrange[0:11]
set yrange[0:1.5]
set output "ex1.png"
fit f2(x) 'ex1_exp.plot' using 1:2:3 yerr via a2, b2, c2, d2
set label "график a_{эксп} построен по мнк 'a + b/(cx + d)'" at 6, 0.18
plot 	'ex1_calc.plot' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,	\
	'ex1_calc.plot' with lines lc rgb "red" title "a_{расч}(N)",		\
	'ex1_exp.plot' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,	\
	f2(x) lc rgb "green" title "a_{эксп}(N)"

unset label
set label "график зависимости a_{расч}, a_{эксп}, отклонения a от 1/m" at graph 0.5, graph 1.025 center
set ytics nomirror
set y2tics 
unset xrange
set yrange[0.05:0.18]
set y2range[0:100]
set y2label "delta a, %"
set xlabel "1/M, кг^{-1}"
set ylabel "a, м/c^2"
fit f1(x) 'ex2_exp.plot' using 1:2 via a1, b1
set output "ex2.png"
plot 	'ex2_cal.plot'	axes x1y1 notitle lc 0 pt 1 lw 2,				\
	'ex2_cal.plot'	with lines lc rgb "red" axes x1y1 title "a_{расч}, м/с^{2}", 	\
	'ex2_exp.plot'	axes x1y1 notitle lc 0 pt 1 lw 2,				\
	f1(x)		lc rgb "green" axes x1y1 title "a_{эксп}, м/с^{2}",		\
	'ex2_dev.plot'	axes x1y2 notitle lc 0 pt 1 lw 2,				\
	'ex2_dev.plot'	with lines lc rgb "blue" axes x1y2 title "delta a, %"

unset label
set label "график зависимости a_{расч}, a_{эксп}, отклонения a от delta m" at graph 0.5, graph 1.025 center
unset xrange
set yrange[0:2.8]
set y2range[0:100]
set y2label "delta a, %"
set xlabel "delta m, кг"
set ylabel "a, м/c^2"
fit f1(x) 'ex3_exp.plot' using 1:2 via a1, b1
fit f3(x) 'ex3_dev.plot' using 1:2 via a3, b3, c3, d3
set output "ex3.png"
plot 	'ex3_cal.plot'	axes x1y1 notitle lc 0 pt 1 lw 2,				\
	'ex3_cal.plot'	with lines lc rgb "red" axes x1y1 title "a_{расч}, м/с^{2}", 	\
	'ex3_exp.plot'	axes x1y1 notitle lc 0 pt 1 lw 2,				\
	f1(x)		lc rgb "green" axes x1y1 title "a_{эксп}, м/с^{2}",		\
	'ex3_dev.plot'	axes x1y2 notitle lc 0 pt 1 lw 2,				\
	'ex3_dev.plot'	with lines lc rgb "blue" axes x1y2 title "delta a, %",		\
	f3(x) axes x1y2 lt 2 lw 1 dashtype "--" notitle

set output "ex3_mnk.png"
unset label
set label "график зависимости a_{расч}, a_{эксп}, отклонения a от delta m для МНК" at graph 0.5, graph 1.025 center
set label sprintf("C = %.4f", a1) at 0.0255, 0.9
set label sprintf("D = %.4f", b1) at 0.0255, 0.83
unset xrange
unset yrange
unset y2tics
unset y2label 
fit f1(x) 'ex3_exp.plot' via a1, b1
plot 	'ex3_exp.plot' notitle lc rgb "red" pt 1 lw 2,				\
	f1(x) with lines lc 0 title "a(dm)"

