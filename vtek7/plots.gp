set term pngcairo size 1000, 800

f1(x) = a1 * x + b1
fit f1(x) 'ex1_EL.plot' yerr via a1, b1

f21(x) = a21 * x + b21
f22(x) = a22*log(x - b22) + c22
fit f21(x) 'ex2_IE.plot' yerr via a21, b21
fit f22(x) 'ex2_UE.plot' via a22, b22, c22

#f31(x) = a31*(x + c31) + exp(b31*(x + c31))
#f32(x) = a32*(x + c32) + exp(b32*(x + c32))
#fit f31(x) 'ex3_1.plot' xyerr via a31, b31, c31
#fit f32(x) 'ex3_2.plot' xyerr via a32, b32, c32

f41(x) = a41*log(x - b41) + c41
f42(x) = a42*x + b42
fit f41(x) 'ex4_UE.plot' yerr via a41, b41, c41
fit f42(x) 'ex4_IE_1.plot' yerr via a42, b42

#f61(x) = log(a61*x + b61) + c61
#f62(x) = x/a62 * exp(-x/a62 + b62)
#fit f61(x) 'ex6_UI.plot' xyerr via a61, b61, c61
#fit f62(x) 'ex6_nuR.plot' xyerr via a62, b62

set linetype 10 dashtype (15, 15) lw 1 lc 0

set linetype 1 lc rgb "red"
set linetype 2 lc rgb "green"
set linetype 3 lc rgb "blue"


set output 'ex1.png'
set title "Зависимость освещённости E от расстояния до источника L^{-2}"
set xlabel "L^{-2}, м^{-2}"
set ylabel "E, лк"
set xrange[0:110]
set yrange[0:3500]

plot 	'ex1_EL.plot' using 1:2:3 with yerrorbars notitle lc 0 pt 1 lw 2,	\
	f1(x) title "МНК" lc rgb "red"


reset 

set output 'ex2.png'
set title "Зависимосьть I, U фотоида от освещённости E"
set y2tics
set xlabel "E, лк"
set ylabel "U, В"
set y2label "I, А"
set xrange[0:3500]


plot	'ex2_IE.plot' axis x1y2 with lines title "I(E)",\
	'ex2_UE.plot' axis x1y1 with lines title "U(E)",\
	'ex2_IE.plot' axis x1y2 with yerrorbars notitle,\
	'ex2_UE.plot' axis x1y1 with yerrorbars notitle,\
	f21(x) axis x1y2 lt 10 notitle, f22(x) axis x1y1 lt 10 notitle

reset

set output 'ex3.png'
set term pngcairo size 1272, 900
set title "ВАХ фотоида при разных значениях освещённости"
set xlabel "U, В"
set ylabel "I, А"
set xrange[-11:0.6]

plot	'ex3_1.plot' with lines title "I(U) с засветкой",\
	'ex3_2.plot' with lines title "I(U) без засветки",\
	'ex3_2.plot' using 1:2:3:4 with xyerrorbars notitle, \
	'ex3_1.plot' using 1:2:3:4 with xyerrorbars notitle,\
#	f31(x) notitle lt 10, f32(x) notitle lt 10

set title "ВАХ фотоида при разных значениях освещённости (Отрицательная ветка)"
set output 'ex3_left.png'
set yrange[-0.00000255:0.0000025]

replot

set title "ВАХ фотоида при разных значениях освещённости (Положительная ветка)"
set output 'ex3_right.png'
unset yrange
set xrange[0:0.5]
replot

reset
set term pngcairo size 1000, 800
set output 'ex4.png'

set title "Зависимосьть I, U фотоида от освещённости E"
set y2tics
set xlabel "E, лк"
set ylabel "U, В"
set y2label "I, А"
set xrange[0:3500]

plot 	'ex4_UE.plot' axis x1y1 with lines title "U(E)",\
	'ex4_IE_1.plot' axis x1y2 with lines title "I_1(E)",\
	'ex4_IE_2.plot' axis x1y2 with lines title "I_2(E)",\
	'ex4_IE_1.plot' axis x1y2 with yerrorbars notitle,\
        'ex4_IE_2.plot' axis x1y2 with yerrorbars notitle,\
	'ex4_UE.plot' axis x1y1 with yerrorbars notitle,\
	f41(x) lt 10 notitle , f42(x) axis x1y2 lt 10 notitle

reset

set output 'ex6_1.png'
set title "ВАХ Солнечной батареи"
set xlabel "U, В"
set ylabel "I, А"

plot 	'ex6_UI.plot' with lines title "I(U)",\
	'ex6_UI.plot' notitle with xyerrorbars lt 3,\
#	f61(x) lt 10 notitle

set output 'ex6_2.png'
set title "Зависимость КПД Nu от сопротивления R"
set xlabel "Nu, ед."
set ylabel "R, кОм"
plot	'ex6_nuR.plot' with lines title "Nu(R)",\
	'ex6_nuR.plot' notitle lt 3,\
#	f62(x) notitle lt 10
