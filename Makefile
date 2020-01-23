compile:
	g++ -lpthread code.cpp 

pthread:
	g++ -o p.out  -fpermissive pthreads_dup.cpp -lpthread
