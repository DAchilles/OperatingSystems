#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;
#define N 3     // 消费者或者生产者的数目
#define M 10    // 缓冲数目
 
int in = 0;             // 生产者放置产品的位置
int out = 0;            // 消费者取产品的位置
int buff[M]={0};        // 缓冲初始化为0, 开始时没有产品
sem_t empty_sem;        // 同步信号量, 当满了时阻止生产者放产品
sem_t full_sem;         // 同步信号量, 当没产品时阻止消费者消费
pthread_mutex_t mutex;  // 互斥信号量, 一次只有一个线程访问缓冲
int producer_id = 0;    //生产者id
int consumer_id = 0;    //消费者id
 
/* 打印缓冲情况 */
void print()
{
    for(int i=0; i<M; i++)
        cout <<buff[i] <<"\t";
    cout <<endl;
}
 
/* 生产者 */
void *producer(void *arg)
{
    //生产者编号
    int id = ++producer_id;
    while (1)
    {
        //调节进程速度
        sleep(0.9);
        //P操作
        sem_wait(&empty_sem);
        //互锁
        pthread_mutex_lock(&mutex);
        //生成一个资源
        buff[in] = rand()%1000+id*1000;
        //输出
        printf("producer%d new No.%d: \t", id, in);
        print();
        //下一个资源位置
        in = (in+1) % M;
        //解锁
        pthread_mutex_unlock(&mutex);
        //V操作
        sem_post(&full_sem);
    }
}
 
/* 消费者 */
void * consumer(void *arg)
{
    //消费者编号
    int id = ++ consumer_id;
    while (1)
    {
        //调节进程速度
        sleep(1);
        //P操作
        sem_wait(&full_sem);
        //互锁
        pthread_mutex_lock(&mutex);
        //消费一个资源
        buff[out] = 0;
        //输出buffer区
        printf("consumer%d eat No.%d: \t", id, out);
        print();
        //下一个消费者位置
        out = (out+1) % M;
        //解锁
        pthread_mutex_unlock(&mutex);
        //V操作
        sem_post(&empty_sem);
    }
}
 
int main()
{
    pthread_t id1[N];
    pthread_t id2[N];
    int ret[N];
    
    // 初始化同步信号量
    int ini1 = sem_init(&empty_sem, 0, M);
    int ini2 = sem_init(&full_sem, 0, 0);
    if (ini1 && ini2 != 0)
    {
        printf("sem init failed \n");
        exit(1);
    }
 
    //初始化互斥信号量
    int ini3 = pthread_mutex_init(&mutex, NULL);
    if (ini3 != 0)
    {
        printf("mutex init failed \n");
        exit(1);
    }
 
    // 创建N个生产者线程
    for (int i = 0; i < N; i++)
    {
        ret[i] = pthread_create(&id1[i], NULL, producer, (void *) (&i));
        if (ret[i] != 0)
        {
            printf("producer%d creation failed \n", i);
            exit(1);
        }
    }
 
    //创建N个消费者线程
    for (int i = 0; i < N; i++)
    {
        ret[i] = pthread_create(&id2[i], NULL,  consumer, NULL);
        if (ret[i] != 0)
        {
            printf(" consumer%d creation failed \n", i);
            exit(1);
        }
    }
    
    //等待线程结束
    for(int i=0;i<N;i++)
    {
        pthread_join(id1[i],NULL);
        pthread_join(id2[i],NULL);
    }
    exit(0);
}