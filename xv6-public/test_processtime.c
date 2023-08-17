#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


int main(int argc, char *argv[])
{
  getprocesstimedetails();
  
  int pid=fork();
  if(pid==0)
  {
  printf(1,"child\n");
  sleep(1000);
  exit(0);
  }
  else
  {
  wait(0);
  printf(1,"parent\n");
  printf(1,"After waiting\n");
  getprocesstimedetails();
  }
  exit(0);
}
