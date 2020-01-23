all:
	g++ -o p.out -fpermissive pthreads.cpp -lpthread
	g++ -o s.out sequential.cpp 
	
pthread:
	g++ -o p.out -fpermissive pthreads.cpp -lpthread
sequential:
	g++ -o s.out sequential.cpp 
