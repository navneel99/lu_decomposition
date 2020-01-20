#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>

using namespace std;

typedef vector<vector<double> > MATRIX;


void print_vector(MATRIX v){
    int n = v.size();
    for (int i = 0; i < n; i++){
        for (int j = 0; j< n; j++){
            cout << v[i][j]<<" ";
        }
        cout<<endl;
    }
}

MATRIX generateRandom(int n){
    MATRIX v(n,vector<double>(n,0));
    for (int i = 0; i < n; i++){
        for(int j = 0; j<n;j++){
            v[i][j] = 1 + (drand48() * 100);
        }
    }
    return v;
}

int main(int argc, char const *argv[])
{
    int n;
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


    for ( int k = 0; k < n;k++){ // For each column 
        double m = 0;
        int ind = 0;
        for (int i = k;i<n;i++){
            if (m < v[i][k]){
                m = v[i][k];
                ind = i;
            }
        }
        if (m == 0){
            cerr <<"Singular Matrix!";
        }
        double t = p[ind];
        p[ind] = p[k];
        p[k] = t;

        vector<double> temp = v[ind];
        v[ind] = v[k];
        v[k] = temp;

        for ( int i = 0; i<k-1;i++){
            double t = l[k][i];
            l[k][i] = l[ind][i];
            l[k][i] = t;
        } 
        u[k][k] = v[k][k];

        for (int i = k; i < n; i++){
            l[i][k] = v[i][k]/u[k][k];
            u[k][i] = v[k][i];
        }
        for (int i = k; i< n; i++){
            for(int j = k ; j< n; j++){
                v[i][j] = v[i][j] - (l[i][k] * u[k][j]);
            }
        }
    }
    print_vector(v);
    cout<<"---"<<endl;
    print_vector(l);
    cout<<"---"<<endl;
    print_vector(u);
    cout<<"---"<<endl;

    return 0;
}
