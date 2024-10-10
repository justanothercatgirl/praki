f(x) = a*x+b

set term png 
set grid
set output 'T2_timer.png'
set xlabel "m, ед."
set ylabel "T^2, c^2"
fit f(x) 'outputT2_timer.data' using 1:2:3 via a,b
plot 'outputT2_timer.data' using 1:2:3 with yerrorbars title "",\
	'outputT2_timer.data' with line title "T^2(m)",\
	f(x) title "ax+b"
set output 'T2_photo.png'
set xlabel "m, ед."
set ylabel "T^2, c^2"
fit f(x) 'outputT2_photo.data' using 1:2:3 via a,b
plot 'outputT2_photo.data' using 1:2:3 with yerrorbars title "",\
	'outputT2_photo.data' with line title "T^2(m)",\
	f(x) title "ax+b"
set output 'U.png'
set xlabel "U, В"
set ylabel "w, 1/c"
set yrange[0:250]
fit f(x) 'outputU.data' using 1:2:3 via a,b
plot 'outputU.data' using 1:2:3:4 with xyerrorbars title "",\
	'outputU.data' with line title "w(U)",\
	f(x) title "ax+b"
