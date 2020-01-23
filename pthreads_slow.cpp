#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>
#include <time.h>
#include <chrono>

using namespace std::chrono;
using namespace std;

typedef vector<double> ROW;
typedef vector<ROW> MATRIX;

struct arguments{
    int a1;
    int a2;
};

#define NUM_THREADS 8

MATRIX a,l,u;
pthread_mutex_t mlock;
vector<int> p;

pthread_t threads[NUM_THREADS];

int n;

int k2,q2;


void print_vector(vector<vector<double> > v){
    int n = v.size();
    for (int i = 0; i < n; i++){
        for (int j = 0; j< n; j++){
            cout << v[i][j]<<" ";
        }
        cout<<endl;
    }
}

double l2norm(MATRIX q1, MATRIX q2){
    double ans = 0;

    for(int i = 0; i < n; i++){
        for (int j = 0; j < n ;j++){
            ans = ans + (q1[i][j] - q2[i][j])*(q1[i][j] - q2[i][j]);

        }
    }
    return sqrt(ans);
}

MATRIX matmul(MATRIX q1, MATRIX q2){
    MATRIX m(n,ROW(n,0));
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
    MATRIX m;
    int dum = (int)(n/NUM_THREADS);
    for (int i = 0; i < dum; i++){
        ROW r,s;
        for(int j = 0; j<n;j++){
            r.push_back(1 + (((float)rand()/RAND_MAX) * 100) );
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
                r.push_back(1 + (((float)rand()/RAND_MAX) * 100) );
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
    arguments *a0 = (arguments *)r;
    int k = a0 -> a2,q = a0 -> a1;
    // cout<<k<<"||"<<q<<endl;
    int st = k+1 + (q*(n-k-1)/NUM_THREADS);
    int end = k+1 + (q+1)*(n-k-1)/NUM_THREADS;
    // cout<<st<<" "<<end<<" "<<q<<" "<<k<<" "<<endl;//st<<" "<<st<<" "
    // pthread_mutex_unlock(&mlock);
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
    // free(r);
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
    auto start = high_resolution_clock::now();
    if (argc == 2){
        n = stoi(argv[1]);
    }else{
        n = 500;
    }
    srand(time(0));

    // n = 3;

    pthread_mutex_init(&mlock,NULL);
    createMatrix();
    // a = {{34.5575, 82.4529, 11.7004},{65.1702, 14.8189, 59.231},{48.3556, 27.7371, 4.53131}};
    MATRIX aa = a;
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

        ROW tmp = a[ind];
        a[ind] = a[k];
        a[k] = tmp;

        for (int i = 0; i <= k-1; i++){
            double t = l[k][i];
            l[k][i] = l[ind][i];
            l[ind][i] = t;
        }
        u[k][k] = a[k][k];
        // struct arguments* args;
        // if ((n-k) > NUM_THREADS){
        arguments * argList;
        argList = malloc(NUM_THREADS * sizeof(arguments));
            for (int i  = 0; i < NUM_THREADS; i++){
                // arguments args = {i,k};
                // (*args).a1 = i;
                // (*args).a2 = k;
                (argList + i) -> a1 = i;
                (argList + i) -> a2 = k;
                // cout<<i<<";;"<<k<<endl;
                pthread_create(&threads[i],NULL,firstSwap,(void *)(argList+i));
            }
            for(int i = 0; i < NUM_THREADS; i++){
                pthread_join(threads[i],NULL);
            }
          free(argList);
          argList = malloc(NUM_THREADS * sizeof(arguments));

            // for (int i = k; i< n; i++){
            //     for(int j = k ; j< n; j++){
            //         a[i][j] = a[i][j] - (l[i][k] * u[k][j]);
            //     }
            // }
            for (int i  = 0; i < NUM_THREADS; i++){
                // vector<int> args = {i,k};
                (argList + i) -> a1 = i;
                (argList + i) -> a2 = k;
                pthread_create(&threads[i],NULL,secSwap,(void *)(argList+i));
            }
            for(int i = 0; i < NUM_THREADS; i++){
                pthread_join(threads[i],NULL);
            }
        // }else{
            // for (int i = k+1; i < n; i++){
            //     l[i][k] = a[i][k]/u[k][k];
            //     u[k][i] = a[k][i];
            // }
            // for (int i = k+1; i< n; i++){
            //     for(int j = k+1 ; j< n; j++){
            //         a[i][j] = a[i][j] - (l[i][k] * u[k][j]);
            //     }
            // }
        // }

    }
    auto stop = high_resolution_clock::now();

    MATRIX pp(n,vector<double>(n,0));
    for(int i = 0; i<p.size(); i++){
      pp[i][ p[i] ] = 1;
    }
    // print_vector(pp);
    cout<<"----------"<<endl;
    cout<<l2norm(matmul(pp, aa), matmul(l, u))<<endl;
    cout<<"----------"<<endl;
    // print_vector(aa);
    // cout <<"---------"<<endl;
    // print_vector(l);
    // cout <<"---------"<<endl;
    // print_vector(u);
    auto duration = duration_cast<microseconds>(stop - start);

    // To get the value of duration use the count()
    // member function on the duration object
    cout << duration.count() << endl;



    return 0;

}
