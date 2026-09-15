#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

 size_t N;
 pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
typedef struct __holder_t{
	int port;
	char* IP;
} holder_t;

void* scan_port(void* arg){
	holder_t m = *(holder_t*)arg;
	int port = m.port;

	struct sockaddr_in addr;
     memset(&addr, 0, sizeof(addr));
     addr.sin_family = AF_INET;
     addr.sin_addr.s_addr = inet_addr(m.IP);

	int sock = socket(AF_INET,SOCK_STREAM,0);
	
	if (sock == -1){
		perror("socket");
			return NULL;}
		
	addr.sin_port = htons(port);
	int result = connect(sock, (struct sockaddr*) &addr, sizeof(addr));
		if (result == 0){
			pthread_mutex_lock(&print_lock);
			printf("Port %d: open\n", port);
			pthread_mutex_unlock(&print_lock);
		}
	
		close(sock);
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

	int start = atoi(argv[2]);
	int end = atoi(argv[3]);

	N = end - start + 1;
	holder_t test[N];
	pthread_t threads[N];
	
	for (size_t counter = 0; counter < N ; ++counter){
		test[counter].port = start + counter;
		test[counter].IP = argv[1];
		pthread_create(&threads[counter], NULL, scan_port, &test[counter]);
	}

	for(size_t i = 0; i < N; ++i){pthread_join(threads[i],NULL);}


	
	return 0;
}
