#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
//#include <ctype.h>

#define SERVER_PORT 4242
#define B_LEN 4096

int main(){
	struct sockaddr_in localAddr, remoteAddr;	
	socklen_t sizeRemoteAddr;

	int local_sckt, remote_sckt;
	char buffer[B_LEN];

	local_sckt = socket(AF_INET, SOCK_STREAM, 0);
	if (local_sckt < 0)
		printf("Falha. Nao possivel criar socket\n");
	else
		printf("Successo. Socket criado\n");

	remoteAddr.sin_family      = AF_INET;
	remoteAddr.sin_port        = htons(SERVER_PORT);
	remoteAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(remoteAddr.sin_zero, 0x0, 8);

	//remote_sckt = connect(local_sckt, (struct sockaddr*)&remoteAddr, sizeRemoteAddr);
	remote_sckt = connect(local_sckt, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
	if (remote_sckt < 0)
		printf("Falha ao conectar com servidor\n");
	else{
		printf("Successo. Conexao estabelecida com o servidor\n");

	    while(1){
		    memset(buffer, 0x0, B_LEN);
	        recv (local_sckt, buffer, B_LEN, 0);
            printf("Msg do servidor: %s\n", buffer );

            memset(buffer, 0x0, B_LEN);
            scanf("%s", buffer);
            send(local_sckt, buffer, strlen(buffer), 0);
	    }
	}
	//close(local_sckt);
	printf("Conexao encerrada com servidor\n");
}