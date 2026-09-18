#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define THREAD_WORKERS 8
 sem_t queue_lock;
 sem_t print_lock;
 int *queue;
 int queue_size;
int next_index = 0;
 int start;
 int end;
char* IP;




void scan_port(int arg){
	int port = arg;
	
	
	struct sockaddr_in addr;
     memset(&addr, 0, sizeof(addr));
     addr.sin_family = AF_INET;
     addr.sin_addr.s_addr = inet_addr(IP);

	int sock = socket(AF_INET,SOCK_STREAM,0);
	
	if (sock == -1){
		perror("socket");
			return;}
		
	addr.sin_port = htons(port);
	int result = connect(sock, (struct sockaddr*) &addr, sizeof(addr));
		if (result == 0){
			sem_wait(&print_lock);
		printf("Port %d: open\n", port);
			sem_post(&print_lock);
		}
		close(sock);
	return;
}


void* portWorker(void* arg){
	
	while (1){
		sem_wait(&queue_lock);
		if(next_index >= queue_size){
			sem_post(&queue_lock);
			return NULL;
		}
		int port = queue[next_index];
		next_index+=1;
		sem_post(&queue_lock);
		scan_port(port);

	}

	return NULL;
		
}


int main(int argc, char* argv[]){
	
	if (argc < 4){
		fprintf(stderr, "Usage: %s <target-ip> <start-port> <end-port>\n",argv[0]);
		return 1;
	}

	if (inet_addr(argv[1]) == INADDR_NONE){
		fprintf(stderr, "Invalid IP Address: %s\n", argv[1]);
		return 1;
	}

	
	 IP = argv[1];
	 start = atoi(argv[2]);
	 end = atoi(argv[3]);
	
	 queue_size = end - start + 1;
	pthread_t threads[THREAD_WORKERS];
	queue = malloc(queue_size * sizeof(int));
		if (queue == NULL){
		perror("malloc");
		return 1;
	}
	
	sem_init(&queue_lock,0,1);
	sem_init(&print_lock,0,1);

	for (size_t counter = 0; counter < queue_size ; ++counter){ //thread pool
		queue[counter] = start + counter;
	}

	for (size_t counter = 0; counter < THREAD_WORKERS ; ++counter){ //thread pool
		pthread_create(&threads[counter], NULL, portWorker, NULL);
	}

	for(size_t i = 0; i < THREAD_WORKERS; ++i){pthread_join(threads[i],NULL);}

	printf("Leave Main\n");
	free(queue);
	return 0;
}
