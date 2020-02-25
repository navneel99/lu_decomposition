#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

int main(){
    string l = "Lower.txt";
    string u = "Upper.txt";
    string p = "Permutation.txt";
    // string l = "Lowerop.txt";
    // string u = "Upperop.txt";    
    // string p = "Permutationop.txt";
    int n = 100;
    float L[n][n], U[n][n], P[n][n], A[n][n];

    ifstream myfile;
    myfile.open(l);
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            double a;
            myfile>>a;
            L[i][j] = a;
            // cout<<a<<" ";
        }
        // cout<<endl;
    }
    myfile.close();

    myfile.open(u);
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            double a;
            myfile>>a;
            U[i][j] = a;
        }
    }
    myfile.close();

    myfile.open(p);
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            double a;
            myfile>>a;
            P[i][j] = a;
        }
    }
    myfile.close();

    myfile.open("B_100.txt");
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            double a;
            myfile>>a;
            A[i][j] = a;
        }
    }
    myfile.close();

    double LU[n][n];
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            double s = 0;
            for(int k = 0; k<n; k++){
                s+= L[j][k]*U[k][i];
            }
            LU[i][j] = s;
        }
    }

    double PA[n][n];
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            double s = 0;
            for(int k = 0; k<n; k++){
                s+= P[j][k]*A[k][i];
            }
            PA[i][j] = s;
        }
    }

    double ans = 0;
    for(int j = 0; j<n; j++){
        double sum = 0; 
        for(int i = 0; i<n; i++){
            sum += (PA[i][j]-LU[i][j])*(PA[i][j]-LU[i][j]);
        }
        ans += sqrt(sum);
    }

    cout<<ans<<endl;
}