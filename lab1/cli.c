#include <stdio.h>
#include <stdlib.h> //exit, system
#include <sys/msg.h>
#include <unistd.h>
#include <string.h> //memset


#define REGEXPSIZE 123
#define TIME_LIMIT 60
#define BUF_MAXSIZE 2048

struct msgbuf {
  long mtype;
  char mtext[BUF_MAXSIZE];
};

int main() {
  //переменные
  int key,qid,time,shift;
  char new_char = ' ';
  pid_t pid;
  int numc;
  struct msgbuf mesb;
  struct msqid_ds qstatus;
  char req[BUF_MAXSIZE+REGEXPSIZE];

  key = 15;

  memset(mesb.mtext, 0, BUF_MAXSIZE);
  memset(req, 0, BUF_MAXSIZE+REGEXPSIZE);

    time=0;
    while(1) {
      qid = msgget(key,0);
      if(qid == -1) {
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
 
  numc = msgrcv(qid, &mesb, BUF_MAXSIZE, 1, 0);
  if(numc == -1){
	perror("Ошибка получения сообщения\n");
    exit(1);
  }
  /* printf("%s\n", mesb.mtext); */
	
	while(strstr(mesb.mtext, "\n") != NULL){
		char* firstslh = strstr(mesb.mtext, "\n");
		*firstslh = new_char;
	}
   shift=REGEXPSIZE+1;
  strcpy(req, "grep -l '\\([[:space:]]\\+trap\\|^trap\\)[[:space:]]\\+\\([^[:space:]]\\+[[:space:]]*\\)\\+[[:space:]]\\+\\([0-9A-Z]\\+\\)' ");
  strcat(req,mesb.mtext);
  printf("Файлы с обработкой прерываний: \n");
  system(req);
  if(msgctl(qid,IPC_STAT,&qstatus)<0){
	perror("Ошибка доступа к структуре");
	exit(1);
    }
  pid = getpid();
  if(qstatus.msg_perm.uid != pid) {
	  qstatus.msg_perm.uid = pid;
  }
  printf("последнее сообщение было отправлено в очередь процессом с id:%d\n",qstatus.msg_lspid);
  msgctl(qid,IPC_RMID,0);
  exit(0);
}
