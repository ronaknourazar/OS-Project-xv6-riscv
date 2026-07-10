#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "pinfo.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
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

uint64
sys_getpinfo(void)
{
  uint64 upinfo;
  struct pinfo opinfo;
  struct proc *p;
  // counter to the element we are processing
  int i = 0;

  argaddr(0, &upinfo);
  if (upinfo == 0)
    return -1;

  extern struct proc proc[];

  for (p = proc; p < &proc[NPROC]; p++) {
    // lock the process
    acquire(&p->lock);

    opinfo.pid = p->pid;
    opinfo.state = p->state;
    opinfo.priority = p->priority; 
    opinfo.tickets = p->tickets;

    // if a process is unused, set id to 0
    if (p->state == UNUSED)
      opinfo.pid = 0;

    // copying the kernel results in the user space (checking if it fails)
    // first parameter -> pointing to the pagetable of the process
    // second parameter -> pointing to which element of the array we are currently processing
    // third parameter -> address of the data we are copying
    // forth parameter -> number of bytes we have to copy
    if (copyout(myproc()->pagetable, upinfo + i * sizeof(struct pinfo), (char *)&opinfo, sizeof(struct pinfo)) < 0) {
        release(&p->lock);
        return -1;
      }
    // unlock the process
    release(&p->lock);
    i++;
  }

  return 0;
}

uint64
sys_setpriority(void) {
  int pid;
  int priority;

  argint(0, &pid);
  argint(1, &priority);

  if (priority < 0 || priority > 100)
    return -1;

  struct proc *p;
  extern struct proc proc[];

  // search for the process with the same ID as input from user
  // then set its priority
  for (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if (p->pid == pid) {
      p->priority = priority;
      release(&p->lock);
      return 0;
    }
    release(&p->lock);
  }
  // system call was unsuccessful
  return -1;
}

uint64
sys_settickets(void) {
  int tickets;

  argint(0, &tickets);

  if (tickets < 0)
    return -1;

  // get current process
  struct proc *p = myproc();
  // lock process, set new ticket number, release process
  acquire(&p->lock);
  p->tickets = tickets;
  release(&p->lock);

  return 0;

}
