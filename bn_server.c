#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER_PORT 4242
#define B_LEN 4096

int main(){
	struct sockaddr_in localAddr, remoteAddr;	
	socklen_t sizeRemoteAddr;	

	int local_sckt, remote_sckt, bind_sckt;
	int i_recv, i_send;
	char buffer[B_LEN];

	local_sckt = socket(AF_INET, SOCK_STREAM, 0);
	if (local_sckt < 0)
		printf("Falha. Nao possivel criar socket\n");
	else
		printf("Sucesso. Socket criado\n");

	localAddr.sin_family     = AF_INET;
	localAddr.sin_port       = htons(SERVER_PORT);
	memset(localAddr.sin_zero, 0x0, 8);
	bind_sckt = bind(local_sckt, (struct sockaddr*)&localAddr, sizeof(localAddr));
	if (bind_sckt < 0)
		printf("Falha. Erro ao vincular porta com socket\n");
	else
		printf("Sucesso. Vinculo de porta com socket efetuado\n");

	listen(local_sckt, 1);
	remote_sckt = accept(local_sckt, (struct sockaddr*)&remoteAddr, &sizeRemoteAddr);
	if (remote_sckt < 0)
		printf("Falha ao conectar com cliente\n");
	else{
		printf("Conexao estabelecida com cliente\n");
    
        memset(buffer, 0x0, B_LEN);
        strcpy(buffer, "Sua conexao foi aceita. Bem Vindo\0");
        send(remote_sckt, buffer, strlen(buffer), 0);
        while(1){
        	
    	    memset(buffer, 0x0, B_LEN);
    	    i_recv = recv(remote_sckt, buffer, B_LEN, 0);
    	    if (i_recv <= 0)
    	    	printf("Falha ao receber msg\n");
    	    printf("Msg do cliente = %s \n", buffer);

    	    memset(buffer, 0x0, B_LEN);
    	    strcpy(buffer, "Recebi sua msg O_- ");
    	    send(remote_sckt, buffer, B_LEN, 0);
        }
	}
	//close(local_sckt);
	printf("Conexao encerrada com cliente\n");
}