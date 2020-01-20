compile:
	g++ -lpthread code.cpp 

pthread:
	g++ -o p.out  -fpermissive test.cpp -lpthread