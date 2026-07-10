#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// تابعی برای ایجاد بار محاسباتی سنگین جهت درگیر کردن پردازنده
void do_heavy_work(int loops) {
  volatile int count = 0;
  for (int i = 0; i < loops; i++) {
    count++;
  }
}

int main(int argc, char *argv[]) {
  printf("==================================================\n");
  printf("--- STARTING LOTTERY SCHEDULING TEST ---\n");
  printf("==================================================\n\n");

  printf("[Scenario 1] 2 processes with EQUAL tickets (30 each)\n");
  
  for (int i = 0; i < 2; i++) {
    int pid = fork();
    if (pid == 0) {
      int my_pid = getpid();
      settickets(30);
      printf("-> Child %d (Group Equal) started with 30 tickets...\n", my_pid);
      do_heavy_work(80000000);
      printf(">>> [Equal Group] Child %d (30 Tickets) FINISHED!\n", my_pid);
      exit(0);
    }
  }
  
  wait(0);
  wait(0);
  printf("--------------------------------------------------\n");

  printf("\n[Scenario 2] processes with Ticket Differences\n");

  if (fork() == 0) {
    int my_pid = getpid();
    settickets(5);
    printf("-> Child %d (Extreme Low) started with 5 tickets...\n", my_pid);
    do_heavy_work(60000000);
    printf(">>> [Extreme Low] Child %d (5 Tickets) FINISHED!\n", my_pid);
    exit(0);
  }

  if (fork() == 0) {
    int my_pid = getpid();
    settickets(200);
    printf("-> Child %d (Extreme High) started with 200 tickets...\n", my_pid);
    do_heavy_work(60000000);
    printf(">>> [Extreme High] Child %d (200 Tickets) FINISHED!\n", my_pid);
    exit(0);
  }

  wait(0);
  wait(0);
  printf("--------------------------------------------------\n");

  printf("\n[Scenario 3] Mixed Distribution (150 vs 75 vs 15 tickets)\n");
  printf("Expected Finish Order: First 150, then 75, and lastly 15.\n");

  
  if (fork() == 0) {
    settickets(75);
    printf("-> Child %d started with 75 tickets\n", getpid());
    do_heavy_work(2500000000);
    printf(">>> [Mixed 75] Child FINISHED!\n");
    exit(0);
  }

  if (fork() == 0) {
    settickets(150);
    printf("-> Child %d started with 150 tickets\n", getpid());
    do_heavy_work(2500000000);
    printf(">>> [Mixed 150] Child FINISHED!\n");
    exit(0);
  }

  if (fork() == 0) {
    settickets(15);
    printf("-> Child %d started with 15 tickets\n", getpid());
    do_heavy_work(2500000000);
    printf(">>> [Mixed 15] Child FINISHED!\n");
    exit(0);
  }

  wait(0);
  wait(0);
  wait(0);

  printf("\n==================================================\n");
  printf("--- LOTTERY SCENARIOS COMPLETED SUCCESSFULLY ---\n");
  printf("==================================================\n");
  
  exit(0);
}
