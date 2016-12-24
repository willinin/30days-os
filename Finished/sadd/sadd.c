# include "apilib.h"

void  HariMain(void){
  int num,temp,x,outcome;
  char *s;
  if(api_var_create("svar",1)==1)//如果创建成功
    api_var_wrt("svar",1,0);//svar=0；
  if(api_var_create("snum",1)==1)//如果创建成功
    api_var_wrt("snum",1,0);//snum=0;
  if(api_var_read("snum",1)==0)//if（snum==0）
    api_var_wrt("svar",1,0);//svar=0;
  num=api_var_read("snum",1)+1;
  api_var_wrt("snum",1,num);//snum+=1
  while(api_var_read("snum",1)<2)
  ;
  for(;;)
  {
    api_lock();
    temp=api_var_read("svar",1);//temp=svar
    num=api_var_read("svar",1)+1;
    api_var_wrt("svar",1,num);//svar+=1
    x=api_var_read("svar",1);
    outcome=x-temp;
    api_unlock();
    if(outcome>1)
    {
      sprintf(s,"share=%d,but share+1=%d\n",temp,x);
      api_putstr0(s);
      num=api_var_read("snum",1)-1;
      api_var_wrt("snum",1,num);//snum-=1
      break;
    }
  }
  api_end();
}
