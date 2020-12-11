#include <iostream>
#include <time.h>
#define M 2048
#define N 2048
using namespace std;

int a[M][N];
int b[M][N];

int main()
{
    clock_t start1 = clock();
    for(int i=0; i<M; i++)
        for(int j=0; j<N; j++)
            a[i][j]=0;
    clock_t end1 = clock();
    cout <<"time1: " <<end1-start1 <<"ms" <<endl;

    clock_t start2 = clock();
    for(int i=0; i<M; i++)
        for(int j=0; j<N; j++)
            b[j][i]=0;
    clock_t end2 = clock();
    cout <<"time2: " <<end2-start2 <<"ms" <<endl;

    return 0;
}