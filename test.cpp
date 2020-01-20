#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>

using namespace std;

typedef vector<double> ROW;
typedef vector<ROW> MATRIX;

#define NUM_THREADS 2


MATRIX a;


void print_vector(vector<vector<double> > v){
    int n = v.size();
    for (int i = 0; i < n; i++){
        for (int j = 0; j< n; j++){
            cout << v[i][j]<<" ";
        }
        cout<<endl;
    }
}


void* generateRandom(void* vn){
    int n = (int) vn;
    for (int i = 0; i < n/NUM_THREADS; i++){
        ROW r;
        for(int j = 0; j<n;j++){
            r.push_back(1 + (drand48() * 100) );
        }
        a.push_back(r);
    }

}


// void * diag( void * p){
//     vector<vector<int> > v = *(reinterpret_cast< vector<vector<int> > *> (p) );
//     int n = v.size();
//     for (int i = 0; i<n;i++){
//         v[i][i]= 1;
//     }
//     cout<<"In DIAG: "<<endl;
//     print_vector(v);
//     p = (void*)&v;
//     return p;
// }



// void *ludecompose( vector<vector<int> >v,int n){

//     vector<vector<int> > l(n,vector<int>(n,0)),u(n,vector<int>(n,0));

//     pthread_t threads[NUM_THREADS];
//     void* dummy;
//     for (int i = 0; i < NUM_THREADS; i++){
//         int ti = pthread_create(&threads[i],NULL, &diag, &l);
//    }
//     int a = pthread_join(threads[0],&dummy);
//     // pthread_exit
//     cout<<a<<endl;
//     print_vector(l);
//     // int pthread_create();
// }



int main(){
    int n = 10;
    pthread_t threads[NUM_THREADS];
    for ( int i = 0; i < NUM_THREADS; i++){
        int ti = pthread_create(&threads[i],NULL,generateRandom,(void *)n);  
        // generateRandom(n);
    }

    for(int i =0; i< NUM_THREADS;i++){
        void * d;
        int a = pthread_join(threads[i],&d);
    }
    print_vector(a);
    return 0;

}