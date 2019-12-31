#ifndef TASK_H
#define TASK_H
#include <stdbool.h>

typedef struct task task;
struct task{
    int id; //task's ID
    int aT; //arrival time
    int C; //running time
    int D; //deadline
    int T; //task's period
    int P; //task's priority
    int rT; //response time
    //int jitter; //jitter on activation
    int remain;//remaining running time
};

typedef struct exeTime exeTime;
struct exeTime{
    int idE;
    int activTime;
    int compT;
    exeTime *next;
};

task * createTask(int no, int c, int d, int t, int at);
int LCMofTowNumbers (int x , int y);
int LCMofNNumbres (struct task **tas, int tableSize);
float utilisation(int n);
int rbf(int n, int t);
int workload(int n, int t);
bool schedRBF (int n);
void wcrt();
bool isShedulable(int n);
bool isFeasible ();
void createInsert (int n, int a, int r);
void deadlineMonotonic(int h);
void fifo(int h);
void roundRobin(int q, int h);
void display();
void deleteList();

#endif

