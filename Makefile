all:
	g++ -o p.out -w -fpermissive pthreads.cpp -lpthread
	# g++ -o s.out sequential.cpp 
	g++ -o op.out -fopenmp openmp.cpp
	
pthread:
	g++ -o p.out -w -fpermissive pthreads.cpp -lpthread

openmp:
	g++ -o op.out -fopenmp openmp.cpp

sequential:
	g++ -o s.out sequential.cpp 

clean:
	rm *.out
