#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>
#include <time.h>
#include <chrono>
#include <random>

using namespace std::chrono;
using namespace std;


//struct to help in providing arguments to the pthread_create function
struct arguments{
    int a1;
    int a2;
};

//Global variables
int NUM_THREADS;
int matrix_size;
int n;

double **a;
double **l;
double **u;
double **pp;
double **aa;
int *p;

//Variable to return the L2 Norm as specified by the user
bool v = false;

string name = "A_1000.txt";

//Helper Function, to help in debugging
void print_vector(double** v ){
    for (int i = 0; i < matrix_size; i++){
        for (int j = 0; j< matrix_size; j++){
            cout << v[i][j]<<" ";
        }
        cout<<endl;
    }
}
//Helper Function to get the L2,1 Norm
double l2norm(double** q1, double** q2){
    double ans = 0;

    for(int j = 0; j < n; j++){
        double sum = 0;
        for (int i = 0; i < n ;i++){
            sum = sum + (q1[i][j] - q2[i][j])*(q1[i][j] - q2[i][j]);

        }
        ans = ans + sqrt(sum);
    }
    return ans;
}

//Matrix Multiplication Function
double **matmul(double **q1, double **q2){
    double **m;
    m = (double**)malloc(n*sizeof(double*));
    for(int i = 0; i<n; i++){
        m[i] = (double*)malloc(n*sizeof(double));
    }   
    for(int i = 0; i < n; i++){
        for (int j = 0; j < n ;j++){
            double s=0;
            for (int k = 0; k< n;k++){
                s+= q1[j][k] * q2[k][i];
            }
            m[i][j] = s;
        }
    }
    return m;
}
void PrintMatrix(double **a, int n, string name){
    ofstream myfile;
    myfile.open(name);
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            myfile<<a[i][j]<<" ";
        }
        myfile<<"\n";
    }
    myfile.close();
}



//Function to generate and fill random numbers in the matrix per thread.
void* generateRandom(void* arg){
    //returns the thread_id (from 0 to NUM_THREADS-1) 
    int jk = (int) arg;
    //returns the starting and the ending row indices to fill by this thread.
    int st = (int)(jk * n/NUM_THREADS);
    int end = (int)((jk+1) *n/NUM_THREADS);
    //Gives the reentrant seed particularly for this thread by munging the system clock with the thread_id
    unsigned seed = system_clock::now().time_since_epoch().count() + (rand()%(NUM_THREADS+jk));
    mt19937 generator(seed);
    for (int i = st; i<end;i++){
      for(int j = 0; j < n; j++){

        double tmp = (1 + ( (((float)generator())/generator.max()) * 100));
        a[i][j] = tmp;
        l[i][j] = 0;
        u[i][j] = 0;
        if (v){
            pp[i][j] = 0;
            aa[i][j] = tmp;
        }
      }
      p[i] = 0;
    }
    if (jk == NUM_THREADS - 1){
        for (int i =0; i < n%NUM_THREADS;i++){
          for(int j = 0; j<n;j++){
            double tmp = (1 + ( (((float)generator())/generator.max()) * 100));

            a[i][j] = tmp;
            l[i][j] = 0;
            u[i][j] = 0;
            if (v){
                aa[i][j] = tmp;
                pp[i][j] = 0;
            }
          }
          p[i] = 0;
        }
    }
    pthread_exit(NULL);
    return NULL;
}


void* GetMatrix(void* arg){
    string name = "A_1000.txt";
    fstream myfile;
    myfile.open(name);
    //returns the thread_id (from 0 to NUM_THREADS-1) 
    int jk = (int) arg;
    //returns the starting and the ending row indices to fill by this thread.
    int st = (int)(jk * n/NUM_THREADS);
    int end = (int)((jk+1) *n/NUM_THREADS);
    //Gives the reentrant seed particularly for this thread by munging the system clock with the thread_id
    unsigned seed = system_clock::now().time_since_epoch().count() + (rand()%(NUM_THREADS+jk));
    mt19937 generator(seed);
    for (int i = st; i<end;i++){
      for(int j = 0; j < n; j++){

        // double tmp = (1 + ( (((float)generator())/generator.max()) * 100));
        double tmp;
        myfile>>tmp;
        a[i][j] = tmp;
        l[i][j] = 0;
        u[i][j] = 0;
        if (v){
            pp[i][j] = 0;
            aa[i][j] = tmp;
        }
      }
      p[i] = 0;
    }
    if (jk == NUM_THREADS - 1){
        for (int i =0; i < n%NUM_THREADS;i++){
          for(int j = 0; j<n;j++){
            // double tmp = (1 + ( (((float)generator())/generator.max()) * 100));
            double tmp;
            myfile>>tmp;

            a[i][j] = tmp;
            l[i][j] = 0;
            u[i][j] = 0;
            if (v){
                aa[i][j] = tmp;
                pp[i][j] = 0;
            }
          }
          p[i] = 0;
        }
    }
    pthread_exit(NULL);
    return NULL;
}

//Initializes the L and p matrices by adding 1 at the diagonal in L and i in p
void * matrixInit(void* r){
    int q = (int) r;
    int st = q * (n/NUM_THREADS),end = (q+1) * (n/NUM_THREADS);
    for(int i = st; i < end;i++){
        p[i] = i;
        l[i][i] = 1;
    }
    if (q == NUM_THREADS-1){
        for (int i = 0; i < n%NUM_THREADS;i++){
            p[end+ i] = end + i;
            l[end+ i][end + i] = 1;
        }
    }
    pthread_exit(NULL);
    return NULL;
}

//First calculation
void* firstSwap(void* r){
    arguments *a0 = (arguments *)r;
    //k contains the column number; q contains the thread id
    int k = a0 -> a2,q = a0 -> a1;
    //start and end row for each matrix
    int st = k+1 + (q*(n-k-1)/NUM_THREADS);
    int end = k+1 + (q+1)*(n-k-1)/NUM_THREADS;
    for (int i =st; i<end; i++){
        l[i][k] = a[i][k]/double(u[k][k]);
        u[k][i] = a[k][i];
    }
    if (q == NUM_THREADS- 1){
        for(int i  = end; i <n;i++){
            l[i][k] = a[i][k]/double(u[k][k]);
            u[k][i] = a[k][i];
        }
    }
    pthread_exit(NULL);
    return NULL;
}

void* secSwap(void* r){
    //Same as the function firstSwap
    arguments *a0 = (arguments *)r;
    int k = a0 -> a2,q = a0 -> a1;
    int st = k+1 + (q*(n-k-1)/NUM_THREADS);
    int end = k+1+ (q+1)*(n-k-1)/NUM_THREADS;

    for(int i = st; i<end;i++){
        for (int j = k+1; j<n;j++){
            a[i][j] = a[i][j] - l[i][k]* u[k][j];
        }
    }
    if (q == NUM_THREADS-1){
        for(int i = end; i < n; i++){
            for(int j = k+1; j<n ;j++){
            a[i][j] = a[i][j] - l[i][k]* u[k][j];
            }
        }
    }
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char ** argv){
    if (argc == 4){
    //If there are 4 arguments
        NUM_THREADS = stoi(argv[1]);
        matrix_size = stoi(argv[2]);
        //v refers to whether we want to get the L2 norm or not
        v = (stoi(argv[3]) == 0)?false:true;
    }else if (argc == 3){
    //If there are 3 arguments
        NUM_THREADS = stoi(argv[1]);
        matrix_size = stoi(argv[2]);
    }else{
    //Wrong number of arguments
        cout<<"Wrong Number of Arguments Passed. Exiting...\n";
        return 0;
    }
    n = matrix_size;
    pthread_t threads[NUM_THREADS];

    //Allocating memoru for the arrays
    a = (double**)malloc(n * sizeof(double*));
    l = (double**)malloc(n * sizeof(double*));
    u = (double**)malloc(n * sizeof(double*));
    p = (int*)malloc(n*sizeof(int));
    if (v){
        pp = (double**)malloc(n * sizeof(double*));
        aa = (double**)malloc(n * sizeof(double*));
    }
    
    for (int i = 0; i< n; i++){
        //allocating memory for each row in the array.
        a[i] = (double*)malloc(n * sizeof(double));
        l[i] = (double*)malloc(n * sizeof(double));
        u[i] = (double*)malloc(n * sizeof(double));
        if (v){
            pp[i] = (double*)malloc(n * sizeof(double));
            aa[i] = (double*)malloc(n * sizeof(double));
        }

    }
    //Clock Start
    auto start = high_resolution_clock::now();
    //Seed for inside the random number generator
    srand(time(0));
    
    for ( int i = 0; i < NUM_THREADS; i++){
        int ti = pthread_create(&threads[i],NULL,generateRandom,(void *)i);
        // int ti = 
    }
    

    for(int i =0; i< NUM_THREADS;i++){
        int a = pthread_join(threads[i],NULL);
    }

    for ( int i = 0; i < NUM_THREADS; i++){
        int ti = pthread_create(&threads[i],NULL,matrixInit,(void *)i);
    }

    for(int i =0; i< NUM_THREADS;i++){
        int w = pthread_join(threads[i],NULL);
    }


    ifstream myfile;
    myfile.open(name);
    double ma;
    for (int i = 0; i<n;i++){
        for(int j=0; j<n;j++){
            myfile >> ma;
            a[i][j] = ma;
            aa[i][j] = ma;
        }
    }

    PrintMatrix(a,100,"p.txt");

    //For each column number
    for (int  k = 0; k < n; k++){
        double m = 0;
        int ind = 0;
        //Get max of the current column from beneath the diagonal
        for (int  i  = k; i<n; i++){
            if (m < abs(a[i][k])){
                m = abs(a[i][k]);
                ind = i;
            }
        }
        if (m == 0){
            cerr << "Singular Matrix"<<endl;
            break;
        }
        //Swapping p
        double t = p[ind];
        p[ind] = p[k];
        p[k] = t;

        //swapping row in a
        double* temp = a[ind];
        a[ind] = a[k];
        a[k] = temp;
        //Swapping specific slice in l
        for (int i = 0; i <= k-1; i++){
            double t = l[k][i];
            l[k][i] = l[ind][i];
            l[ind][i] = t;
        }
        u[k][k] = a[k][k];
        //Generate an argument list to send to the thread.
        arguments * argList;
        argList = malloc(NUM_THREADS * sizeof(arguments));
        //Function for the first loop
            for (int i  = 0; i < NUM_THREADS; i++){
                (argList + i) -> a1 = i;
                (argList + i) -> a2 = k;
                pthread_create(&threads[i],NULL,firstSwap,(void *)(argList+i));
            }
            for(int i = 0; i < NUM_THREADS; i++){
                pthread_join(threads[i],NULL);
            }
          free(argList);
          argList = malloc(NUM_THREADS * sizeof(arguments));
        //Function for the second loop
            for (int i  = 0; i < NUM_THREADS; i++){
                (argList + i) -> a1 = i;
                (argList + i) -> a2 = k;
                pthread_create(&threads[i],NULL,secSwap,(void *)(argList+i));
            }
            for(int i = 0; i < NUM_THREADS; i++){
                pthread_join(threads[i],NULL);
            }
    }
    //Clock Stop
    auto stop = high_resolution_clock::now();
    //We calculate duration here.
    auto duration = duration_cast<microseconds>(stop - start);
    cout <<"Time taken in microseconds: "<< duration.count() << endl;
    if (v){
        for (int i = 0; i<100;i++ ){
            cout<<p[i]<<" ";
        }
        cout<<"\n";
        //This code executes only if v is true
        for(int i = 0; i<n; i++){
            pp[i][p[i]] = 1;        
        }
        cout<<"L2,1 norm: "<<l2norm(matmul(pp, aa), matmul(l, u))<<endl;
        PrintMatrix(l, n, "Lower.txt");
        PrintMatrix(u, n, "Upper.txt");
        PrintMatrix(pp, n, "Permutation.txt");
    }

    return 0;

}
