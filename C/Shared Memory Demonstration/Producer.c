#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>

int getPriority(int seed) {
  srand(seed);
  int priority = (int)(rand()/20000) ;
  return priority ;
}
int getprocessID(void) {
  int ID = fork() ;
  if (ID == 0) {
    /*This causes the child process to sleep forever,
     so that the rest of the program does not run twice */
    for(;;){
      sleep(1000) ;
    }
  }
  return ID ;
}

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
  sharedMemory() ;
  int queueLength;
  int numberOfJobsToCreate ;
  /* argc is total amount of Command Line arguments, including program name */
  if (argc == 2) { /* If two Command Line Arguments, meaning only numberOfJobsToCreate was specified */
    numberOfJobsToCreate = atoi(argv[1]) ;
    /* Reads queue length from a txt file */
    FILE *storedQueueLength1 = fopen("storedQueueLength.txt", "r");
    char queueStr[5] ;
    (int) fscanf(storedQueueLength1,"%s",queueStr );
    fclose(storedQueueLength1);
    queueLength = atoi(queueStr) ;
  } else if (argc == 3) { /* If three Command Line Arguments, meaning numberOfJobsToCreate and queue length was specified */
    printf("%s\n","Queue Length is being changed, any jobs previously in the queue have been destroyed." );
    /* Writes new queue length to txt file */
    FILE *storedQueueLength2 = fopen("storedQueueLength.txt", "r");
    char queueStr[5] ;
    (int) fscanf(storedQueueLength2,"%s",queueStr );
    fclose(storedQueueLength2);
    queueLength = atoi(queueStr) ;
    /*Kills all job processes and empties the queue when queue length is changed. */
    for (int i = 0; i < queueLength; i++) {
      if (data[i]>0) {
        kill(data[i],SIGKILL);
      }
      data[i] = 0 ;
      data[i+queueLength] = 0;
    }
    numberOfJobsToCreate = atoi(argv[1]) ;
    queueLength = atoi(argv[2]) ;
    /*Writes the specified queue length to storedQueueLength.txt, overwriting the previous one */
    FILE *storedQueueLength3 = fopen("storedQueueLength.txt", "w");
    fprintf(storedQueueLength3, "%d\n",queueLength);
    fclose(storedQueueLength3) ;

  } else if (argc == 1) { /* If one Command Line Arguments, meaning neither numberOfJobsToCreate or queue length was specified */
    numberOfJobsToCreate = 1; /* numberOfJobsToCreate defaults to 1 */
    FILE *storedQueueLength = fopen("storedQueueLength.txt", "r");
    char queueStr[5] ;
    (int) fscanf(storedQueueLength,"%s",queueStr );
    fclose(storedQueueLength);
    queueLength = atoi(queueStr) ; /*Queue length is read from the txt file */
  } else { /*This runs if there are too many arguments */
    printf("%s\n", "Too Many Arguments! Format is ./Producer {numberOfJobsToCreate} {queueLength}" );
    exit(1) ;
  }
  printf("%s %d %s\n\r","The queue has a maximum of", queueLength, "spaces") ;
  if (data[queueLength-1] == 0) { /*This checks if the queue is full */
    for (int i = 0; i < numberOfJobsToCreate; i++) { /*This makes the code that creats the job run for the amount of times specified in the command line arguments */
      for(int i = 0; i<queueLength;i++){ /*This iterates through the queue, the If statement below checks if each place in the queue is empty */
        if(data[i+queueLength]==0) {
          data[i] = getprocessID(); /*ProcessID */;
          data[i+queueLength] = getPriority(data[i]); /*Priority */;
          /*Gets current system time*/
          time_t t = time(NULL);
          struct tm *timeStruct = localtime(&t);
          char timeString[64];
          strftime(timeString, sizeof(timeString), "%c", timeStruct);
          FILE *log = fopen("ProducerLog.txt", "a");
          fprintf(log, "%s: %s %d %s %d\n",timeString,"Job",data[i],"created with priority",data[i+queueLength] ); /*This is the string that is written to ProducerLog.txt */
          fclose(log) ;
          printf("%s %d %s %d\n","Job",data[i],"created with priority",data[i+queueLength] );
          i = queueLength ; /*This sets i to the length of the queue, so that the for loop ends */
        }
        /*This sorts the queue in decending order of priority, this is so
        that the Consumer can take the highest priority job by selecting
        the first element in shared memory */
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
      }
  } else {
    printf("%s\n","Queue is Full" );
  }
    /* Detaches and removes the shared memory segment
    shmdt(data) ;
    shmctl(1234, IPC_RMID, NULL);
    */
  }
