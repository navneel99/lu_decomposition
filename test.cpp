#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>

using namespace std;

typedef vector<double> ROW;
typedef vector<ROW> MATRIX;

// struct payload{
    
// };

#define NUM_THREADS 2


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
    int jk = (int) arg;
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
    if (jk == NUM_THREADS - 1){
        for(int i = 0; i<n%NUM_THREADS;i++){
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
    }


    pthread_mutex_lock(&mlock);
    a.insert(a.end(),m.begin(),m.end());
    pthread_mutex_unlock(&mlock);
    return NULL;
}

void * matrixInit(void* r){
    int q = (int) r;
    double st = q * (n/NUM_THREADS),end = (q+1) * (n/NUM_THREADS);
    for(double i = st; i < end;i++){
        p[i] = i;
        l[i][i] = 1;
    }
    if (q == NUM_THREADS-1){
        for (double i = 0; i < n%NUM_THREADS;i++){
            p[end+ i] = end + i;
            l[end+ i][end + i] = 1;
        }
    }
    return NULL;
}


void createMatrix(){

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

}


void* swapOperation(void* r){
    //{q,k,n,i}
    vector<int> v = *(reinterpret_cast< vector<int>  *>(r)); 
    int k = v[1],q = v[0],n = v[2];//,i= v[3];
    int st = k + (q*(n-k)/NUM_THREADS);
    int end = (q+1)*(n-k)/NUM_THREADS;
    for (int i =st; i<end; i++){
        l[i][k] = a[i][k]/u[i][k];
        u[k][i] = a[k][i];
    }

    if (q == NUM_THREADS- 1){
        for(int i  = end; i <n;i++){
            l[i][k] = a[i][k]/u[i][k];
            u[k][i] = a[k][i];
        }
    }
    return NULL;
}

int main(){
    n = 5;

    pthread_mutex_init(&mlock,NULL);
    createMatrix();

    for (int  k = 0; k < n; k++){
        double m = 0;
        int ind = 0;
        for (int  i  = k; i<n; i++){
            if (m < a[i][k]){
                m = a[i][k];
                ind = i;
            }
        }
        if (m == 0){
            cerr << "Singular Matrix"<<endl;
            break;
        }
        int t = p[ind];
        p[ind] = p[k];
        p[k] = t;
        
        ROW tmp = a[ind];
        a[ind] = a[k];
        a[k] = tmp;

        for (int i = 0; i < k-1; i++){
            double t = l[k][i];
            l[k][i] = l[ind][i];
            l[ind][i] = t; 
        }
        u[k][k] = a[k][k];

        // if ((n-k) > NUM_THREADS){
        //     for (int i  = 0; i < NUM_THREADS; i++){
        //         vector<int> q = {i,k,n};
        //         pthread_create(&threads[i],NULL,swapOperation,static_cast<void*>(&q));
        //     }
        //     for(int i = 0; i < NUM_THREADS; i++){
        //         pthread_join(threads[i],NULL);
        //     }
        // }else{
            for (int i = k; i < n; i++){
                l[i][k] = a[i][k]/u[k][k];
                u[k][i] = a[k][i];
            }
            for (int i = k; i< n; i++){
                for(int j = k ; j< n; j++){
                    a[i][j] = a[i][j] - (l[i][k] * u[k][j]);
                }
            }            
        // }

    }

    print_vector(a);
    cout <<"---------"<<endl;
    print_vector(l);
    cout <<"---------"<<endl;
    print_vector(u);
    
    
    
    return 0;

}