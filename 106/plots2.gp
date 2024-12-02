
# Common settings
set terminal pngcairo size 1000, 800 enhanced
set grid
set datafile separator whitespace
set key top left
set xlabel "N, количество" font ",14"

# 1. Plot ex1_v.plot
set output "plot1_velocity.png"
set ylabel "Скорость (м/с)" font ",14"
plot \
    "ex1_v.plot" using 1:2:3 with yerrorbars linecolor rgb "blue" pointtype 7 title "Скорость", \
    "ex1_v.plot" using 1:2 smooth mcspline with lines linetype 2 linecolor rgb "blue" title ""

# 2. Plot ex2_t.plot and ex2_T.plot
set output "plot2_time_force.png"
set ylabel "t (с)" font ",14"
set y2label "Сила (Н)" font ",14"
set y2tics
set ytics nomirror
plot \
    "ex2_t.plot" using 1:2:3 with yerrorbars linecolor rgb "red" pointtype 5 title "Время", \
    "ex2_t.plot" using 1:2 smooth mcspline with lines linetype 2 linecolor rgb "red" title "", \
    "ex2_T.plot" using 1:2:3 axes x1y2 with yerrorbars linecolor rgb "green" pointtype 7 title "Сила", \
    "ex2_T.plot" using 1:2 smooth mcspline axes x1y2 with lines linetype 2 linecolor rgb "green" title ""

# 3. Plot ex4_WK.plot, ex4_WP.plot, ex4_Wfull.plot
set output "plot3_energy.png"
set ylabel "Энергия (единицы энергии)" font ",14"
plot \
    "ex4_WK.plot" using 1:2:3 with yerrorbars linecolor rgb "magenta" pointtype 7 title "Wk", \
    "ex4_WK.plot" using 1:2 smooth mcspline with lines linetype 2 linecolor rgb "magenta" title "", \
    "ex4_WP.plot" using 1:2:3 with yerrorbars linecolor rgb "cyan" pointtype 5 title "Wp", \
    "ex4_WP.plot" using 1:2 smooth mcspline with lines linetype 2 linecolor rgb "cyan" title "", \
    "ex4_Wfull.plot" using 1:2 with lines linecolor rgb "black" linetype 3 linewidth 1 title "Wfull"

