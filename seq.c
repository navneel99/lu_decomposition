#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>

int n ;
int nthreads = 4;

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

void print_array(double **d, int n){
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            printf("%f ", d[i][j]);
        }
        printf("\n");
    }
}

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

double **make2DmatrixO(int n, bool init, bool isl){
    int i;
    double **m;
    m = (double**)malloc(n*sizeof(double*));
    for(i = 0; i<n; i++){
        m[i] = (double*)malloc(n*sizeof(double));
    }
    if(init){
        #pragma omp collapse(2)
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
        #pragma omp collapse(2)
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

void Serial(bool check){
    double **v = make2Dmatrix(n, false, false);
    double **l = make2Dmatrix(n, true, true);
    double **u = make2Dmatrix(n, true, false);

    int *p = (int*)malloc(n*sizeof(int));
    for(int i = 0; i<n; i++){
        p[i] = i+1;
    }

    // MATRIX a = v;
    double **a = make2Dmatrix(n, false, false);
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            a[i][j] = v[i][j];
        }
    }

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
            printf("Singular Matrix!");
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
    
    double **pp = make2Dmatrix(n, true, false);
    for(int i = 0; i<n; i++){
        pp[i][p[i]-1] = 1;
        
    }
    if(check){
        double **pa = matmul(pp, a);
        double **lu = matmul(l, u);
        double ans = l2norm(pa, lu);
        printf("%f ", ans);
    }
}

void Openmp(bool check){
    double **v = make2DmatrixO(n, false, false);
    double **l = make2DmatrixO(n, true, true);
    double **u = make2DmatrixO(n, true, false);

    int *p = (int*)malloc(n*sizeof(int));
    for(int i = 0; i<n; i++){
        p[i] = i+1;
    }

    // MATRIX a = v;
    double **a = make2DmatrixO(n, false, false);
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            a[i][j] = v[i][j];
        }
    }


    for ( int k = 0; k < n;k++){ // For each column 
        double m = 0; 
        int ind = 0; 

        #pragma omp parallel for num_threads(nthreads)
        for (int i = k;i<n;i++){
            if (m < abs(v[i][k])){
                m = abs(v[i][k]);
                ind = i;
            }
        }

        if (m == 0){
            printf("Singular Matrix!");
            break;
        }

        
        double t = p[ind];
        p[ind] = p[k];
        p[k] = t;

        double *tem = v[ind];
        v[ind] = v[k];
        v[k] = tem;

        #pragma omp parallel shared(m, ind, v, l, u, p) num_threads(nthreads)
        {
            #pragma omp for
            for ( int i = 0; i<=k-1;i++){
                double tm = l[k][i];
                l[k][i] = l[ind][i];
                l[ind][i] = tm;
            }
            
            u[k][k] = v[k][k];

            #pragma omp for
            for (int i = k+1; i < n; i++){
                l[i][k] = v[i][k]/u[k][k];
                u[k][i] = v[k][i];
            }
        }
        #pragma omp collapse(2)
        for (int i = k+1; i< n; i++){
            for(int j = k+1 ; j< n; j++){
                v[i][j] = v[i][j] - (l[i][k] * u[k][j]);
            }
        }
    }
 
    
    double **pp = make2DmatrixO(n, true, false);
    for(int i = 0; i<n; i++){
        pp[i][p[i]-1] = 1;
        
    }
    if(check){
        double **pa = matmul(pp, a);
        double **lu = matmul(l, u);
        double ans = l2norm(pa, lu);
        printf("%f ", ans);
    }
}
int main(int argc, char const *argv[]){
    srand(time(0)); 
    sscanf(argv[1], "%d", &n);
    int t = 0; 
    sscanf(argv[2], "%d", &t);
    int check = 0;
    sscanf(argv[3], "%d", &check);

    double tt = omp_get_wtime();
    if(t ==  0){
        Serial(check);
        double time_taken = omp_get_wtime() - tt; // in seconds 
        printf("LU-decomposition took %f seconds to execute Serial code \n", time_taken);
    }else{
        sscanf(argv[4], "%d", &nthreads);
        Openmp(check);
        double time_taken = omp_get_wtime() - tt; // in seconds 
        printf("LU-decomposition took %f seconds to execute Openmp code \n", time_taken);
    }
    
    


    return 0;
}
