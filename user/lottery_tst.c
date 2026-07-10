#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define WORK_SMALL  80000000
#define WORK_MEDIUM 200000000
#define WORK_LARGE 500000000

void do_heavy_work(int loops)
{
    volatile int x = 0;
    for (int i = 0; i < loops; i++)
        x++;
}

void run_group(int tickets[], int n, int work)
{
    printf("--------------------------------------------\n");
    printf("Launching %d processes\n", n);

    for (int i = 0; i < n; i++)
    {
        int pid = fork();

        if (pid == 0)
        {
            printf("[START] PID %d | Tickets = %d\n",
                   getpid(), tickets[i]);

            do_heavy_work(work);

            printf("[DONE ] PID %d | Tickets = %d\n",
                   getpid(), tickets[i]);

            exit(0);
        }
        settickets(tickets[i]);
    }

    for (int i = 0; i < n; i++)
        wait(0);

    printf("Group completed.\n\n");
}

int
main(int argc, char *argv[])
{
    printf("\n=============================================\n");
    printf("        LOTTERY SCHEDULER TEST SUITE\n");
    printf("=============================================\n\n");

    printf("TEST 1 : Equal Tickets Fairness\n");

    int samples_1[] = {
        50,50,50,50,50,50
    };

    run_group(samples_1, 6, WORK_SMALL);

    printf("TEST 2 : Increasing Tickets\n");

    int samples_2[] = {
        10,20,30,40,50,60,70,80
    };

    run_group(samples_2, 8, WORK_MEDIUM);

    printf("TEST 3 : Extreme Difference\n");

    int samples_3[] = {
        1,
        5,
        10,
        50,
        100,
        250,
        500
    };

    run_group(samples_3, 7, WORK_MEDIUM);

    printf("TEST 4 : Random Distribution\n");

    int samples_4[] = {
        77,
        15,
        200,
        31,
        94,
        8,
        150,
        61,
        300,
        12
    };

    run_group(samples_4, 10, WORK_MEDIUM);

    printf("TEST 5 : High Contention (16 Processes)\n");

    int samples_5[] = {
        5,10,15,20,
        25,30,35,40,
        45,50,55,60,
        65,70,75,80
    };

    run_group(samples_5, 16, WORK_LARGE);

    printf("TEST 6 : Repeated Probability Test\n");

    for (int round = 1; round <= 2; round++)
    {
        printf("\nRound %d\n", round);

        int t[] = {
            10,
            20,
            40,
            80
        };

        run_group(t, 4, WORK_SMALL);
    }

    printf("\n=============================================\n");
    printf("        ALL TESTS COMPLETED\n");
    printf("=============================================\n");

    exit(0);
}
