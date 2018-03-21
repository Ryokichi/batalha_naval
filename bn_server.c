#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER_PORT 3132
#define B_LEN 4096  ////tamanho do buffer para trocar mensagens


////O servidor controlará as jogadas e cuidará de replicar para o cliente o status atual
char battleField[10][20] = {
    {'~','~','~','~','~','~','~','~','~','~','-','-','-','-','-','-','-','-','-','-'},
    {'~','~','~','~','~','~','~','~','~','~','-','-','-','-','-','-','-','-','-','-'},
    {'~','~','~','~','~','~','~','~','~','~','-','-','-','-','-','-','-','-','-','-'},
    {'~','~','~','~','~','~','~','~','~','~','-','-','-','-','-','-','-','-','-','-'},
    {'~','~','~','~','~','~','~','~','~','~','-','-','-','-','-','-','-','-','-','-'},
    {'~','~','~','~','~','~','~','~','~','~','-','-','-','-','-','-','-','-','-','-'},
    {'~','~','~','~','~','~','~','~','~','~','-','-','-','-','-','-','-','-','-','-'},
    {'~','~','~','~','~','~','~','~','~','~','-','-','-','-','-','-','-','-','-','-'},
    {'~','~','~','~','~','~','~','~','~','~','-','-','-','-','-','-','-','-','-','-'},
    {'~','~','~','~','~','~','~','~','~','~','-','-','-','-','-','-','-','-','-','-'}
};

///Estrututra para conexões
struct sockaddr_in localAddr, remoteAddr;
socklen_t sizeRemoteAddr;

///Variáveis
int local_sckt, remote_sckt, bind_sckt;
int i_recv, i_send, play_first = 0;
char coord[2];
char buffer[B_LEN];    


int tossCoin(){
	int coin;
	srand(time(NULL));
	coin = ((((rand()+rand())%7)%3)%2);
	printf("Valor sorteado = %d\n", coin);
	return 1;
}

void msgToClient(char buffer[B_LEN]){
    //send(remote_sckt, buffer, strlen(buffer), 0);    
}
void printField(){
	char cy[1];
	int i, j;
	printf("  | 0|1|2|3|4|5|6|7|8|9 | ");
	printf(  "|----- SEU CAMPO -----|\n");
	printf("  |---------------------| ");
	printf(  "|---------------------|\n");
	for(i = 0; i < 10; i++){
		memset(cy, 0x0,sizeof(char));
		switch(i){
			case 0: strcpy(cy,"A"); break;
			case 1: strcpy(cy,"B"); break;
			case 2: strcpy(cy,"C"); break;
			case 3: strcpy(cy,"D"); break;
			case 4: strcpy(cy,"E"); break;
			case 5: strcpy(cy,"F"); break;
			case 6: strcpy(cy,"G"); break;
			case 7: strcpy(cy,"H"); break;
			case 8: strcpy(cy,"I"); break;
			case 9: strcpy(cy,"J"); break;
		}		
		printf("%s | ", cy);

		///Seu campo
		for(j = 0; j < 10; j++){
			printf("%c ", battleField[i][j]);
		}
		///Campo adversário
		printf("| | ");
		for (j = 10; j < 20; j++){
			printf("%c ", battleField[i][j]);
		}
		printf("|\n");
	}	
	printf("  |---------------------| ");
	printf(  "|---------------------|\n");
	printf("  | 0|1|2|3|4|5|6|7|8|9 | ");
	printf(  "|----- SEU CAMPO -----|\n\n");
}

int main(){
    ///Inicia o socket local
    local_sckt = socket(AF_INET, SOCK_STREAM, 0);
    if (local_sckt < 0)
        printf("Falha. Nao possivel criar socket\n");
    else
        printf("Sucesso. Socket criado\n");

    ///Configura a porta e vincula com o socket
    localAddr.sin_family     = AF_INET;
    localAddr.sin_port       = htons(SERVER_PORT);
    memset(localAddr.sin_zero, 0x0, 8);
    bind_sckt = bind(local_sckt, (struct sockaddr*)&localAddr, sizeof(localAddr));
    if (bind_sckt < 0)
        printf("Falha. Erro ao vincular porta com socket\n");
    else
        printf("Sucesso. Vinculo de porta com socket efetuado \nAguardando conexao do cliente\n");

    ///Aguarda a requisição de uma conexão pelo cliente e então estabelece
    listen(local_sckt, 1);
    remote_sckt = accept(local_sckt, (struct sockaddr*)&remoteAddr, &sizeRemoteAddr);
    if (remote_sckt < 0)
        printf("Falha ao conectar com cliente\n");
    else{
        printf("Conexao estabelecida com cliente\n");

        memset(buffer, 0x0, B_LEN);
        strcpy(buffer, "Bem Vindo. Sua conexao foi aceita.\0");
        send(remote_sckt, buffer, sizeof(buffer),0);

        //shuffleField();
        send(remote_sckt, battleField, sizeof(battleField), 0);        

        play_first = tossCoin();
        sprintf(buffer, "%d", play_first);
        send(remote_sckt, buffer, sizeof(buffer), 0);

        printField();

        if (play_first == 1){
        	printf("Voce sera o segundo a jogar\n\n");
        	printf("Aguardando oponente...\n");
        	recv(remote_sckt, coord, sizeof(coord), 0); ///É assumido que as coordenadas são validas


        }else{
        	printf("Voce joga primeiro\n\n");
        }
        	


        /*
        while(1){            
            memset(buffer, 0x0, B_LEN);
            i_recv = recv(remote_sckt, buffer, B_LEN, 0);
            if (i_recv <= 0)
                printf("Falha ao receber msg\n");
            printf("Msg do cliente = %s \n", buffer);

            memset(buffer, 0x0, B_LEN);
            strcpy(buffer, "Recebi sua msg O_- ");
            send(remote_sckt, battleField, sizeof(battleField), 0);
        }*/
        memset(buffer, 0x0, B_LEN);
        i_recv = recv(remote_sckt, buffer, B_LEN, 0);
    }    
    printf("Conexao encerrada com cliente\n");
}