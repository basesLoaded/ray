source ../0parameters.sh
source ../0mix32-parameters.sh
source ../0mix31-parameters.sh
cat  $r1 $r2 $r3 $r4541 $r4542 $r4543 $r4544 $r4545 > reads.fasta
Assemble.pl reads.fasta $wordSize &> log1
print-latex.sh $ref reads.fasta.contig EULER-SR
