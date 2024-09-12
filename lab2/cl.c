
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h> //access, sleep
#include <string.h> //memset

#define SHMSIZE 2048
#define TIME_LIMIT 60

int main() {
  int key, semid, shmid, time,count;
  struct sembuf my_sembuf = {0, 1, 0};
  struct sembuf my_sembuf1 = {0, -2, 0};
  char* shmdata;
  FILE* pipe;

    key = 4;

    
    time=0;
    while(1) {
      semid = semget(key, 0, 0);
      if(semid == -1) {
        time+=1;
        if(time>=TIME_LIMIT) {
          break;
        }
        sleep(1);
      }
      else {
        time = 0;
        break;
      }
    }
    if(time >= TIME_LIMIT ) {
      perror("Ошибка времени доступа\n");
      exit(1);
    }

    time=0;
    while(1) {
      shmid = shmget(key, 0, 0);
      if(shmid == -1) {
        time+=1;
        if(time>=TIME_LIMIT) {
          break;
        }
        sleep(1);
      }
      else {
        time = 0;
        break;
      }
    }
    if(time >= TIME_LIMIT ) {
      perror("Ошибка времени доступа\n");
      exit(1);
    }
     


  shmdata = (char*) shmat(shmid, 0, 0);
  if(shmdata == (char*)-1) {
    perror("Ошибка");
    exit(1);
  }

  memset(shmdata, 0, SHMSIZE);

  pipe = popen("who", "r");
  if(pipe == 0) {
    perror("Ошибка\n");
    exit(1);
  }

  count = 0;
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

  if(pclose(pipe) == -1) {
    perror("Ошибка\n");
    exit(1);
  }


  semop(semid, &my_sembuf, 1);
  semop(semid, &my_sembuf1, 1);

  printf("%s", shmdata);
  printf(" - Дата последнего изменения семаформа\n");

  if(shmdt(shmdata)==-1) {
    perror("Ошибка\n");
    exit(1);
  }
  exit(0);
}
