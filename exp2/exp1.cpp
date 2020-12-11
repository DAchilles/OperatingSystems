#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
using namespace std;
int main()
{
    int statu;
    pid_t pid;
    while((pid=fork())<0)
        ;
    //子进程
    if(pid==0)
    {
        for(int i=0; i<10; i++)
            cout <<"I'm child, pid=" <<getpid() <<endl;
        sleep(5);
        exit(0);
    }
    //父进程
    else
    {
        wait(&statu);
        for(int i=0; i<10; i++)
            std::cout <<"I'm father, pid=" <<getpid() <<endl;
    }
    return 0;
}