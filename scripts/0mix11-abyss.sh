source ../0mix11-parameters.sh
source ../0parameters.sh
time ABYSS -k$wordSize $p1left $p1right $p2left $p2right -o abyss-contigs > log
print-latex.sh $mg1655 abyss-contigs ABySS

