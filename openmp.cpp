#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>
#include <chrono> 
#include <time.h>

using namespace std::chrono; 
using namespace std;

int n ;

// typedef vector<vector<double> > MATRIX;

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
    // int n;
    n  = stoi(argv[1]);
    // n = 4;
    
    MATRIX v,l(n,vector<double>(n,0)),u(n,vector<double>(n,0));
    for(int i = 0; i < n; i++){
        l[i][i] = 1;
    }
    
    vector<int> p(n,0);
    for(int i = 0; i < n ;i++){
        p[i] = i+1;
    }

    v = generateRandom(n);
    
    // n = 3;
    // v = generateRandom(4);
    // v[0][0] = 1.00;    v[0][1] = 2.00;    v[0][2] = 3.00; v[0][3] = 4.0;
    // v[1][0] = 5.00;    v[1][1] = 6.00; v[1][2] = 7.00; v[1][3] = 8.0;
    // v[2][0] = 9.00;    v[2][1] = 10.00; v[2][2] = 11.00; v[2][3] = 12.0;
    // v[3][0] = 13.00;    v[3][1] = 14.00; v[3][2] = 15.00; v[3][3] = 16.0;
    // print_vector(v);
    // cout<<"\n\n";
    MATRIX vv = v;


    for ( int k = 0; k < n;k++){ // For each column 
        // cout<<"-----------------------\n";
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
        // cout<<ind<<endl;
        
        vector<double> temp = v[ind];
        v[ind] = v[k];
        v[k] = temp;
        // print_vector(v);
        // cout<<"---a"<<endl;

        // if(k == 1){
        //     double tm = l[k][0];
        //     l[k][0] = l[ind][0];
        //     l[ind][0] = tm;
        // }else{
            for ( int i = 0; i<=k-1;i++){
            // cout<<"Mayank\n";
                double tm = l[k][i];
                l[k][i] = l[ind][i];
                l[ind][i] = tm;
            }
        // }
        
        // while(i)
        // print_vector(l);
        // cout<<"---l"<<endl; 

        u[k][k] = v[k][k];

        // print_vector(u);
        // cout<<"---u"<<endl;

        for (int i = k+1; i < n; i++){
            l[i][k] = v[i][k]/u[k][k];
            u[k][i] = v[k][i];
        }
        // print_vector(l);
        // cout<<"---l"<<endl;
        // print_vector(u);
        // cout<<"---u"<<endl;
        for (int i = k+1; i< n; i++){
            for(int j = k+1 ; j< n; j++){
                v[i][j] = v[i][j] - (l[i][k] * u[k][j]);
            }
        }
        // print_vector(v);
        // cout<<"---a"<<endl;
        // print_vector(l);
        // cout<<"---l"<<endl;
        // print_vector(u);
        // cout<<"---u"<<endl<<endl;
        // cout<<"==============================="<<k;
    }
    auto stop = high_resolution_clock::now();
    // print_vector(v);
    // cout<<"---"<<endl;
    // print_vector(l);
    // cout<<"---"<<endl;
    // print_vector(u);
    // cout<<"---"<<endl;
    MATRIX pp = generateRandom(p.size());
    for(int i = 0; i<p.size(); i++){
        // cout<<"("<<i<<")"<<endl;
        for(int j = 0; j<p.size(); j++){
            if(p[i] == j+1){
                pp[i][j] = 1;
                // cout<<"1 ";
            }else{
                pp[i][j] = 0;
                // cout<<"0 ";
            }
        }
        // cout<<endl;
    }
    // cout<<"---"<<endl;
    // MATRIX temp = matmul(pp, vv);
    // print_vector(temp);
    // cout<<"---"<<endl;
    // temp = matmul(l, u);
    // print_vector(temp);

    cout<<l2norm(matmul(pp, vv), matmul(l, u))<<endl;

    auto duration = duration_cast<microseconds>(stop - start); 
  
    // To get the value of duration use the count() 
    // member function on the duration object 
    cout<< "--" << duration.count() << endl; 

    return 0;
}
