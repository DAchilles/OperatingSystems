#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

#define N 5

int philosopher_id=0;           //哲学家ID
sem_t chopstick_sem[N];         //同步信号量
pthread_mutex_t mutex[N];       //互斥信号量

 
//哲学家
void *philosopher(void *arg)
{
	//哲学家编号
	int id = philosopher_id++;
	while (1)
	{
		//哲学家开始了思考
        cout <<"Philosopher" <<id <<" start thinking" <<endl;
        sleep(1);
        
        //哲学家停止了思考
        cout <<"Philosopher" <<id <<" start break" <<endl;
        sleep(1);
		
        //奇数号哲学家先拿右手的筷子
        if(id%2)
        {   
            //哲学家拿起了右手的筷子
            sem_wait(&chopstick_sem[(id+1)%N]);
            cout <<"philosopher" <<id <<" picks up chopstick" <<(id+1)%N <<endl;
            //哲学家拿起了左手的筷子
            sem_wait(&chopstick_sem[id]);
            cout <<"philosopher" <<id <<" picks up chopstick" <<id <<endl;
            
            //哲学家开始恰饭
            cout <<"philosopher" <<id <<" start eating" <<endl;
            sleep(1);
            
            //哲学家放下了左手的筷子
            sem_post(&chopstick_sem[id]);
            cout <<"philosopher" <<id <<" puts down chopstick" <<id <<endl;
            //哲学家放下了右手的筷子
            sem_post(&chopstick_sem[(id+1)%N]);
            cout <<"philosopher" <<id <<" puts down chopstick" <<(id+1)%N <<endl;
        }
        //偶数号哲学家先拿左手筷子
        else
        {
            //哲学家拿起了左手的筷子
            sem_wait(&chopstick_sem[id]);
            cout <<"philosopher" <<id <<" picks up chopstick" <<id <<endl;
            //哲学家拿起了右手的筷子
            sem_wait(&chopstick_sem[(id+1)%N]);
            cout <<"philosopher" <<id <<" picks up chopstick" <<(id+1)%N <<endl;
            
            //哲学家开始恰饭
            cout <<"philosopher" <<id <<" start eating" <<endl;
            //sleep(1);
            
            //哲学家放下了右手的筷子
            sem_post(&chopstick_sem[(id+1)%N]);
            cout <<"philosopher" <<id <<" puts down chopstick" <<(id+1)%N <<endl;
            //哲学家放下了左手的筷子
            sem_post(&chopstick_sem[id]);
            cout <<"philosopher" <<id <<" puts down chopstick" <<id <<endl;
        }
	}
}
 
int main()
{
	pthread_t id[N];
	
	//初始化同步信号量
    for(int i=0; i<N; i++)
    {
        int flag=sem_init(&chopstick_sem[i], 0, 1);
        if(flag)
        {
            printf("sem init failed \n");
		    exit(1);
        }
    }

    //初始化互斥信号量
    for(int i=0; i<N; i++)
    {
        int flag=pthread_mutex_init(&mutex[i], NULL);
        if(flag)
        {
            printf("mutex init failed \n");
		    exit(1);
        }
    }

	// 创建哲学家线程
	for(int i=0; i<N; i++)
	{
		int flag=pthread_create(&id[i], NULL, philosopher, (void *) (&i));
		if (flag)
		{
			printf("philosopherr%d creation failed \n", i);
			exit(1);
		}
	}
	
	//等待线程结束
	for(int i=0;i<N;i++)
		pthread_join(id[i],NULL);
	
	exit(0);
}