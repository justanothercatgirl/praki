# Set common properties
set terminal pngcairo size 600,800 enhanced
set style line 1 lw 2 pt 7 lc rgb "blue"  # Line style 1
set style line 2 lw 2 pt 5 lc rgb "red"   # Line style 2
set style line 3 lw 2 pt 9 lc rgb "green" # Line style 3

# Plot 1: ex1_v.plot with yerrorbars and lines
set output "ex1_velocity.png"
set xlabel "N, количество"
set ylabel "Скорость (м/с)"
set key top left
plot "ex1_v.plot" using 1:2:3 with yerrorbars ls 1 title "Скорость", \
     "ex1_v.plot" using 1:2 with lines ls 1 notitle

# Plot 2: ex2_t.plot and ex2_T.plot with two Y axes and lines
set output "ex2_time_force.png"
set xlabel "N, количество"
set ylabel "Время (с)"
set y2label "Сила (Н)"
set y2tics
set ytics nomirror
set key top left
plot "ex2_t.plot" using 1:2:3 with yerrorbars ls 1 title "Время", \
     "ex2_t.plot" using 1:2 with lines ls 1 notitle, \
     "ex2_T.plot" using 1:2:3 axes x1y2 with yerrorbars ls 2 title "Сила", \
     "ex2_T.plot" using 1:2 axes x1y2 with lines ls 2 notitle

# Plot 3: ex4_WK.plot, ex4_WP.plot (yerrorbars and lines) and ex4_Wfull.plot (lines only)
set output "ex4_energy.png"
set xlabel "N, количество"
set ylabel "Энергия (Дж)"
set key top left
plot "ex4_WK.plot" using 1:2:3 with yerrorbars ls 1 title "Энергия Wk", \
     "ex4_WK.plot" using 1:2 with lines ls 1 notitle, \
     "ex4_WP.plot" using 1:2:3 with yerrorbars ls 2 title "Энергия Wp", \
     "ex4_WP.plot" using 1:2 with lines ls 2 notitle, \
     "ex4_Wfull.plot" using 1:2 with lines ls 3 title "Полная энергия"


