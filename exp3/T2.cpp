#include <iostream>
#include <algorithm>
#include <cstdio>
#define COMMAND_NUM 160
#define PAGE_CONTENT 4
#define FALSE -1
#define DEBUG
using namespace std;

//指令集合结构体
struct commandList{
    int command;        //10条指令的集合，代表所在的虚拟页页号
    int offset;         //指令偏移地址
    int lastTimeHit;    //最后一次命中的时间
    int enterNum;       //模拟进入的顺序
};

//物理内存页
struct physicalPage{
    int command;//装入的指令集,-1代表没有装入
};

//物理页数
int physicalPageNum;
//虚拟页，int存储的是虚拟页中的指令，该指令随机产生
int* mVirtualPages;
//物理页
physicalPage* mPhysicalPage;
//指令集
commandList* mCommandList;
//缺页次数
int disaffect = 0;
//缺页率
float disaffectRate;

//随机数模拟虚拟页中的指令
void initVirtualPage(int * vp)
{
    srand((unsigned)time(NULL));
    for (int i=0; i<COMMAND_NUM; i++)
    {
        vp[i] = rand()% COMMAND_NUM;
    //  printf("%d  ", vp[i]);
    }
}

//初始化指令集合
void initCommandList(commandList* cl)
{
    for (int i=0; i<COMMAND_NUM; i++)
    {
        //方便起见，指令集和下标即为该指令
        (cl + i)->lastTimeHit = -1;
        (cl + i)->command = i / PAGE_CONTENT;
        (cl + i)->offset = i % PAGE_CONTENT;
        (cl + i)->enterNum = 0;
    }
}

//初始化物理内存页
void initPhysicalPage(physicalPage* pp){
    for (int i=0; i<physicalPageNum; i++)
    {
        (pp+i)->command = -1;
    }
}

//是否命中
int isExistInPhysicalPage(int command, physicalPage*pp, int length){
    for (int i = 0; i < length; i++)
    {
        if ((pp + i)->command == command) return i;
    }
    return FALSE;
}

//FIFO淘汰策略
int whichIsToSelectFIFO(physicalPage*pp, int length){
    int returnValue = 0;
    int target = (mCommandList + pp->command*PAGE_CONTENT)->enterNum;
    for (int i = 0; i < length; i++)
    {
        //若物理内存为空，则直接返回该位置
        if ((pp + i)->command == -1) return i;
        //若不为空，则返回最先进入内存的页面
        else
        {
            int tmp = (mCommandList + (pp + i)->command*PAGE_CONTENT)->enterNum;
            if (tmp < target){
                target = tmp;
                returnValue = i;
            }
        }
    }
    return returnValue;
}

//FIFO模拟
void FIFO(){
    for (int i = 0; i < COMMAND_NUM; i++)
    {
        int command =* (mVirtualPages+i)/PAGE_CONTENT;
#ifdef DEBUG
        printf("---------------------------------------------------------------------------\n");
        printf("指令%d请求分配内存,", *(mVirtualPages + i));
#endif
        //是否存在物理页？若存在，返回所在位置，否则返回-1
        int index = isExistInPhysicalPage(command, mPhysicalPage, physicalPageNum);
        // 若不存在
        if (index == FALSE){
            disaffect++;
            /*模拟执行装入内存*/
            int targetIndex = whichIsToSelectFIFO(mPhysicalPage, physicalPageNum);
            //写入
            (mPhysicalPage + targetIndex)->command = command;
#ifdef DEBUG
            printf("物理内存%d上的指令被淘汰,取而代之的是%d\n", targetIndex, *(mVirtualPages + i));
#endif
            //更新使用情况
            (mCommandList + command*PAGE_CONTENT)->enterNum = i+1;
            (mCommandList + command*PAGE_CONTENT)->lastTimeHit = i;
        }
        //否则直接更新
        else
        {
#ifdef DEBUG
            printf("该指令已经存在内存中！\n");
#endif
            (mCommandList + command*PAGE_CONTENT)->lastTimeHit = i;
        }
#ifdef DEBUG
        printf("内存使用情况:\n");
        //输出内存使用情况
        for (int k = 0; k < physicalPageNum; k++)
        {
            printf(" %d ", (mPhysicalPage + k)->command);
        }
        printf("\n");
#endif
    }
}

//LUR淘汰策略
int whichIsToSelectLRU(physicalPage*pp, int length){
    int returnValue = 0;
    int target = (mCommandList + pp->command*PAGE_CONTENT)->lastTimeHit;
    for (int i = 0; i < length; i++)
    {
        //若物理内存为空，则直接返回该位置
        if ((pp + i)->command == -1) return i;
        //若不为空，则返回最先进入内存的页面
        else
        {
            int tmp = (mCommandList + (pp + i)->command*PAGE_CONTENT)->lastTimeHit;
            if (tmp < target){
                target = tmp;
                returnValue = i;
            }
        }
    }
    return returnValue;
}

//LUR模拟
void LRU(){
    for (int i = 0; i < COMMAND_NUM; i++)
    {
        int command = *(mVirtualPages + i)/PAGE_CONTENT;

#ifdef DEBUG
        printf("---------------------------------------------------------------------------\n");
        printf("指令%d请求分配内存,", *(mVirtualPages+i));
#endif

        //是否存在物理页？若存在，返回所在位置，否则返回-1
        int index = isExistInPhysicalPage(command, mPhysicalPage, physicalPageNum);
        // 若不存在
        if (index == FALSE){
            disaffect++;
            /*模拟执行装入内存*/
            int targetIndex = whichIsToSelectLRU(mPhysicalPage, physicalPageNum);
            //写入
            (mPhysicalPage + targetIndex)->command = command;
#ifdef DEBUG
            printf("物理内存%d上的指令被淘汰,取而代之的是%d\n", targetIndex, *(mVirtualPages + i));
#endif
            //更新使用情况
            //
            (mCommandList + command*PAGE_CONTENT)->enterNum = i + 1;
            (mCommandList + command*PAGE_CONTENT)->lastTimeHit = i;
        }
        //存在内存中直接更新
        else
        {
#ifdef DEBUG
            printf("该指令已经存在内存中！\n");
#endif
            (mCommandList + command*PAGE_CONTENT)->lastTimeHit = i;
        }
#ifdef DEBUG
        printf("内存使用情况:\n");
        //输出内存使用情况
        for (int k = 0; k < physicalPageNum; k++)
        {
            printf(" %d ", (mPhysicalPage + k)->command);
        }
        printf("\n");
#endif
    }
}

//OPT淘汰策略
int whichIsToSelectOPT(physicalPage*pp, int length,int startNum){
    int returnValue = 0;
    int target=0;
    int tmp=-1;
    for (int i = startNum; i < COMMAND_NUM; i++){
        //若找到与当前内存相同的最近的指令 
        if (pp->command == *(mVirtualPages + i) / PAGE_CONTENT) {
            target = i - startNum + 1;
            break;
        }
        else
        {
            //否则是最远距离
            target = COMMAND_NUM - startNum + 1;
        }
    }
    //第一个已经考虑过了
    for (int i = 1; i < length; i++)
    {
        //若物理内存为空，则直接返回该位置
        if ((pp + i)->command == -1) return i;
        //若不为空，则返回最先进入内存的页面
        else
        {
            for (int k= startNum; k< COMMAND_NUM; k++){
                //若找到与当前内存相同的最近的指令 
                if ((pp+i)->command == *(mVirtualPages + k) / PAGE_CONTENT) {
                    tmp = k- startNum + 1;
                    break;
                }
                else
                {
                    //否则最远距离 
                    tmp = COMMAND_NUM - startNum + 1;
                }
            }
            if (tmp >target){
                target = tmp;
                returnValue = i;
            }
        }
    }
    return returnValue;
}

//OPT模拟
void OPT(){
    for (int i = 0; i < COMMAND_NUM; i++)
    {
        int command = *(mVirtualPages + i) / PAGE_CONTENT;

#ifdef DEBUG
        printf("---------------------------------------------------------------------------\n");
        printf("指令%d请求分配内存,", *(mVirtualPages + i));
#endif
        //是否存在物理页？若存在，返回所在位置，否则返回-1
        int index = isExistInPhysicalPage(command, mPhysicalPage, physicalPageNum);
        // 若不存在
        if (index == FALSE){
            //缺页
            disaffect++;
            int targetIndex;
            /*模拟执行装入内存*/
            //若已经到达数列末尾
            if (i + 1 == COMMAND_NUM) targetIndex = 0;
            else    targetIndex= whichIsToSelectOPT(mPhysicalPage, physicalPageNum,i+1);
            //写入
            (mPhysicalPage + targetIndex)->command = command;
#ifdef DEBUG
            printf("物理内存%d上的指令被淘汰,取而代之的是%d\n", targetIndex, *(mVirtualPages + i));
#endif
            //更新使用情况
            (mCommandList + command)->enterNum = i + 1;
            (mCommandList + command)->lastTimeHit = i;
        }
        //否则直接更新
        else
        {
#ifdef DEBUG
            printf("该指令已经存在内存中！\n");
#endif
            (mCommandList + command)->lastTimeHit = i;
        }
#ifdef DEBUG
        printf("内存使用情况:\n");
        //输出内存使用情况
        for (int k = 0; k < physicalPageNum; k++)
        {
            printf(" %d ", (mPhysicalPage + k)->command);
        }
        printf("\n");
#endif
    }
}

int main()
{
    while (true)
    {
        while (true)
        {
            puts("请输入物理页数:");
            scanf("%d", &physicalPageNum);
            //初始化全局变量
            if (physicalPageNum < 4 || physicalPageNum>32)
            {
                puts("输入错误，请输入大于4小于32的整数");
            }
            else
            {
                break;
            }
        }
        mPhysicalPage = (physicalPage*)malloc(physicalPageNum*sizeof(physicalPage));
        //这里模拟32个不同的指令集合
        mCommandList = (commandList*)malloc(COMMAND_NUM*sizeof(commandList));
        mVirtualPages = (int*)malloc(COMMAND_NUM*sizeof(int));
        initCommandList(mCommandList);
        initPhysicalPage(mPhysicalPage);
        initVirtualPage(mVirtualPages);
        disaffect = 0;
        printf("FIFO策略:\n");
        FIFO();
        disaffectRate = (float)disaffect / (float)COMMAND_NUM;
        printf("命中率为%f\n", 1 - disaffectRate);
        //重置某些数据
        initCommandList(mCommandList);
        initPhysicalPage(mPhysicalPage);
        disaffect = 0;
        printf("\n\nLRU策略:\n");
        LRU();
        disaffectRate = (float)disaffect / (float)COMMAND_NUM;
        printf("命中率为%f\n", 1 - disaffectRate);
        //重置某些数据
        initCommandList(mCommandList);
        initPhysicalPage(mPhysicalPage);
        disaffect = 0;
        printf("\n\nOPT策略:\n");
        OPT();
        disaffectRate = (float)disaffect / (float)COMMAND_NUM;
        printf("命中率为%f\n", 1 - disaffectRate);
    }
    //system("pause");
    return 0;
}