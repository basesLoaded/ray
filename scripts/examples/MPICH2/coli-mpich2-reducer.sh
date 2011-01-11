mpirun -np 31 ~/Ray/trunk/code/Ray \
-p /data/users/boiseb01/sra/SRA001125/sdata/SRR001665_1.fastq \
   /data/users/boiseb01/sra/SRA001125/sdata/SRR001665_2.fastq \
-p /data/users/boiseb01/sra/SRA001125/sdata/SRR001666_1.fastq \
   /data/users/boiseb01/sra/SRA001125/sdata/SRR001666_2.fastq \
-o ecoli2 -r

~/Ray/trunk/scripts/print-latex.sh ~/nuccore/Ecoli-k12-mg1655.fasta ecoli2.fasta 
