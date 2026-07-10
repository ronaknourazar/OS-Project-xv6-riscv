#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define WORK_SMALL  300000000
#define WORK_MEDIUM 600000000
#define WORK_LARGE  1200000000

void
do_heavy_work(int loops)
{
	volatile int x = 0;
	for (int i = 0; i < loops; i++)
	x++;
}

void
run_group(int priorities[], int n, int work)
{
	printf("-------------------------------------------\n");
	printf("Launching %d test samples\n", n);
	
	for (int i = 0; i < n; i++) {
		int pid = fork();
		
		if (pid < 0) {
			printf("fork failed!\n");
			exit(1);
		}
		
		if (pid == 0) {
			do_heavy_work(work);
			
			printf("[FINISH] PID %d Priority=%d\n",
			getpid(), priorities[i]);
			
			exit(0);
		}
		setpriority(pid, priorities[i]);
		
		printf("[CREATE] PID %d Priority=%d\n",
		pid, priorities[i]);
	}
	
	for (int i = 0; i < n; i++)
	wait(0);
	
	printf("Group completed.\n\n");
}

int
main(int argc, char *argv[])
{
	printf("=============================================\n");
	printf("      PRIORITY SCHEDULER TEST\n");
	printf("=============================================\n\n");
	
	printf("TEST 1 : Equal Priorities\n");
	printf("Expected: All processes should run equally.\n");
	
	int samples_1[] = {
		20,20,20,20,20,20
	};
	
	run_group(samples_1, 6, WORK_SMALL);
	
	printf("TEST 2 : Increasing Priorities\n");
	printf("Expected: Priority 5 should generally finish before rest.\n");
	
	int samples_2[] = {
		90,
		70,
		50,
		30,
		15,
		5
	};
	
	run_group(samples_2, 6, WORK_MEDIUM);
	
	printf("TEST 3 : Extreme Priorities (Repeated)\n");
	
	for (int round = 1; round <= 2; round++) {
		printf("\nRound %d\n", round);
		
		int samples_3[] = {
			99,
			80,
			60,
			40,
			20,
			1
		};
		
		run_group(samples_3, 6, WORK_MEDIUM);
	}
	
	printf("TEST 4 : Random Priorities\n");
	printf("Expected: Higher priorities (lower numerical values) should complete first.\n");
	
	int samples_4[] = {
		47,
		10,
		81,
		25,
		63,
		5,
		90,
		34
	};
	
	run_group(samples_4, 8, WORK_MEDIUM);
	
	printf("TEST 5 : High Contention\n");
	printf("Expected: Higher priorities (lower numerical values) should complete first.\n");
	
	int samples_5[] = {
		95,85,75,65,
		55,45,35,25,
		20,18,15,12,
		10,8,5,1
	};
	
	run_group(samples_5, 16, WORK_LARGE);
	
	printf("\n=============================================\n");
	printf("All tests completed.\n");
	printf("=============================================\n");
	
	exit(0);
}
