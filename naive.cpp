#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>
#include <chrono> 
#include <time.h>

using namespace std::chrono; 
using namespace std;

int n ;


typedef vector<double> ROW;
typedef vector<ROW> MATRIX;

//Printing the matrix
void print_vector(MATRIX v){
    int n = v.size();
    for (int i = 0; i < n; i++){
        for (int j = 0; j< n; j++){
            cout << v[i][j]<<" ";
        }
        cout<<endl;
    }
}

//Matrix multiplication of two matrices q1 and q2
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

//Calculation of l2 norm given two matrices q1 and q2
double l2norm(MATRIX q1, MATRIX q2){
    double ans = 0;

    for(int i = 0; i < n; i++){
        for (int j = 0; j < n ;j++){
            ans = ans + (q1[i][j] - q2[i][j])*(q1[i][j] - q2[i][j]);
            
        }
    }
    return sqrt(ans);
}

MATRIX generateRandom(int n){
    MATRIX v(n,vector<double>(n,0));
    for (int i = 0; i < n; i++){
        for(int j = 0; j<n;j++){
            v[i][j] = 1 + ((float)(rand())/RAND_MAX * 100);
        }
    }
    return v;
}

int main(int argc, char const *argv[]){
    srand(time(0));
    auto start = high_resolution_clock::now();
    n  = stoi(argv[1]);
    
    MATRIX v,l(n,vector<double>(n,0)),u(n,vector<double>(n,0));
    for(int i = 0; i < n; i++){
        l[i][i] = 1;
    }
    
    vector<int> p(n,0);
    for(int i = 0; i < n ;i++){
        p[i] = i+1;
    }

    v = generateRandom(n);
    
    MATRIX a = v;


    for ( int k = 0; k < n;k++){ // For each column 
        double m = 0;
        int ind = 0;
        
        for (int i = k;i<n;i++){
            if (m < abs(v[i][k])){
                m = abs(v[i][k]);
                ind = i;
            }
        }
        
        if (m == 0){
            cerr <<"Singular Matrix!";
            break;
        }

        double t = p[ind];
        p[ind] = p[k];
        p[k] = t;
        
        vector<double> temp = v[ind];
        v[ind] = v[k];
        v[k] = temp;

        for ( int i = 0; i<=k-1;i++){
            double tm = l[k][i];
            l[k][i] = l[ind][i];
            l[ind][i] = tm;
        }

        u[k][k] = v[k][k];

        for (int i = k+1; i < n; i++){
            l[i][k] = v[i][k]/u[k][k];
            u[k][i] = v[k][i];
        }

        for (int i = k+1; i< n; i++){
            for(int j = k+1 ; j< n; j++){
                v[i][j] = v[i][j] - (l[i][k] * u[k][j]);
            }
        }
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start); 
    cout<< "--" << duration.count() << endl; 

    //Converting p of size(n,1) to matrix of size (n,n) for calculation of PA
    MATRIX pp = generateRandom(p.size());
    for(int i = 0; i<p.size(); i++){
        for(int j = 0; j<p.size(); j++){
            if(p[i] == j+1){
                pp[i][j] = 1;
            }else{
                pp[i][j] = 0;
            }
        }
    }

    cout<<l2norm(matmul(pp, a), matmul(l, u))<<endl;


    return 0;
}
