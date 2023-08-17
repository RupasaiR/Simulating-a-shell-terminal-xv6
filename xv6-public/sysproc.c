#include "types.h"
#include "x86.h"
#include "defs.h"

#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
	int status;
 if( argint(0, &status) < 0 ){
   return -1;
 }
  exit(status);
  return 0;  // not reached
}

int
sys_wait(void)
{
int *p;
 if(argptr(0, (void*)&p, sizeof(int))<0){
   return -1;
 }
return wait(p);
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;
  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  myproc()->hz+=n;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//prints process info
int 
sys_ps(void)
{
	return ps();
}

int 
sys_procinfo(void)
{
	int pid;
	if(argint(0,&pid)<0)
		return -1;
	return procinfo(pid);
}

//prints helloworld and return number
int
sys_helloWorld(void)
{
  cprintf("%s","Hello World\n");
  return 0;
}

//returns number of openfiles
int
sys_numOpenFiles(void)
{
  int cnt=0;
  struct proc *p=myproc();
  for(int i=0;i<NOFILE;i++){
  if(p->ofile[i])
    cnt+=1;
   
  }
  return cnt;
}
int
sys_memAlloc(void)
{
  struct proc *p=myproc(); 
  return p->hz;
}

int
sys_getprocesstimedetails(void)
{
  struct proc *p=myproc(); 
  if(p)
  {
  cprintf("processCreationDateTime: %d:%d:%d %d:%d:%d\n",p->creation_time.second,p->creation_time.minute,p->creation_time.hour,p->creation_time.day,p->creation_time.month,p->creation_time.year);
  cprintf("processLastContextSwitchedOutDateTime: %d:%d:%d %d:%d:%d\n",p->swtcout_time.second,p->swtcout_time.minute,p->swtcout_time.hour,p->swtcout_time.day,p->swtcout_time.month,p->swtcout_time.year);
  cprintf("processLastContextSwitchedInDateTime: %d:%d:%d %d:%d:%d\n",p->swtchin_time.second,p->swtchin_time.minute,p->swtchin_time.hour,p->swtchin_time.day,p->swtchin_time.month,p->swtchin_time.year);
  return 1;
  }
  return -1;
}

