#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define PORT 9000
#define MAX_CLIENT 50

struct clientStructure
{
	int index;
	int socketID;
	struct sockaddr_in client_addr;
	int len;
	char status[20];
};

static int numberOfclients = 0;
struct clientStructure clientArray[50];

pthread_t clientThread[50];

void removeFromBuffer(int id)
{

	clientArray[id].index = -1;
	clientArray[id].socketID = -1;
}
void *chat(void *ClientInfo)
{
	struct clientStructure *clientDetail = (struct clientStructure *)ClientInfo;
	int index = clientArray[clientDetail->index].index;
	int clientSocket = clientDetail->socketID;

	printf("\033[1;33mClient %d connected.\033[0m\n", index + 1);

	while (1)
	{

		char data[1024];
		int read = recv(clientSocket, data, 1024, 0);
		data[read] = '\0';
		char output[1024];

		if (strcmp(data, "LIST") == 0)
		{

			int l = 0, self = 0;

			for (int i = 0; i < numberOfclients; i++)
			{

				if (i != index && clientArray[i].index >= 0)
				{
					self = 1;
					l += snprintf(output + l, 1024, "Client %d is at socket %d.\n", clientArray[i].index + 1, clientArray[i].socketID);
				}
			}

			if (self == 0)
			{
				strcpy(output, "\033[1;32mOnly you are in the list.\033[0m");
				send(clientSocket, output, 1024, 0);
			}
			else
			{

				send(clientSocket, output, 1024, 0);
			}
		}
		else if (strcmp(data, "SEND") == 0)
		{

			bzero(data, sizeof(data));
			recv(clientSocket, data, 1024, 0);

			// puts(data);
			if (strcmp(data, "EXIT") == 0)
			{
				continue;
			}

			else if (strlen(data) == 1)
			{

				int id = atoi(data) - 1;
				int flag = 0;
				for (int i = 0; i < numberOfclients; i++)
				{

					if (i != index && id == clientArray[i].index)
					{
						flag = 1;
						break;
					}
				}

				if (flag == 1)
				{

					bzero(data, sizeof(data));
					read = recv(clientSocket, data, 1024, 0);
					data[read] = '\0';

					sprintf(output, "[C%d]: %s", index + 1, data);
					send(clientArray[id].socketID, output, 1024, 0);
				}
				else
				{

					bzero(data, sizeof(data));
					read = recv(clientSocket, data, 1024, 0);
					data[read] = '\0';
					sprintf(output, "\033[1;31mWrong client id!!\033[0m\nCan't send messege: %s", data);
					send(clientSocket, output, 1024, 0);
				}
			}
			else
			{
				bzero(data, sizeof(data));
				read = recv(clientSocket, data, 1024, 0);
				data[read] = '\0';
				sprintf(output, "\033[1;31mWrong chat input!!\033[0m\nCan't send messege: %s", data);
				send(clientSocket, output, 1024, 0);
				bzero(data, sizeof(data));
			}
		}
		else if (strcmp(data, "ALL") == 0)
		{

			bzero(data, sizeof(data));
			read = recv(clientSocket, data, 1024, 0);
			data[read] = '\0';
			bzero(output, sizeof(output));
			sprintf(output, "\033[1;33m[Broadcast]:\033[0m%s", data);
			printf("\033[1;32m%s\033[0m\n", output);
			int length = 0;
			for (int i = 0; i < numberOfclients; i++)
			{

				if (i != index)

					send(clientArray[i].socketID, output, 1024, 0);
			}
		}
		else if (strcmp(data, "EXIT") == 0)
		{

			bzero(data, sizeof(data));
			sprintf(data, "\033[1;36mClient %d has left the chat!!\033[0m", index + 1);
			for (int i = 0; i < numberOfclients; i++)
			{

				if (i != index)

					send(clientArray[i].socketID, data, 1024, 0);
			}
			break;
		}
		else if (strcmp(data, "WHO") == 0)
		{
			sprintf(output, "\033[1;36m--Hi there--\033[0m\n\033[1;32mYou are client\033[0m \033[1;33m%d\033[0m \033[1;32mand you're connected to socket\033[0m \033[1;33m%d\033[0m.", index + 1, clientSocket);
			send(clientSocket, output, 1024, 0);
		}

		else
		{

			strcpy(output, "\033[1;33mWrong Command\033[0m...Try one of these..\n\033[1;36m> Type 'SEND' to enter into the chat mode.\n> After enterning into the chat mode Type '<client id> <message>' to chat.\n> Type 'ALL <message>' for broadcast.\n> Type 'LIST' to view no: of active clients.\n> Type 'WHO' to see your status.\n> Type 'EXIT' to get out from the program if you're not in the chat mode else type 'EXIT' 2 times.\033[0m");
			send(clientSocket, output, 1024, 0);
		}

		bzero(data, sizeof(data));
		bzero(output, sizeof(output));
	}
	removeFromBuffer(index);
	close(clientSocket);
	printf("\033[1;36mClient %d has left the chat!\033[0m\n", index + 1);
	return NULL;
}

int main()
{

	int socketFD = socket(PF_INET, SOCK_STREAM, 0);

	if (socketFD < 0)
	{
		printf("\033[1;31mSome error occured while creating the socket!!\033[0m\n");
		return -1;
	}

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(9000);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

	if (bind(socketFD, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		printf("\033[1;31mError occured while binding!!\033[0m\n");
		return 1;
	}

	if (listen(socketFD, 50) < 0)
	{
		printf("\033[1;31mError while listening to the port %d\033[0m", PORT);
		return -1;
	}

	printf("\033[1;32mServer started listenting on port %d\033[0m\n", PORT);

	while (numberOfclients < 50)
	{

		clientArray[numberOfclients].socketID = accept(socketFD, (struct sockaddr *)&clientArray[numberOfclients].client_addr, &clientArray[numberOfclients].len);
		clientArray[numberOfclients].index = numberOfclients;
		pthread_create(&clientThread[numberOfclients], NULL, chat, (void *)&clientArray[numberOfclients]);

		numberOfclients++;
	}

	for (int i = 0; i < numberOfclients; i++)
		pthread_join(clientThread[i], NULL);

	close(socketFD);
	return 0;
}