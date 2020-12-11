#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>

//打印 1~1000
void *print1(void *)
{
    int  i = 0;
    while (i <= 1000)
    {
        printf("B: %d\n", i);
        i++;
        sleep(0.2);
    }
}

//打印1000~1
void *print2(void *)
{
    int i = 1000;
    while (i > 0)
    {
        printf("A: %d\n", i);
        i--;
        sleep(0.2);
    }
}

int main()
{
    pthread_t tips1;
    pthread_t tips2;
    int hThread1 = pthread_create(&tips1, NULL, print1, NULL);
    int hThread2 = pthread_create(&tips2, NULL, print2, NULL);
    
    if (hThread1 != 0)
        printf("hThread1 err");
    else if (hThread2 != 0)
        printf("hThread2 err!");

    void *result1;
    void *result2;
    pthread_join(tips1, &result1);
    pthread_join(tips2, &result2);
    
    return 0;
} 