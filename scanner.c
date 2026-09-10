#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char* argv[]){
	if (argc < 4){
		fprintf(stderr, "Usage: %s <target-ip> <start-port> <end-port>\n",argv[0]);
		return 1;
	}
	if (inet_addr(argv[1]) == INADDR_NONE){
		fprintf(stderr, "Invalid IP Address: %s\n", argv[1]);
		return 1;
	}
	 struct sockaddr_in addr;
     memset(&addr, 0, sizeof(addr));
     addr.sin_family = AF_INET;
     addr.sin_addr.s_addr = inet_addr(argv[1]);

	int start = atoi(argv[2]);
	int end = atoi(argv[3]);

	if (start > end || end > 65535 || start < 1){
		fprintf(stderr, "Invalid Port Range: %d %d\n",start,end);
		return 1;
	}

	for (int port = start; port  <= end; ++port){
		int sock = socket(AF_INET,SOCK_STREAM,0);
			if (sock == -1){
				perror("socket");
			continue;
			} 
		addr.sin_port = htons(port);
	
		int result = connect(sock, (struct sockaddr*) &addr, sizeof(addr));
		if (result == 0){printf("Port %d: open\n", port);}
		close(sock);
	}
	return 0;
}
