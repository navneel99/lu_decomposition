#include <bits/stdc++.h>
#include <chrono> 

using namespace std::chrono; 
using namespace std;

int n ;

//Calculation of l2 norm given two matrices q1 and q2
double l2norm(double **q1, double **q2){
    double ans = 0;

    for(int i = 0; i < n; i++){
        for (int j = 0; j < n ;j++){
            ans = ans + (q1[i][j] - q2[i][j])*(q1[i][j] - q2[i][j]);
            
        }
    }
    return sqrt(ans);
}

//function that print an array
void print_array(double **d, int n){
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            printf("%f ", d[i][j]);
        }
        printf("\n");
    }
}

//function to make an 2D matrix and return the pointer to it
double **make2Dmatrix(int n, bool init, bool isl){
    int i;
    double **m;
    m = (double**)malloc(n*sizeof(double*));
    for(i = 0; i<n; i++){
        m[i] = (double*)malloc(n*sizeof(double));
    }
    if(init){
        for(int i = 0; i<n; i++){
            for(int j = 0; j<n; j++){
                if(isl && i == j){
                    m[i][j] = 1;
                }else{
                    m[i][j] = 0;
                }
            }
        }
    }else{
        for(int i = 0; i<n; i++){
            for(int j = 0; j<n; j++){
                m[i][j] = 1 + ((float)(rand())/RAND_MAX * 100);
            }
        }
    }
    return m;
}

//Matrix multiplication of two matrices q1 and q2
double **matmul(double **q1, double **q2){
   double **m = make2Dmatrix(n, false, false);
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

int main(int argc, char const *argv[]){
    srand(time(0));
    auto start = high_resolution_clock::now();
    n  = stoi(argv[1]);
    int verify = stoi(argv[2]);
    double **v = make2Dmatrix(n, false, false);
    double **l = make2Dmatrix(n, true, true);
    double **u = make2Dmatrix(n, true, false);

    int *p = (int*)malloc(n*sizeof(int));
    for(int i = 0; i<n; i++){
        p[i] = i+1;
    }


    double **a = make2Dmatrix(n, false, false);
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            a[i][j] = v[i][j];
        }
    }

    //LU decomposition using partial pivoting iterating over columns
    for ( int k = 0; k < n;k++){
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

        double *tem = v[ind];
        v[ind] = v[k];
        v[k] = tem;

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
    cout<< "Time taken by Serial code for Lu-Decomposition: " << duration.count() << endl; 

    if(verify == 1){
        double **pp = make2Dmatrix(n, true, false);
        for(int i = 0; i<n; i++){
            pp[i][p[i]-1] = 1;
            
        }

        double **pa = matmul(pp, a);
        double **lu = matmul(l, u);
        cout<<l2norm(matmul(pp, a), matmul(l, u))<<endl;
    }

    return 0;
}
