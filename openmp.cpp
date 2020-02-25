#include <bits/stdc++.h>
#include <chrono> 
#include <omp.h>
#include <math.h>
#include <fstream>

using namespace std::chrono; 
using namespace std;

int n ;
int n_thread;

//Calculation of l2 norm given two matrices q1 and q2
double l2norm(double** q1, double** q2){
    double ans = 0;

    for(int j = 0; j < n; j++){
        double sum = 0;
        for (int i = 0; i < n ;i++){
            sum = sum + (q1[i][j] - q2[i][j])*(q1[i][j] - q2[i][j]);
            // cout<<q1[i][j]<<"-"<<q2[i][j]<<" ";
        }
        // cout<<"\n";
        ans = ans + sqrt(sum);
    }
    return ans;
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

double **Get2Dmatrix(int n, bool init, bool isl, string name){
    ifstream myfile;
    myfile.open(name);

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
                double a;
                myfile>>a;
                m[i][j] = a;
            }
        }
    }
    myfile.close();
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

void PrintMatrix(double **a, int n, string name){
    ofstream myfile;
    myfile.open(name);
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            myfile<<a[i][j]<<" ";
        }
        myfile<<"\n";
    }
    myfile.close();
}

int main(int argc, char const *argv[]){
    srand(time(0));
    auto start = high_resolution_clock::now();
    
    n  = stoi(argv[1]);
    n_thread = stoi(argv[2]);
    int verify = stoi(argv[3]);
    int batch_size = ceil(n/n_thread);
    

    double **v = Get2Dmatrix(n, false, false, "A_1000.txt");
    // double **v = make2Dmatrix(n, false, false);
    double **l = make2Dmatrix(n, true, true);
    double **u = make2Dmatrix(n, true, false);
    int *p = (int*)malloc(n*sizeof(int));
    #pragma omp parallel for schedule(static, batch_size)
    for(int i = 0; i<n; i++){
        p[i] = i;
    }

    double **a = make2Dmatrix(n, false, false);

    #pragma omp parallel for collapse(2)
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            a[i][j] = v[i][j];
        }
    }

    //LU decomposition using partial pivoting iterating over columns
    for ( int k = 0; k < n;k++){
        double m = 0; 
        int ind = 0; 

        #pragma omp parallel for shared(m, k, ind)
        for (int i = k;i<n;i++){
            if (m < abs(v[i][k])){
                m = abs(v[i][k]);
                ind = i;
            }
        }  

        if (m == 0){
            cerr <<"Singular Matrix!"<<endl;
            break;
        }

        
        double t = p[ind];
        p[ind] = p[k];
        p[k] = t;

        double *tem = v[ind];
        v[ind] = v[k];
        v[k] = tem;

        

        #pragma omp parallel shared(l, u, v, k)
        {
            #pragma omp  for schedule(static)
            for ( int i = 0; i<=k-1;i++){
                double tm = l[k][i];
                l[k][i] = l[ind][i];
                l[ind][i] = tm;
            }
            u[k][k] = v[k][k];
            #pragma omp for schedule(static)
            for (int i = k+1; i < n; i++){
                l[i][k] = v[i][k]/double(u[k][k]);
                u[k][i] = v[k][i];
            }
        }
        
        #pragma omp parallel for collapse(2)
        for (int i = k+1; i< n; i++){
            for(int j = k+1; j< n; j++){
                v[i][j] = v[i][j] - (l[i][k] * u[k][j]);
            }
        }
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start); 
    cout<< "Time taken by Openmp code for Lu-Decomposition: " << duration.count() << endl; 

    //Verify is used in case we want to print the l2-norm value for PA-LU matrix.
    if(verify == 1){

        for (int i = 0; i<100;i++ ){
            cout<<p[i]<<" ";
        }
        cout<<"\n";

        double **pp = make2Dmatrix(n, true, false);
        for(int i = 0; i<n; i++){
            pp[i][p[i]] = 1;
            
        }

        double **pa = matmul(pp, a);
        double **lu = matmul(l, u);
        cout<<l2norm(pa, lu)<<endl;
        // print_array(u, n);
        PrintMatrix(pa, n, "PA.txt");
        PrintMatrix(lu, n, "LU.txt");
        PrintMatrix(l, n, "Lowerop.txt");
        PrintMatrix(u, n, "Upperop.txt");
        PrintMatrix(pp, n, "Permutationop.txt");
    }

    return 0;
}
