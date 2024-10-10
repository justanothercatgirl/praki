set term png 
set output 'T2_timer.png'
set grid
set xlabel "m, ед."
set ylabel "T^2, c^2"
plot 'outputT2_timer.data' using 1:2:3 with yerrorbars title "", 'outputT2_timer.data' with line title "T^2(m)"
set term png 
set output 'T2_photo.png'
set grid
set xlabel "m, ед."
set ylabel "T^2, c^2"
plot 'outputT2_photo.data' using 1:2:3 with yerrorbars title "", 'outputT2_photo.data' with line title "T^2(m)"
set term png 
set output 'U.png'
set grid
set xlabel "U, В"
set ylabel "w, 1/c"
set yrange[0:250]
plot 'outputU.data' using 1:2:3:4 with xyerrorbars title "", 'outputU.data' with line title "w(U)"
