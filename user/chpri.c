#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Not enough arguments.\nUsage: chpri [pid] [priority]\n");
    exit(1);
  }
  
  int pid = atoi(argv[1]);
  int priority = atoi(argv[2]);
  
  if (setpriority(pid, priority) < 0) {
    printf("Error: System call was unsuccessful.\n");
    exit(1);
  }
  
  exit(0);
}
