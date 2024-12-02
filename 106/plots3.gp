# Common settings
set terminal pngcairo size 1000, 800 enhanced
set grid
set datafile separator whitespace
set xlabel "N, ед." font ",14"

# 1. Plot ex1_v.plot
set output "plot1_velocity.png"
set ylabel "v (м/с)" font ",14"
f1(x) = a1 * x + b1
fit f1(x) "ex1_v.plot" using 1:2 via a1, b1
plot \
    "ex1_v.plot" using 1:2:3 with yerrorbars linecolor rgb "blue" pointtype 7 title "v", \
    f1(x) with lines linetype 2 linecolor rgb "blue" notitle

# 2. Plot ex2_t.plot and ex2_T.plot
set output "plot2_time_force.png"
set ylabel "t (с)" font ",14"
set y2label "F (Н)" font ",14"
set y2tics
set ytics nomirror
f2(x) = a2 * x + b2
fit f2(x) "ex2_t.plot" using 1:2 via a2, b2
f3(x) = a3 * x + b3
fit f3(x) "ex2_T.plot" using 1:2 via a3, b3
plot \
    "ex2_t.plot" using 1:2:3 with yerrorbars linecolor rgb "red" pointtype 5 title "t, c", \
    f2(x) with lines linetype 2 linecolor rgb "red" notitle,\
    "ex2_T.plot" using 1:2:3 axes x1y2 with yerrorbars linecolor rgb "green" pointtype 7 title "F, Н", \
    f3(x) axes x1y2 with lines linetype 2 linecolor rgb "green" notitle

unset y2tics
unset y2label
# 3. Plot ex4_WK.plot, ex4_WP.plot, ex4_Wfull.plot
set output "plot3_energy.png"
set ylabel "Энергия (единицы энергии)" font ",14"
f4(x) = a4 * x + b4
fit f4(x) "ex4_WK.plot" using 1:2 via a4, b4
f5(x) = a5 * x + b5
fit f5(x) "ex4_WP.plot" using 1:2 via a5, b5
f6(x) = a6 * x + b6
fit f6(x) "ex4_Wfull.plot" using 1:2 via a6, b6
plot \
    "ex4_WK.plot" using 1:2:3 with yerrorbars lc rgb "red" pointtype 5 title "W_k", \
    f4(x) with lines linetype 2 linecolor rgb "green" notitle, \
    "ex4_WP.plot" using 1:2:3 with yerrorbars lc rgb "green" pointtype 5 title "W_p", \
    f5(x) with lines linetype 2 linecolor rgb "blue" notitle, \
    "ex4_Wfull.plot" using 1:2 pt 5 linecolor rgb "blue" title "W_{full}", \
    f6(x) with lines linetype 2 linecolor rgb "red" notitle


