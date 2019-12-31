// This an example of scheduling 2 tasks using DEADLINE MONOTONIC | FIFO-SCHED | ROUND ROBIN 
// All tasks schould begin at instant 0
#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include <math.h>
#include <stdbool.h>

task *taskSet[2];
exeTime *setSched = NULL;


/********** TASK CREATION **********/
task * createTask(int no, int c, int d, int t, int at){
task * tau = (struct task *) (malloc(sizeof(struct task)));
tau->id = no;
tau->aT = at;
tau->C = c;
tau->D = d;
tau->T = t;
tau->P = 1;
tau->rT = 0;
//tau->jitter = 0;
tau->remain = 0;
return tau;
}


/********** LCM of 2 numbers *************/

int LCMofTowNumbers (int x , int y){

    int lcm = (x > y) ? x : y;

    // Toujours vrai
    while(1)
    {
        if( lcm%x==0 && lcm%y==0 )
        {
            //printf("PPCM de %d et %d = %d", x, y, lcm);
            break;
        }
        ++lcm;
    }
    return (lcm);
}


/********** LCM of N numbers *************/

int LCMofNNumbres (struct task **tas, int tableSize) {
  int lcm = 1;
  int i;
  for (i = 0 ; i < tableSize; i++)
    lcm = LCMofTowNumbers(lcm,tas[i]->T);
  return lcm;
}


/********** UTILIZATION FACTOR **********/
float utilisation(int n){
float sum = 0, t;
int i;
for (i=0; i<n; i++){
       sum += (float) taskSet[i]->C / taskSet[i]->T;
}
return sum;
}


/********** DEMANDE BOUND FUNCTION **********/
float dbfFunction(int t, int n) {
  int i;
  float dbf = 0;
  for (i = 0 ; i<n; i++)
    dbf += taskSet[i]->C * ceil((t+taskSet[i]->T-taskSet[i]->D)/taskSet[i]->T);
  return dbf;
}


/********** SCHEDULABILITY ANALYSIS BASED ON RBF FUNCTION **********/

int rbf(int n, int t){
return ( taskSet[n]->C * ceil(t/taskSet[n]->T));
}

int workload(int n, int t){
int i, sum = 0;
for (i=0; i<=n; i++)
    sum += rbf(i,t);
return sum;
}

bool schedRBF (int n){
int i,j,t,w, k;
bool sched[2];
for (i=0; i<n; i++){
    sched[i] = false;
    j=0;
    k=1;
    while (j<=i){
        t = k * taskSet[j]->T;
        w = workload(i,t);
        if ( w <= t)
            sched[i] = true;
         j++;
         k = floor(taskSet[i]->D / taskSet[j]->D);
    }
}
for(i=0; i<n; i++){
    if (sched[i]==false)
        return false;
}
return true;
}


/********** WCRT **********/
void wcrt(){
taskSet[0]->rT = taskSet[0]->C;
taskSet[1]->rT = taskSet[1]->C + (taskSet[0]->C * ceil((float)taskSet[0]->rT/taskSet[0]->T));
}


/********** SCHEDULABILITY ANALYSIS BASED ON WCRT **********/
bool isShedulable(int n){
int i;
for (i=0; i<n; i++){
    if (taskSet[i]->rT > taskSet[i]->D)
        return(false);
}
return(true);
}


/********** FEASIBILITY TEST **********/
bool isFeasible (){
if (isShedulable(2) || schedRBF(2))
    return true;
else
    return false;
}


void createInsert (int n, int a, int r){
exeTime *q, *p = setSched;
q = (exeTime*)malloc(sizeof(exeTime));
q->idE = n;
q->compT = r;
q->activTime = a;
q->next = NULL;
if(setSched == NULL)
    setSched = q;

else{
    while(p->next != NULL)
        p = p->next;
    p->next = q;
}
}

/********** DEADLINE MONOTONIC SCHEDULING  **********/

void deadlineMonotonic(int h){
int k = 0, i = 0, r;
while (k<h){
    if (taskSet[i]->remain > 0){
        createInsert(taskSet[i]->id,k,taskSet[i]->remain);
        k += taskSet[i]->remain;

    }
    else{
        if (i==0){
            createInsert(1,k,taskSet[i]->C);
            taskSet[i]->aT += taskSet[i]->T;
            k += taskSet[i]->C;
        }
        else{
            if (k+taskSet[i]->C <= taskSet[0]->aT){
                    createInsert(2,k,taskSet[i]->C);
                    taskSet[i]->aT += taskSet[i]->T;
                    k += taskSet[i]->C;
                    }
                    else{
                        r = taskSet[0]->aT - k;
                        taskSet[i]->remain = taskSet[i]->C - r;
                        createInsert(2,k,r);
                        k = taskSet[0]->aT;
                    }
        }
    }
    i = (i+1)%2;
    if (k < taskSet[i]->aT)
        k = taskSet[i]->aT;
}
}


/********** FIFO SCHEDULING  **********/

void fifo(int h){
int k = 0, i = 0, q;
while (k<h){
    taskSet[i]->aT += taskSet[i]->T;
    createInsert(taskSet[i]->id, k, taskSet[i]->C);
    k += taskSet[i]->C;
    q = (i+1)%2;
    if (taskSet[q]->aT <= taskSet[i]->aT)
        i = q;
    /*else
        k = taskSet[i]->aT;*/
}
}


/********** ROUND ROBIN SCHEDULING  **********/

void roundRobin(int q, int h){
int i = 0, k = 0;
while (k < h){
        if (taskSet[i]->C <= q){
            createInsert(taskSet[i]->id, k, taskSet[i]->C);
            k += taskSet[i]->C;
        }
        else{
            if (taskSet[i]->remain == 0){
                createInsert(taskSet[i]->id, k, q);
                taskSet[i]->remain = taskSet[i]->C - q;
                k += q;
            }
            else{
                if (taskSet[i]->remain <= q){
                    createInsert(taskSet[i]->id, k, taskSet[i]->remain);
                    taskSet[i]->remain = 0;
                    k += taskSet[i]->remain;
                }
                else{
                    createInsert(taskSet[i]->id, k, q);
                    taskSet[i]->remain = taskSet[i]->remain - q;
                    k += q;
                }
            }
        }
        i = (i+1)%2;
}

}

void display(){
    exeTime *p, *q;
if (setSched == NULL)
    printf("\n la liste est vide");
else{
    printf("\n Task | ID | ActiveTime | ComputationTime |");
    p = setSched;
    while( p->next != NULL ){
        printf ("\n T[%d] | %d |     %d     |        %d       |", p->idE, p->idE ,p->activTime, p->compT);
        p = p->next;
    }
}
}


void deleteList(){
exeTime* current = setSched;
exeTime* next;

while (current != NULL)
{
    next = current->next;
    free(current);
    current = next;
}
setSched = NULL;
}


int main(){

int i,process = 2,quantum = 3, hyperPeriod;
int arrivalTime, compTime, deadline, period;
float u, demand;

/*printf("Enter the number of process : ");
scanf("%d", &process);*/

printf("\nEnter the details of tasks in their increasing deadline\n");

for (i=0; i<process; i++){
    printf("\n Enter the details of process N° : %d", i+1);
    printf("\n Arrival time : ");
    scanf("%d", &arrivalTime);
    printf("\n running time : ");
    scanf("%d", &compTime);
    printf("\n Deadline : ");
    scanf("%d",&deadline);
    printf("\n Period : ");
    scanf("%d",&period);
    taskSet[i] = createTask(i+1,compTime,deadline,period,arrivalTime);
}

printf("\n------------------  TASKS DETAILS --------------------\n");
printf("Task | ID | arrival time | Running time | Deadline |");
for(i=0; i<process; i++)
    printf("\n T[%d] : %d | %d | %d | %d | %d ",i+1, taskSet[i]->id, taskSet[i]->aT, taskSet[i]->C, taskSet[i]->D, taskSet[i]->T);


hyperPeriod = LCMofNNumbres(taskSet,process);
printf("\n\n hyperperiod = %d", hyperPeriod);

u = utilisation(process);
printf("\n\n Utilisation factor = %.2f",u);

demand = dbfFunction(hyperPeriod,process);
printf("\n\n dbf = %.2f",demand);


printf("\n\n------------------  DEADLINE MONOTONIC SCHEDULING ALGORITHM------------------->\n");
deadlineMonotonic(hyperPeriod);
display();

deleteList();


printf("\n\n------------------  FIFO SCHEDULING ALGORITHM------------------->\n");
fifo(hyperPeriod);
display();


deleteList();


printf("\n\n------------------  ROUND ROBIN ALGORITHM------------------->\n");
roundRobin(quantum,hyperPeriod);
display();

printf("\n\n -----------------------------------------------------------------------\n");

wcrt();
printf("\n The task's WCRT :");
for(i=0; i<2; i++){
    printf("\n T[%d] | %d |",i+1,taskSet[i]->rT);
}

printf("\n\n------------------  FEASIBILITY TEST ------------------->\n");

if (u>1)
    printf("\n The task set is not feasible");
else{
    if (isFeasible())
        printf("\n The task set is feasible");
    else
        printf("\n The task set is not feasible");
}


printf("\n\n------------------  SCHEDULABILITY ANALYSIS BASED ON WCRT ------------------->\n");
if (isShedulable(2))
    printf("\n The task set is schedulable by DEADLINE MONOTONIC algorithm");
else
    printf("\n The task set is not schedulable by DEADLINE MONOTONIC algorithm");



printf("\n\n------------------  SCHEDULABILITY ANALYSIS BASED ON RBF------------------->\n");
if (schedRBF(2))
    printf("\n The task set is schedulable");
else
    printf("\n The task set is not schedulable");

return 0;
}

