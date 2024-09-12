#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define TIME_LIMIT 60
#define MBUFSIZE 4096

int main()
{
    int count,t;
	char buffer[MBUFSIZE];
	FILE *pipe;
	struct sockaddr_un server_addr;
	memset(buffer, 0, MBUFSIZE);
	
	int sid = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sid == -1)
    {
        perror("socket");
        exit(1);
    }

    bzero((void *) &server_addr, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "/tmp/sock1");
	
	t = 0;
    while(1) {
    if(connect(sid, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
      t+=1;
      if(t > TIME_LIMIT) {
        perror("Ошибка времени подключения");
        exit(1);
      }
      sleep(1);
    }
    else {
      break;
    }
  }
	
	pipe = popen("who | awk '{print $1}'", "r");

	count = 0;
    while(1) {
		buffer[count] = fgetc(pipe);
		if(buffer[count] == 0) {
			break;
		}
		else if(buffer[count] == EOF) {
			buffer[count] = 0;
			break;
		}
		count++;
		if(count == MBUFSIZE) {
			perror("Ошибка размера сообщения\n");
			exit(1);
		}
  }
	memset(buffer, 0, MBUFSIZE);
    if (send(sid, buffer, count,0) == -1){
		perror("Ошибка отправки");
		exit(1);
	}
    printf("Информация послана на сервер. %d\n",count);
    close(sid);


    exit(0);
}