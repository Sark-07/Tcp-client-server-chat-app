#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

void *doRecieving(void *sockID)
{

	int clientSocket = *((int *)sockID);

	while (1)
	{

		char data[1024];
		int read = recv(clientSocket, data, 1024, 0);
		if (read == 0)
		{
			printf("\033[1;33mServer closed!!\033[0m\n\n");
			exit(1);
		}
		data[read] = '\0';
		printf("\033[1;32m%s\033[0m\n", data);
	}
}

void sigintHandler(int sig_num)
{

	signal(SIGINT, sigintHandler);
	printf("\nCannot be terminated using \033[1;31mCtrl+C\033[0m Type \033[1;32m'EXIT'\033[0m to terminate.\n");
	fflush(stdout);
}
int main()
{

	int identifier = 1;
	int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(9000);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		return 0;

	printf("\033[1;32mConnection established!!\033[0m\n");

	pthread_t thread;
	pthread_create(&thread, NULL, doRecieving, (void *)&clientSocket);

	char input[1024];
	signal(SIGINT, sigintHandler);
	while (1)
	{
		scanf("%s", input);

		if (strcmp(input, "EXIT") == 0)
		{

			send(clientSocket, input, 1024, 0);
			break;
		}

		else if (strcmp(input, "ALL") == 0)
		{

			send(clientSocket, input, 1024, 0);
			char msg[1024];
			bzero(msg, sizeof(msg));
			gets(msg);

			send(clientSocket, msg, 1024, 0);
		}
		else if (strcmp(input, "SEND") == 0)
		{

			while (1)
			{

				send(clientSocket, "SEND", 1024, 0);
				bzero(input, sizeof(input));
				scanf("%s", input);
				if (strcmp(input, "EXIT") == 0)
				{

					strcpy(input, "EXIT");
					send(clientSocket, input, 1024, 0);
					bzero(input, sizeof(input));
					break;
				}
				else
				{
					send(clientSocket, input, 1024, 0);

					scanf("%[^\n]s", input);
					send(clientSocket, input, 1024, 0);
				}
			}

			printf("Exiting the chat mode!\n");
		}
		else
		{
			send(clientSocket, input, 1024, 0);
		}

		bzero(input, sizeof(input));
	}

	close(clientSocket);
}