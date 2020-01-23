#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>
#include <time.h>
#include <chrono>
#include <random>

using namespace std::chrono;
using namespace std;

// #define NUM_THREADS 8


struct arguments{
    int a1;
    int a2;
};


int NUM_THREADS;
int matrix_size;// = 7000;
// const int n = matrix_size;
int n;

double **a;// = (double**)malloc(n * sizeof(double*));
double **l;// = (double**)malloc(n * sizeof(double*));
double **u;// = (double**)malloc(n * sizeof(double*));
double **pp;// = (double**)malloc(n * sizeof(double*));
double **aa;// = (double**)malloc(n * sizeof(double*));




// pthread_mutex_t mlock;

int *p;// = (int*)malloc(n*sizeof(int));

bool v = false;



void print_vector(double** v ){
    for (int i = 0; i < matrix_size; i++){
        for (int j = 0; j< matrix_size; j++){
            cout << v[i][j]<<" ";
        }
        cout<<endl;
    }
}

double l2norm(double** q1, double** q2){
    double ans = 0;

    for(int i = 0; i < n; i++){
        for (int j = 0; j < n ;j++){
            ans = ans + (q1[i][j] - q2[i][j])*(q1[i][j] - q2[i][j]);

        }
    }
    return sqrt(ans);
}

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

void* generateRandom(void* arg){
    int jk = (int) arg;
    int st = (int)(jk * n/NUM_THREADS);
    int end = (int)((jk+1) *n/NUM_THREADS);
    unsigned seed = system_clock::now().time_since_epoch().count() + (rand()%NUM_THREADS);
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




void* firstSwap(void* r){
    arguments *a0 = (arguments *)r;
    int k = a0 -> a2,q = a0 -> a1;
    int st = k+1 + (q*(n-k-1)/NUM_THREADS);
    int end = k+1 + (q+1)*(n-k-1)/NUM_THREADS;
    for (int i =st; i<end; i++){
        l[i][k] = a[i][k]/float(u[k][k]);
        u[k][i] = a[k][i];
    }
    if (q == NUM_THREADS- 1){
        for(int i  = end; i <n;i++){
            l[i][k] = a[i][k]/float(u[k][k]);
            u[k][i] = a[k][i];
        }
    }
    pthread_exit(NULL);
    return NULL;
}

void* secSwap(void* r){
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
        NUM_THREADS = stoi(argv[1]);
        matrix_size = stoi(argv[2]);
        v = (stoi(argv[3]) == 0)?false:true;
    }else if (argc == 3){
        NUM_THREADS = stoi(argv[1]);
        matrix_size = stoi(argv[2]);
    }else{
        cout<<"Wrong Number of Arguments Passed. Exiting...\n";
        return 0;
    }
    n = matrix_size;
    pthread_t threads[NUM_THREADS];

    a = (double**)malloc(n * sizeof(double*));
    l = (double**)malloc(n * sizeof(double*));
    u = (double**)malloc(n * sizeof(double*));
    p = (int*)malloc(n*sizeof(int));
    if (v){
        pp = (double**)malloc(n * sizeof(double*));
        aa = (double**)malloc(n * sizeof(double*));
    }

    for (int i = 0; i< n; i++){
        a[i] = (double*)malloc(n * sizeof(double));
        l[i] = (double*)malloc(n * sizeof(double));
        u[i] = (double*)malloc(n * sizeof(double));
        if (v){
            pp[i] = (double*)malloc(n * sizeof(double));
            aa[i] = (double*)malloc(n * sizeof(double));
        }

    }
    auto start = high_resolution_clock::now();
    srand(time(0));


    // createMatrix();
    for ( int i = 0; i < NUM_THREADS; i++){
        int ti = pthread_create(&threads[i],NULL,generateRandom,(void *)i);
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





    for (int  k = 0; k < n; k++){
        double m = 0;
        int ind = 0;
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
        double t = p[ind];
        p[ind] = p[k];
        p[k] = t;

        double* temp = a[ind];
        a[ind] = a[k];
        a[k] = temp;

        for (int i = 0; i <= k-1; i++){
            double t = l[k][i];
            l[k][i] = l[ind][i];
            l[ind][i] = t;
        }
        u[k][k] = a[k][k];
        arguments * argList;
        argList = malloc(NUM_THREADS * sizeof(arguments));
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

            for (int i  = 0; i < NUM_THREADS; i++){
                (argList + i) -> a1 = i;
                (argList + i) -> a2 = k;
                pthread_create(&threads[i],NULL,secSwap,(void *)(argList+i));
            }
            for(int i = 0; i < NUM_THREADS; i++){
                pthread_join(threads[i],NULL);
            }
    }
    // }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout <<"Time taken in microseconds: "<< duration.count() << endl;
    if (v){
        for(int i = 0; i<n; i++){
            pp[i][p[i]] = 1;        
        }
        // print_vector(pp);
        cout<<"L2,1 norm: "<<l2norm(matmul(pp, aa), matmul(l, u))<<endl;
    }

    
    // cout<<"----------"<<endl;
    // cout<<"----------"<<endl;
    // print_vector(a);
    // cout <<"---------"<<endl;
    // print_vector(l);
    // cout <<"---------"<<endl;
    // print_vector(u);

    // To get the value of duration use the count()
    // member function on the duration object



    return 0;

}
