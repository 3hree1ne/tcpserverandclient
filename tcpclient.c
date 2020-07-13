/*
gcc tcpclient -o tcpclient -pthread
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <arpa/inet.h>

FILE *Clientlogs;
int sockfd, clientfd;
const char *ipnum = "127.0.0.1";

//threadmain func
void *threadMain1(void *arg){

	while(1){
		char buff[512] = {0}; 
		int retval;
		fgets(buff, sizeof(buff), stdin);
		retval = send(*((int *)arg), buff, 512, 0);
		if(retval == -1){
			fprintf(stdout,"Send error: %s\n",strerror(errno));
			exit(1);
		}
		Clientlogs = fopen("/home/daddy/Documents/logs/Clientlogs" , "ab");
		fprintf(Clientlogs,"%s", buff);
		fclose(Clientlogs);
	}
}

void *threadMain2(void *arg){

        while(1){
                char buff[512] = {0};
                int retval, slen;
		slen = sizeof(buff);
                retval = recv(*((int *)arg), &buff, slen, 0);
                if(retval == -1){
                        fprintf(stdout,"Recv error: %s\n",strerror(errno));
                        exit(1);
                }else{

			if(buff[0] == '\0'){
				fprintf(stdout, "Client ayrıldı!\n");
				exit(1);
			}

			fprintf(stdout,"%s", buff);
			Clientlogs = fopen("/home/daddy/Documents/logs/Clientlogs" , "ab");
			fprintf(Clientlogs,"%s", buff);
			fclose(Clientlogs);
		}
        }
}

//main func
int main(){
	struct sockaddr_in server;
	int retval, pretval1, pretval2;
        int port = 54321;
	int yes = 1;
	socklen_t slen = sizeof(server);
	pthread_t tid1, tid2;

//open socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		fprintf(stdout,"Socket error: %s\n",strerror(errno));
		exit(1);

		}

//build socket structures
	server.sin_family      = AF_INET;
	server.sin_port        = htons(port);
	inet_aton(ipnum, (struct in_addr *)&server.sin_addr.s_addr);

//connect socket
	retval = connect(sockfd, (struct sockaddr *)&server, slen);
	if(retval == -1){
                fprintf(stdout,"Connect error: %s\n",strerror(errno));
                exit(1);
	}
	fprintf(stdout, "Client Bağlandı!\n");	

//create recv & send threads
	pretval1 = pthread_create(&tid1, NULL, &threadMain1, &sockfd);
	if(pretval1 != 0){
                fprintf(stdout,"pthread_create error: %s\n",strerror(errno));
                exit(1);
        }

	pretval2 = pthread_create(&tid2, NULL, &threadMain2, &sockfd);
        if(pretval2 != 0){
                fprintf(stdout,"pthread_create error: %s\n",strerror(errno));
                exit(1);

        }

//recv & send
		pthread_join(tid1, NULL);
		pthread_join(tid2, NULL);

		return 0;
}
