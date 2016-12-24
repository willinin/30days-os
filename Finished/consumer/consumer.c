# include "apilib.h"
#define BUFFER_SIZE 10
void  HariMain(void){
  int num=0;
  int out=0,i;
  int counter=0;
  int temp,outcome;
  char *s;
  if(api_var_create("counter",1)==0)
    api_putstr0("The shared var counter has been created\n");
  else
    api_var_wrt("counter",1,counter);//counter=0
  if(api_var_create("buffer",10)==0)
    api_putstr0("The shared var buffer has been created\n");

  for(i=0;i<500;i++)
  {
    while ((counter=api_var_read("counter",1)) == 0){
    }
    temp=out;
    api_entrance(1);
    outcome=api_var_read("buffer",out+1);
    out = (out + 1)%BUFFER_SIZE;
    counter--;
    api_var_wrt("counter",1,counter);
    api_exit(1);
    sprintf(s,"in buffer %d,consume %d\n",temp+1,outcome);
    api_putstr0(s);
  }
  api_end();
}
