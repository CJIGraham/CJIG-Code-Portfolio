#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>

int *data;
int * sharedMemory() {
  /* This makes a 1KB shared memory segment */
  #define SHM_SIZE 1024
  key_t key = 1234;
  int shmid;
  /* This creates the shared memory: */
  shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT) ;
  /* This gets a pointer to shared memory and assigns it to the variable 'data': */
  data = shmat(shmid, (void *)0, 0);
  /* return pointer to variable that can be used in other parts of the program */
  return data;
}

  void main( int argc, char *argv[] ) {
    sharedMemory();
    int numberOfJobsToDestroy ;
    if (argc == 1){ /*If there is only 1 argument, it means that numberOfJobsToDestroy was not specified, so is defaulted to 1 */
      numberOfJobsToDestroy = 1 ;
    } else if (argc == 2) { /*If there are 2 arguments, then numberOfJobsToDestroy was specified and so takes the value specified */
      numberOfJobsToDestroy = atoi(argv[1]) ;
    }
    int queueLength ;
    /*Reads queue length from the txt file, for use in the sort later. */
    FILE *storedQueueLength = fopen("storedQueueLength.txt", "r");
    char queueStr[5] ;
    (int) fscanf(storedQueueLength,"%s",queueStr );
    fclose(storedQueueLength);
    queueLength = atoi(queueStr) ;
    /*Checks if the first element is empty, if it is, then the queue is empty and the Consumer will exit*/
    if (data[0]== 0) {
      printf("%s\n","Queue is Empty" );
      exit(0) ;
    } else {
      for (int i = 0; i < numberOfJobsToDestroy; i++) { /*Iterates for the amount of times specified by numberOfJobsToDestroy */
        /*Gets current system time*/
        time_t t = time(NULL);
        struct tm *timeStruct = localtime(&t);
        char timeString[64];
        strftime(timeString, sizeof(timeString), "%c", timeStruct);
        FILE *log = fopen("ConsumerLog.txt", "a");
        fprintf(log, "%s: %s %d %s %d\n",timeString,"Job",data[i],"destroyed with priority",data[i+queueLength] ); /*This is the string that is written to ConsumerLog.txt */
        fclose(log) ;
        printf("%s %d %s %d\n","Job",data[i],"destroyed with priority",data[i+queueLength] );
        /*Kills the process that is stored in the queue and sets the ProcessID and Priority in the queue to 0 */
        if (data[i]>0) {
          kill(data[i],SIGKILL);
        }
        data[i] = 0 ;
        data[i+queueLength] = 0;
      }
      /*This sorts the queue in decending order of priority, this is so
      that the Producer does not insert a new job at the first element
      in shared memory when there are other jobs after */
      for(int k = 0; k<queueLength; k++){
        for(int i = 0; i<queueLength; i++){
          int processTemp = 0 ;
          int priorityTemp = 0 ;
          if(data[i+queueLength]<data[i+(queueLength+1)]){
            processTemp = data[i] ;
            priorityTemp = data[i+queueLength];
            data[i] = data[i+1];
            data[i+queueLength] = data[i+(queueLength+1)] ;
            data[i+1] = processTemp ;
            data[i+(queueLength+1)] = priorityTemp ;
          }
        }
      }
    }
    /* Detaches and removes the shared memory segment */
    shmdt(data);
    shmctl(1234, IPC_RMID, NULL);
  }
