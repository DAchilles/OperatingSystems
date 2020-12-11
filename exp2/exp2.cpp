#include <iostream>
#include <unistd.h>
#include <wait.h>
using namespace std;
void print(int signum)
{
    cout <<"Bye world" <<endl;
    kill(getpid(),SIGKILL);
    return ;
}
int main()
{
    signal(SIGINT,print);
    pid_t pid;
    while((pid=fork())<0)
        ;
    //子进程
    if(pid==0)
    {
        while(1)
        {
            cout <<"I am Child Process, alive!" <<endl;
            sleep(2);
        }
    }
    //父进程
    else
    {
        while (1)
        {
            char choice;
            cout <<"To terminate Child Process. Yes or No?" <<endl;
            cin >>choice;
            if(choice == 'Y')
            {
                kill(pid, SIGINT);
                exit(0);
            }
            sleep(2);
        }
        
    }
    return 0;
}