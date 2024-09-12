
#include <stdio.h>
#include <stdlib.h> //exit, system
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h> //access, sleep, getuid
#include <string.h> //memset
#include <time.h> // strftime

#define SHMSIZE 2048


union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
  struct seminfo *__buf;
};

int main() {

  int key, semid, shmid,count,count1;
  char semdate[256];
  struct sembuf my_sembuf = {0, -1, 0};
  struct sembuf my_sembuf1 = {0, 2, 0};
  struct semid_ds semstat;
  char* shmdata;
  struct shmid_ds shmstat;
  char req[SHMSIZE+247];
  union semun semarg;
  FILE* pipe;

  semarg.val = 0;
  count1 = 0;
  count = 0;
  memset(semdate, 0, 256);
  memset(req, 0, SHMSIZE+123);

    key = 4;

    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if(semid == -1) {
      perror("Ошибка создания семафора\n");
      exit(1);
    }
    shmid = shmget(key, SHMSIZE, IPC_CREAT | IPC_EXCL | 0666);
    if(shmid == -1) {
        perror("Ошибка создания разделяемой области памяти\n");
      exit(1);
    }


  semctl(semid, 0, SETVAL, semarg);                                   

  semop(semid, &my_sembuf, 1);

  shmdata = (char*) shmat(shmid,0,0);
  if(shmdata == (char*)-1) {
      perror("Ошибка подключения РОП\n");
	  exit(1);
    }
    
  

  //printf("%s\n", shmdata);
  snprintf(req, SHMSIZE+247, "echo \"%s\" | awk '{print $3, $4}' | awk -F \"[ :-]\" '{print systime()-mktime($1\" \"$2\" \"$3\" \"$4\" \"$5\" \"0)}' | awk 'BEGIN{ RS = \"\" ; F= \"\\n\" }{for(i=1;i<=NF-1;i++) {fin=\"date -u -d \\\"@\"$i\"\\\" +\\\"%%H:%%M\\\"\"; system(fin)}}'", shmdata);
  //system(req);
  
  pipe = popen(req, "r");
  
  
  
  semarg.buf = &semstat;
  if(semctl(semid, 0,IPC_STAT, semarg) == -1) {
    perror("Ошибка получения информации о семафоре");
    exit(1);
  }
  strftime(semdate, 256, "%F %T", localtime(&semstat.sem_ctime));
	
  
  memset(shmdata, 0, SHMSIZE);
  while(1) {
    shmdata[count] = fgetc(pipe);
    if(shmdata[count] == 0) {
      break;
    }
    else if(shmdata[count] == EOF) {
      shmdata[count] = 0;
      break;
    }
    count++;
    if(count == SHMSIZE) {
      perror("Ошибка размера сообщения\n");
      exit(1);
    }
  }
  
  while(1) {
    shmdata[count] = semdate[count1];
    if(shmdata[count] == 0) {
      break;
    }
    else if(shmdata[count] == EOF) {
      shmdata[count] = 0;
      break;
    }
    count++;
	count1++;
    if(count == SHMSIZE) {
      perror("Ошибка размера сообщения\n");
      exit(1);
    }
  }
  

  semop(semid, &my_sembuf1, 1);

  if(shmdt(shmdata)==-1) {
    perror("Ошибка отключения РОП\n");
    exit(1);
  }
  
   if(shmctl(shmid, IPC_STAT, &shmstat) == -1) {
    perror("Ошибка получения информации о семафоре");
    exit(1);
  }
  
  if(shmstat.shm_perm.uid != getuid()) {
    shmstat.shm_perm.uid = getuid();
    if(shmctl(shmid, IPC_SET, &shmstat) == -1) {
      perror("Ошибка удаления РОП");
      exit(1);
    }
  }
  if(shmctl(shmid, IPC_RMID, 0) == -1) {
    perror("Ошибка удаления РОП");
    exit(1);
  }



  if(semstat.sem_perm.uid != getuid()) {
    semstat.sem_perm.uid = getuid();
    if(semctl(semid, 0, IPC_SET, semarg) == -1) {
      perror("Ошибка удаления семафора");
      exit(1);
    }
  }
  if(semctl(semid, 0, IPC_RMID, 0) == -1) {
    perror("Ошибка удаления семафора");
    exit(1);
  }

  exit(0);
}
