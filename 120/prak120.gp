set term png
set output 'prak120.png'
set errorbars linecolor black linewidth 0.5 dashtype '.'
s(x) = a*x + b
c(x) = c*x + d
d(x) = e*x + f
fit s(x) 'output_120_сталь.data' using 1:2:3 via a,b
fit c(x) 'output_120_медь.data' using 1:2:3 via c,d
fit d(x) 'output_120_дюраль.data' using 1:2:3 via e,f
set xlabel "n грузов (ед.)"
set ylabel "Delta N(n) (мм)"
plot 'output_120_сталь.data' using 1:2:3 with yerrorbars pointtype 1 linecolor 0 notitle,	 s(x) title "Сталь" , \
     'output_120_дюраль.data' using 1:2:3 with yerrorbars pointtype 1 linecolor 0 notitle,	 d(x) title "Дюраль", \
     'output_120_медь.data' using 1:2:3 with yerrorbars pointtype 1 linecolor 0 notitle,	 c(x) title "Медь"
