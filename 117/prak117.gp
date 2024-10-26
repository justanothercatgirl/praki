set term png
set output 'output_117_T_a.png'
set xlabel "a(m)"
set ylabel "T(c)"
set grid
plot 'output_117_T_a.data' using 1:2 with lines title "T(a)", 		\
     'output_117_T_a.data' linecolor rgb '#FF0000' pointtype 5 notitle

set output 'output_117_2.png'
set xlabel "d(mm)"
set ylabel "T(c)"
f(x) = a*x+b
g(x) = c*x+d
fit f(x) 'output_117_T1.data' using 1:2:3 via a,b
fit g(x) 'output_117_T2.data' using 1:2:3 via c,d
plot f(x) title "T_{прям.}(d)",\
    'output_117_T1.data' using 1:2:3 pointtype 6 notitle with yerrorbars , \
     g(x) title "Т_{обр.}(d)", \
     'output_117_T2.data' using 1:2:3 pointtype 6 notitle with yerrorbars

set output 'output_117_2_zoomed.png'
set xrange[90:110]
replot

unset xrange
set xlabel "T^2⋅a, c^2⋅m"
set ylabel "a^2, m^2"
set output 'output_117_mnk.png'
h(x) = m*x+l
fit h(x) 'output_117_mnk.data' using 1:2:3 via m,l
plot h(x) title "y(x)", 'output_117_mnk.data' using 1:2:3 pointtype 6 notitle with yerrorbars
