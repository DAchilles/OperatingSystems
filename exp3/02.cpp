#include <iostream>
using namespace std;

#define DEBUG

int page_num;       //最大能保存的虚拟块
int page_size;      //每一块能保存的指令数
int command_num;    //指令数量
int fifo_index=0;   //先进先出栈顶
int missing_cnt=0;  //缺页次数
int hit_cnt=0;      //命中次数

//虚拟块
struct v_page{
    bool visit;     //是否被使用
    int command;    //首指令地址
    int in_time;    //进入页表时间
    int last_time;  //上次使用时间
    void init()
    {
        visit = false;
        command = -1;
        in_time = 0;
        last_time = 4000;
    }
} page[16];

//物理块
struct p_block
{
    int index;
    int command;
} commands[2048];

void swap(p_block &a, p_block &b)
{
    int x=a.index, y=a.command;
    a.index = b.index, a.command = b.command;
    b.index = x, b.command = y;
}

//初始化内存空间
void init_pages(v_page *v, int n)
{
    for(int i=0; i<n; i++)
        page[i].init();
}

//生成指令集
void init_commands(p_block *p, int n)
{
    srand((unsigned)time(NULL));
    for(int i=0; i<n; i++)
    {
        p[i].index = i;
        p[i].command = rand()%command_num;
    }
}

//输出内存信息
void print_pages()
{
    for(int i=0; i<page_num; i++)
    {
        if(page[i].visit)
            cout <<page[i].command <<'\t';
        else
            cout <<"NULL\t";
    }
    cout <<endl;
}

//乱序
void shuffle(p_block *p, int n)
{
    for(int i=n-1; i>=1; i--)
    {
        srand((unsigned) time(NULL));
        int index=rand()%(i+1);
        
        swap(p[i], p[index]);
    }
}

//是否命中
int is_exist(v_page *v, int command)
{
    for(int i=0; i<page_num; i++)
    {
        if(v[i].visit && command/page_size == v[i].command)
            return i;
    }
    return -1;
}

//FIFO
void FIFO()
{
    for(int i=0; i<command_num; i++)
    {
        cout <<"Command[" <<commands[i].command <<"]\tIndex[" <<commands[i].index <<"]\n";
        //判断指令是否已经加载进内存
        int flag=is_exist(page, commands[i].index);
        //已在内存中
        if(flag != -1)
        {
            cout <<"Hit!\t\t";
            hit_cnt++;
        }
        //不在内存则加载进内存
        else
        {
            cout <<"Missing!\t";
            missing_cnt++;
            
            int index=commands[i].index/page_size;
            page[fifo_index].visit = true;
            page[fifo_index].command = index;
            fifo_index = (fifo_index+1)%page_num;
        }
        //输出内存信息
        print_pages();
        
    }
}

void LRU()
{
    for(int i=0; i<command_num; i++)
    {
        cout <<"Command: [" <<commands[i].command <<"]\tIndex: [" <<commands[i].index <<"]\n";
        //判断指令是否已经加载进内存
        int flag=is_exist(page, commands[i].index);
        //已在内存中
        if(flag != -1)
        {
            cout <<"Hit!\t\t";
            hit_cnt++;
            page[flag].last_time=0;
        }
        //不在内存则加载进内存
        else
        {
            cout <<"Missing!\t";
            missing_cnt++;

            //查找替换位置
            int max_time=-1;
            int index=0;
            for(int j=0; j<page_num; j++)
            {
                if(!page[j].visit)
                {
                    index=j;
                    break;
                }
                else
                {
                    if(page[j].last_time>max_time)
                    {
                        max_time = page[j].last_time;
                        index = j;
                    }
                } 
            }

            page[index].visit = true;
            page[index].command = commands[i].index/page_size;
            page[index].last_time = 0;
            page[index].in_time = 0;
        }
        for(int j=0; j<page_num; j++)
            if(page[j].visit)
                page[j].last_time++;
        //输出内存信息
        print_pages();
    }
}

void OPT()
{
    for(int i=0; i<command_num; i++)
    {
        cout <<"Command: [" <<commands[i].command <<"]\tIndex: [" <<commands[i].index <<"]\n";
        //判断指令是否已经加载进内存
        int flag=is_exist(page, commands[i].index);
        //已在内存中
        if(flag != -1)
        {
            cout <<"Hit!\t\t";
            hit_cnt++;
        }
        //不在内存中
        else
        {
            cout <<"Missing!\t";
            missing_cnt++;

            //确定替换位置
            int index = 0;
            int max_next = -1;
            for(int j=0; j<page_num; j++)
            {
                //如果还有空位
                if(!page[j].visit)
                {
                    index = j;
                    break;
                }
                //没有空位则找代替位
                else
                {
                    int k;
                    int temp=commands[i].index/page_size;
                    for(k=i+1; k<command_num; k++)
                    {
                        if(commands[k].index/page_size == temp && k-i>max_next)
                        {
                            index = j;
                            max_next = k-i;
                            break;
                        }
                    }
                    if(k==command_num)
                    {
                        index = j;
                        max_next = command_num;
                    }
                }
            }

            page[index].visit = true;
            page[index].command = commands[i].index/page_size;
            page[index].last_time = 0;
            page[index].in_time = 0;
        }
        //输出内存信息
        print_pages();
    }
}

int main()
{
    string choice;
    
    while(1)
    {
        cout <<"How many pages[3~16]: ";
        cin >>page_num;
        if(page_num>=3 && page_num <=8)
            break;
        cout <<"Illegal input!" <<endl;
    }

    while(1)
    {
        cout <<"How big each page[1~16]: ";
        cin >>page_size;
        if(page_size>0 && page_size<=16)
            break;
        cout <<"Illegal input!" <<endl;
    }

    while(1)
    {
        cout <<"How many commands(Multiples of page_size): ";
        cin >> command_num;
        if(command_num >0 && command_num%page_size==0)
            break;
        cout <<"Illegal input!" <<endl;
    }
    
    init_commands(commands, command_num);
    init_pages(page, page_num);

#ifdef DEBUG
    commands[0].index=3;
    commands[1].index=4;
    commands[2].index=2;
    commands[3].index=6;
    commands[4].index=4;
    commands[5].index=3;
    commands[6].index=7;
    commands[7].index=4;
    commands[8].index=3;
    commands[9].index=6;
    commands[10].index=3;
    commands[11].index=4;
    commands[12].index=8;
    commands[13].index=4;
    commands[14].index=6;

    command_num=15;
    page_num=3;
    page_size=1;
#endif

    while(1)
    {
        cout <<"Which replacement algorithm[FIFO/LRU/OPT]: ";
        cin >>choice;

        if(choice=="FIFO")
        {
            //shuffle(commands, command_num);
            FIFO();
            break;
        }
        else if(choice=="LRU")
        {
            LRU();
            break;
        }
        else if(choice=="OPT")
        {
            OPT();
            break;
        }
        else
            cout <<"Illegal input!" <<endl;
    }   

    cout <<endl;
    cout <<"Hit times: " <<hit_cnt <<endl;
    cout <<"Minssing times: " <<missing_cnt <<endl;
    cout <<"Minssing rate: " <<(double)(command_num-hit_cnt)/command_num*100 <<"%\n";

    return 0;
}