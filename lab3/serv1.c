#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MBUFSIZE 4096

int main()
{
	int cliid,count,count1;
	char buffer[MBUFSIZE];
    struct sockaddr_un serv_addr;
	FILE* pipe;
	
	int sid = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sid == -1)
    {
        perror("socket");
        exit(1);
    }

    
    bzero((void *)&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, "/tmp/sock1");

	memset(buffer, 0, MBUFSIZE);

    if (bind(sid, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
    {
        perror("Ошибка bind");
        exit(1);
    }
	if (listen(sid, 5) != 0)
    {
        perror("Ошибка listen");
        exit(1);
    }
	cliid = accept(sid,0,0);
	if (cliid == -1){
		perror("Ошибка accept");
        exit(1);
	}
	count = recv(cliid, buffer, MBUFSIZE, 0);
	if (count == -1){
		perror("Ошибка получения");
		exit(1);
	}
	count1= 0;
	while(buffer[count1] != 0){
		count1++;
	}
	printf("%d , %d, %s",count, count1,buffer);
	pipe = popen("sort -u | grep -c -E '.*'","w");
	fwrite(buffer, sizeof(buffer[0]),count,pipe);
	
    printf("Количество уникальных пользователей: ");
	fflush(stdout);
	fclose(pipe);
    close(sid);
	system("rm /tmp/sock1");
    exit(0);
}