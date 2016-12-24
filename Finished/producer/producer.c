# include "apilib.h"
#define BUFFER_SIZE 10
void  HariMain(void){

  int in=0,i;
  int num=0;
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
    while ( (counter=api_var_read("counter",1))== BUFFER_SIZE){
    }
    temp=in;
    outcome=rand();
    api_entrance(0);
    api_var_wrt("buffer",in+1,outcome);
    in = (in + 1)%BUFFER_SIZE;
    counter++;
    api_var_wrt("counter",1,counter);
    api_exit(0);
    sprintf(s,"in buffer %d,produce %d\n",temp+1,outcome);
    api_putstr0(s);
  }
  api_end();
}
