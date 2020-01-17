#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>

#define NUM_THREADS 1

using namespace std;


void print_vector(vector<vector<int> > v){
    int n = v.size();
    for (int i = 0; i < n; i++){
        for (int j = 0; j< n; j++){
            cout << v[i][j]<<" ";
        }
        cout<<endl;
    }
}


void * diag( void * p){
    vector<vector<int> > v = *(reinterpret_cast< vector<vector<int> > *> (p) );
    int n = v.size();
    for (int i = 0; i<n;i++){
        v[i][i]= 1;
    }
    cout<<"In DIAG: "<<endl;
    print_vector(v);
    p = (void*)&v;
    return p;
}



void *ludecompose( vector<vector<int> >v,int n){

    vector<vector<int> > l(n,vector<int>(n,0)),u(n,vector<int>(n,0));

    pthread_t threads[NUM_THREADS];
    void* dummy;
    for (int i = 0; i < NUM_THREADS; i++){
        int ti = pthread_create(&threads[i],NULL, &diag, &l);
   }
    int a = pthread_join(threads[0],&dummy);
    // pthread_exit
    cout<<a<<endl;
    print_vector(l);
    // int pthread_create();
}



int main(){
    int n =3;
    vector<vector<int> >k(n,vector<int>(n,0));
    ludecompose(k,n);
    return 0;

}