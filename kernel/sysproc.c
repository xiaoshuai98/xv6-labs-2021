#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
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


#ifdef LAB_PGTBL
uint64 sys_pgaccess(void) {
  uint64 start_addr;
  int num_pages;
  uint64 user_bitmask;
  struct proc *p = myproc();

  if (argaddr(0, &start_addr) < 0 || argint(1, &num_pages) < 0 ||
      argaddr(2, &user_bitmask) < 0) {
    return -1;    
  }

  uint64 kern_bitmask;
  memset((void*)&kern_bitmask, 0, sizeof(uint64));

  start_addr = PGROUNDDOWN(start_addr);
  for (int i = 0; i < num_pages; i++) {
    uint64 va = start_addr + i * PGSIZE;
    if (va >= MAXVA) {
      break;
    }
    pte_t *pte = walk(p->pagetable, va, 0);
    if ((*pte & PTE_A) != 0) {
      kern_bitmask |= (1 << i);
      *pte = (*pte & ~PTE_A);
    }
  }

  if (copyout(p->pagetable, user_bitmask, (char*)&kern_bitmask, sizeof(uint64)) < 0) {
    return -1;
  }
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
