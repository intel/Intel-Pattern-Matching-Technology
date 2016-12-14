#/bin/bash

if [ $# -ne 2 ]
then
    echo "Usage: $0 <input.dat> <output.png>"
    exit 1
fi

gnuplot -e \
"set terminal png;\
 set title '';\
 set output '$2';\
 set xlabel 'sample number';
 set ylabel 'value of sample';
 set yrange [-32767:32768];
 plot '$1' using 1:2 with lines title 'x',\
      '' using 1:3 with lines title 'y',\
      '' using 1:4 with lines title 'z'"
