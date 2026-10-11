echo "Column" >> result.txt
perf stat -r 10 -e L1-dcache-loads,L1-dcache-load-misses,instructions,cycles ../bin/homework1 test ../matrix.bin column &>> result.txt

echo "Row" >> result.txt
perf stat -r 10 -e L1-dcache-loads,L1-dcache-load-misses,instructions,cycles ../bin/homework1 test ../matrix.bin row &>> result.txt

echo "Row AVX2" >> result.txt
perf stat -r 10 -e L1-dcache-loads,L1-dcache-load-misses,instructions,cycles ../bin/homework1 test ../matrix.bin avx &>> result.txt