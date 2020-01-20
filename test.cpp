#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>

using namespace std;

typedef vector<double> ROW;
typedef vector<ROW> MATRIX;

#define NUM_THREADS 4


MATRIX a,l,u;
pthread_mutex_t mlock;
ROW p;

pthread_t threads[NUM_THREADS];

int n;



void print_vector(vector<vector<double> > v){
    int n = v.size();
    for (int i = 0; i < n; i++){
        for (int j = 0; j< n; j++){
            cout << v[i][j]<<" ";
        }
        cout<<endl;
    }
}


void* generateRandom(void* arg){
    // int n = (int) vn;
    MATRIX m;
    for (int i = 0; i < n/NUM_THREADS; i++){
        ROW r,s;
        for(int j = 0; j<n;j++){
            r.push_back(1 + (drand48() * 100) );
            s.push_back(0);
        }
        m.push_back(r);
        l.push_back(s);
        u.push_back(s);
        p.push_back(0);
    }

    pthread_mutex_lock(&mlock);
    a.insert(a.end(),m.begin(),m.end());
    pthread_mutex_unlock(&mlock);

}

void * matrixInit(void* r){
    int q = (int) r;
    double st = q * (n/NUM_THREADS),end = (q+1) * (n/NUM_THREADS);
    for(double i = st; i < end;i++){
        p[i] = i;
        l[i][i] = 1;
    }
}


void createMatrix(){

    for ( int i = 0; i < NUM_THREADS; i++){
        int ti = pthread_create(&threads[i],NULL,generateRandom,(void *)n); 
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

}


int main(){
    n = 5;

    pthread_mutex_init(&mlock,NULL);
    createMatrix();





    print_vector(a);
    cout <<"---------"<<endl;
    print_vector(l);
    cout <<"---------"<<endl;
    print_vector(u);
    
    
    
    return 0;

}