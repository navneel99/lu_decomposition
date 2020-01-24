#!/bin/sh
rm result.txt

echo "Compiling all the codes ..." > result.txt
make all >> result.txt
echo "Compilation Completed"
echo "Compilation completed\n\n" >> result.txt

echo "Execution of pthread code starting ..." >> result.txt
for i in 1 2 4 8 16
do 
	./p.out 100 $i 1 >> result.txt
	echo "Execution of pthread code with $i threads completed"
done
echo "Execution of Pthread completed"
echo "Execution of pthread code completed\n\n" >> result.txt

echo "Execution of Openmp code starting ..." >> result.txt
for i in 1 2 4 8 16
do
	./op.out 100 $i 1 >> result.txt
	echo "Execution of Openmp code with $i threads completed"
done
echo "Execution of Openmp code completed\n\n" >> result.txt

echo "Execution of Sequential code starting ..." >> result.txt
./s.out 100 1 >> result.txt
echo "Execution of Sequential code completed" >> result.txt 

