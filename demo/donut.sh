#!/bin/bash

A=0
B=0
declare -a z
declare -a b
chars=('.' ',' '-' '~' ':' ';' '=' '!' '*' '#' '$' '@')
clear
while true; do
    i=0
    while [ "$i" -lt 1760 ]; do
        b[$i]=" "
        z[$i]=0
        ((i++))
    done
    j=0
    while [ "$(echo "6.28 > $j" | bc)" -eq 1 ]; do
        i=0
        echo $j
        while [ "$(echo "6.28 > $i" | bc)" -eq 1 ]; do
            c=$(echo "s($i)" | bc -l)
            d=$(echo "c($j)" | bc -l)
            e=$(echo "s($A)" | bc -l)
            f=$(echo "s($j)" | bc -l)
            g=$(echo "c($A)" | bc -l)
            h=$(echo "$d + 2" | bc)
            D=$(echo "1 / ($c * $h * $e + $f * $g + 5)" | bc -l)
            l=$(echo "c($i)" | bc -l)
            m=$(echo "c($B)" | bc -l)
            n=$(echo "s($B)" | bc -l)
            t=$(echo "$c * $h * $g - $f * $e" | bc)
            x=$(echo "scale=0; (40 + 30 * $D * ($l * $h * $m - $t * $n)) / 1" | bc -l)
            y=$(echo "scale=0; (12 + 15 * $D * ($l * $h * $n + $t * $m)) / 1" | bc -l)
            o=$(echo "$x + 80 * $y" | bc)
            N=$(echo "scale=0; (8 * (($f * $e - $c * $d * $g) * $m - $c * $d * $e - $f * $g - $l * $d * $n)) / 1" | bc -l)
            if [ "$(($(($(($(($(echo "$y < 22" | bc) && $(echo "$y > 0" | bc))) && $(echo "$x > 0" | bc))) && $(echo "$x < 80" | bc))) && $(echo "$D > ${z[$o]}" | bc)))" -eq 1 ]; then
                z[$o]=$D
                b[$o]=${chars[$(if [ $(echo "$(echo "$N >= $((0))" | bc)") ]; then echo $N; else echo "0"; fi)]}
            fi
            i=$(echo "$i + 0.02" | bc)
        done
        j=$(echo "$j + 0.07" | bc)
    done
    clear
    k=$((0))
    while [ "$k" -lt 1760 ]; do
        if [ "$(echo "$(echo "$k % 80" | bc) > 0" | bc)" -eq 1 ]; then echo -n ${b[$k]}; else echo ""; fi
        ((k++))
    done
    A=$(echo "$A + $(echo 0.04)" | bc)
    B=$(echo "$B + $(echo 0.02)" | bc)
done

