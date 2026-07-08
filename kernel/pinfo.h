#ifndef _PINFO_H_
#define _PINFO_H_

#define NPROC 64

struct pinfo {
    int pid;
    int state;
    int priority;
    int tickets;
};

#endif
