#!bin/bash 
if [ $(($1 % 2)) -eq 0 ]; then
    echo "Число $number четное"
else
    echo "Число $number нечетное"
fi