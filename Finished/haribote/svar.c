#include "bootpack.h"
#include <stdio.h>
#include <string.h>

//--以下是关于实现用户态竞争条件的函数及变量
struct SVARCTL *svarctl;

void init_Svar(struct MEMMAN *memman)
{
	svarctl=(struct SVARCTL *)memman_alloc_4k(memman, sizeof (struct SVARCTL));
	int i;
	for(i=0;i<VAR_MAX_NUM;i++)
	{
		svarctl->var[i].flag=0;
		svarctl->var[i].length=0;
	}
	return;
}

int var_create(char *name,int length)//创建多少长度的int型的共享变量
{
	int i,j;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	//io_cli();
	for(j=0;j<VAR_MAX_NUM;j++)
	{
		if(svarctl->var[j].flag==1&&strcmp(svarctl->var[j].name,name)==0)//找到共享变量了
			{
				return 0;//共享变量重名
			}
	}
  for(i=0;i<VAR_MAX_NUM;i++)
	{
    if(svarctl->var[i].flag==0)//如果该位置还未使用
    {
      svarctl->var[i].flag=1;//已使用
      strcpy(svarctl->var[i].name,name);
      svarctl->var[i].length=length;
			//申请长度为length的内存空间地址
			svarctl->var[i].content=(int *)memman_alloc_4k(memman, sizeof (int)*length);
      return 1;
    }
  }
	//io_sti();
  return 0;
}

int var_read(char *name,int n)//读第n个int变量
{
	int i,j;
	int temp=0;
	//io_cli();
	for(i=0;i<VAR_MAX_NUM;i++)
	{
		if(svarctl->var[i].flag==1&&strcmp(svarctl->var[i].name,name)==0)//找到共享变量了
		  {
				if(n<=svarctl->var[i].length)
				{
					return svarctl->var[i].content[n-1];
				}
			}
	}
	//io_sti();
	return 0;//没找到共享变量则返回空
}

int var_wrt(char *name,int n,int value)//给共享变量的第n个字节写内容
{
  int i;
	//io_cli();
  for(i=0;i<VAR_MAX_NUM;i++)
  {
    if(svarctl->var[i].flag==1&&strcmp(svarctl->var[i].name,name)==0)//找到共享变量了
    {
      if(n>svarctl->var[i].length)//如果写的长度越界了
         return 0;
			else
			{
			svarctl->var[i].content[n-1]=value;
      return 1;
		  }
		}
  }
	//io_sti();
  return 0;
}

int var_free(char *name)//释放共享变量
{
  int i;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	//io_cli();
  for(i=0;i<VAR_MAX_NUM;i++)
  {
    if(svarctl->var[i].flag==1&&strcmp(svarctl->var[i].name,name)==0)//找到共享变量了
    {
  		svarctl->var[i].flag=0;
  		memman_free_4k(memman,svarctl->var[i].content,svarctl->var[i].length*4);
			svarctl->var[i].length=0;
      return 1;
    }
    return 0;
  }
	//io_sti();
}

void avoid_sleep()
{
	struct TASK *now_task;
	now_task=task_now();
	now_task->flags=2;
}

//tesk_and_set锁的实现
int lock=0;
int TestAndSet(int *t)
{
	io_cli();
	int rv=*t;
	*t=1;
	io_sti();
	return rv;
}

void Tlock()
{
	while(TestAndSet(&lock))
	;
}

void unTlock()
{
  lock=0;
}
