#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pinfo.h" // to include struct pinfo

int main(int argc, char *argv[]){
  // user space array we will be displaying
  struct pinfo upinfo[NPROC];

  // if syscall fails
  if (getpinfo(upinfo) < 0) {
    printf("Error: Syscall getpinfo was unsuccessful\n");
    exit(1);
  }

  printf("PID\tState\t\tPriority\tTickets\n");
  printf("=======================================================\n");

  for (int i = 0; i < NPROC; i++) {
    // if there is a process running:
    if (upinfo[i].pid > 0) {
      char *state_str;
      // convert index of the state to a string for display
      switch (upinfo[i].state) {
        case 1: state_str = "USED"; break;
        case 2: state_str = "SLEEPING"; break;
        case 3: state_str = "RUNNABLE"; break;
        case 4: state_str = "RUNNING"; break;
        case 5: state_str = "ZOMBIE"; break;
        default: state_str = "UNUSED"; break;
        }
        printf("%d\t%s\t\t%d\t\t%d\n", upinfo[i].pid, state_str, upinfo[i].priority, upinfo[i].tickets);
    }
  }
  exit(0);
}
