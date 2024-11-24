set term png

set xrange[0:25]
set yrange[0:10]
set xlabel "b, см"
set ylabel "a, см"
set tmargin 1.6

set label "разброс точек и коэффициентов трения скольжения для блока 1" at graph 0.5, graph 1.02 center
set output 'data1.png'
plot 'data_1' using 1:2:3:4 with xyerrorbars pt 7 lc rgb "red" title "Блок 1", \
	0.29*x lc 1 notitle, \
	0.3*x lc 1 notitle, \
	0.29*x lc 1 notitle, \
	0.27*x lc 1 notitle, \
	0.27*x lc 1 notitle, \
	0.34*x lc 1 notitle

unset label
set label "разброс точек и коэффициентов трения скольжения для блока 2" at graph 0.5, graph 1.02 center
set output 'data2.png'
plot 'data_2' using 1:2:3:4 with xyerrorbars pt 7 lc rgb "blue" title "Блок 2", \
	0.31*x lc 2 notitle, \
	0.32*x lc 2 notitle, \
	0.32*x lc 2 notitle, \
	0.31*x lc 2 notitle, \
	0.30*x lc 2 notitle, \
	0.30*x lc 2 notitle



