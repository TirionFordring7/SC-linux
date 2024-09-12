#include <stdio.h>
#include <stdlib.h> //exit, system
#include <sys/msg.h>
#include <string.h> //memset

#define BUF_MAXSIZE 2048

struct msgbuf {
  long mtype;
  char mtext[BUF_MAXSIZE];
};

int main() {

  int key, qid;
  FILE* pipe;
  int numc;
  struct msgbuf mesb;

  key = 15;
  
  memset(mesb.mtext, 0, BUF_MAXSIZE);


    
    qid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if(qid == -1) {   
        perror("Ошибка создания\n");
      exit(1);
    }

  pipe = popen("file * | grep .*:.*shell.* | awk -F: '{print $1}'", "r");
  if(pipe == NULL) {
    printf("Ошибка получения списка файлов\n");
    exit(1);
  }
  numc = 0;
  while(1) {
    mesb.mtext[numc] = fgetc(pipe);
    if(mesb.mtext[numc] == 0) {
      break;
    }
    else if(mesb.mtext[numc] == EOF) {
      mesb.mtext[numc] = 0;
      break;
    }
    numc++;
    if(numc == BUF_MAXSIZE) {
      printf("Ошибка размера буфера сообщений\n");
      exit(1);
    }
  }
  mesb.mtype = 1;
  if(pclose(pipe) == -1) {
	printf("Ошибка закрытия канала\n");
    exit(1);
  }
  
  if(msgsnd(qid, &mesb, numc, 0) == -1) {
	perror("Ошибка отправки\n");
    exit(1);
  }
/*printf("%s\n", mesb.mtext); */
  memset(mesb.mtext, 0, numc);
  exit(0);
}