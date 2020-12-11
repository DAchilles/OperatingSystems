#include <iostream>
#include <cstdio>
#include <fstream>
#include <unistd.h>
#include <pthread.h>
using namespace std;

int main()
{
    string file="/proc/";
    string a;
    
    pid_t pid;
    while((pid=fork())<0)
        ;
    //子进程
    if(pid==0)
    {
        int p=getpid();
        file += to_string(p);
        file += "/pagemap";
        cout <<file;
/*
        ifstream inFile;
	    inFile.open(file,ios::in);
        while(!inFile.eof())
        {
            char ss[256];
            inFile.getline(ss, 256);
            cout <<ss <<endl;
        }
*/
    }
    else
    {
        int p=getpid();
        file += to_string(p);
        file += "/pagemap";
        cout <<file;
/*
        ifstream inFile;
	    inFile.open(file,ios::in);
        while(!inFile.eof())
        {
            char ss[256];
            inFile.getline(ss, 256);
            cout <<ss <<endl;
        }
*/
    }
    return 0;
}