#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>
#include <time.h>
using namespace std;

typedef vector<float> ROW;
typedef vector<ROW> MATRIX;

struct arguments{
    int a1;
    int a2;
};

#define NUM_THREADS 4

MATRIX a,l,u;
pthread_mutex_t mlock;
ROW p;

pthread_t threads[NUM_THREADS];

int n;

int k2,q2;


void print_vector(vector<vector<float> > v){
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
    int dum = (int)(n/NUM_THREADS);
    for (int i = 0; i < dum; i++){
        ROW r,s;
        for(int j = 0; j<n;j++){
            r.push_back(1 + (drand48() * 100) );
            s.push_back(0);
        }
        pthread_mutex_lock(&mlock);
        a.push_back(r);
        l.push_back(s);
        u.push_back(s);
        p.push_back(0);
        pthread_mutex_unlock(&mlock);
    }
    if (jk == NUM_THREADS - 1){
        pthread_mutex_lock(&mlock);
        for(int i = 0; i<(n%NUM_THREADS);i++){
            ROW r,s;
            for(int j = 0; j<n;j++){
                r.push_back(1 + (drand48() * 100) );
                s.push_back(0);
            }
            a.push_back(r);
            l.push_back(s);
            u.push_back(s);
            p.push_back(0);
        }
        pthread_mutex_unlock(&mlock);
    }
    pthread_exit(NULL);

    // pthread_mutex_lock(&mlock);
    // a.insert(a.end(),m.begin(),m.end());
    // pthread_mutex_unlock(&mlock);
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


void* firstSwap(void* r){
    // pthread_mutex_lock(&mlock);
    // vector<int> * v = static_cast<vector<int> *>(r);c
    struct arguments *a0 = (struct arguments *)r;
    int k = a0 -> a2,q = a0 -> a1;
    int st = k + (q*(n-k)/NUM_THREADS);
    int end = k+ (q+1)*(n-k)/NUM_THREADS;
    // cout<<"st: "<<st<<" end: "<<end<<": "<<k<<" "<<q<<" "<<n<<" "<<endl;
    // pthread_mutex_lock(&mlock);
    // print_vector(l);
    // pthread_mutex_unlock(&mlock);

    for (int i =st; i<end; i++){
        l[i][k] = a[i][k]/float(u[k][k]);
        u[k][i] = a[k][i];
    }
    if (q == NUM_THREADS- 1){
        // pthread_mutex_lock(&mlock);
        for(int i  = end; i <n;i++){
            l[i][k] = a[i][k]/float(u[k][k]);
            u[k][i] = a[k][i];
        }
        // pthread_mutex_unlock(&mlock);
    }
    // pthread_mutex_lock(&mlock);
    // print_vector(l);
    // pthread_mutex_unlock(&mlock);
    pthread_exit(NULL);
    return NULL;
}

void* secSwap(void* r){
    //{q,k,n}
    vector<int> v = *(reinterpret_cast< vector<int>  *>(r)); 
    int k = v[1],q = v[0],n = v[2];
    int st = k + (q * (n-k)/NUM_THREADS);
    int end = ((q+1)*(n-k)/NUM_THREADS);
    for(int i = st; i<end;i++){
        for (int j = k; j<n;j++){
            a[i][j] = a[i][j] - l[i][k]* u[k][j];
        }
    }
    return NULL;
}

int main(int argc, char ** argv){
    if (argc == 2){
        n = stoi(argv[1]);
    }else{
        n = 500;
    }
    srand(time(0));


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
            for (int i  = 0; i < NUM_THREADS; i++){
                arguments args = {i,k};
                pthread_create(&threads[i],NULL,firstSwap,(void *)&args);
            }
            for(int i = 0; i < NUM_THREADS; i++){
                pthread_join(threads[i],NULL);
            }
            // for (int i = k; i< n; i++){
            //     for(int j = k ; j< n; j++){
            //         a[i][j] = a[i][j] - (l[i][k] * u[k][j]);
            //     }
            // }   
            // for (int i  = 0; i < NUM_THREADS; i++){
            //     vector<int> q = {i,k,n};
            //     pthread_create(&threads[i],NULL,secSwap,static_cast<void*>(&q));
            // }
            // for(int i = 0; i < NUM_THREADS; i++){
            //     pthread_join(threads[i],NULL);
            // }
        // }else{
            // for (int i = k; i < n; i++){
            //     l[i][k] = a[i][k]/u[k][k];
            //     u[k][i] = a[k][i];
            // }
            for (int i = k; i< n; i++){
                for(int j = k ; j< n; j++){
                    a[i][j] = a[i][j] - (l[i][k] * u[k][j]);
                }
            }            
        // }

    }

    // print_vector(a);
    // cout <<"---------"<<endl;
    // print_vector(l);
    // cout <<"---------"<<endl;
    // print_vector(u);
    
    
    
    return 0;

}