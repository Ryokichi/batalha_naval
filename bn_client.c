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

struct sockaddr_in localAddr, remoteAddr;	
socklen_t sizeRemoteAddr;

int local_sckt, remote_sckt, play_first;
char buffer[B_LEN];
char battleField[10][20];
char coord[2];


void printField(){
    char cy[2], c_aux;
    int i, j;
    printf("  | A|B|C|D|E|F|G|H|I|J | ");
    printf("  |----- SEU CAMPO -----|\n");
    printf("  |---------------------| ");
    printf("  |---------------------|\n");
    for(i = 0; i < 10; i++){
        memset(cy, 0x0,sizeof(char));
        switch(i){
            case 0: strcpy(cy," 1"); break;
            case 1: strcpy(cy," 2"); break;
            case 2: strcpy(cy," 3"); break;
            case 3: strcpy(cy," 4"); break;
            case 4: strcpy(cy," 5"); break;
            case 5: strcpy(cy," 6"); break;
            case 6: strcpy(cy," 7"); break;
            case 7: strcpy(cy," 8"); break;
            case 8: strcpy(cy," 9"); break;
            case 9: strcpy(cy,"10"); break;
        }        
        printf("%s| ", cy);
        ///Campo adversário
        for (j = 0; j < 10; j++){
            c_aux = battleField[i][j];
            if ((c_aux == 'W') || (c_aux != '*'))
            	c_aux = '~';
            printf("%c ", c_aux);
        }

        printf("| %s| ", cy);

        ///Seu campo
        for(j = 10; j < 20; j++){
            c_aux = battleField[i][j];
            if (c_aux == 'W') c_aux = '~';
            printf("%c ", c_aux);
        }        
        printf("|\n");
    }    
    printf("  |---------------------| ");
    printf("  |---------------------|\n");
    printf("  | A|B|C|D|E|F|G|H|I|J | ");
    printf("  |----- SEU CAMPO -----|\n\n");
}

//void validateCoord(){
//	int valid = 0;
//	while(!valid){
//
//		if(!valid){
//			memset(coord, 0x0, sizeof(coord));
//			printf("Coordenadas invalidas\n");
//			applyCoord();
//		}
//	}
//}

//void applyCoord(){	
//	printf("Informe suas coodenadas: ");
//	scanf("%c", coord);
//	validateCoord();
//}


void applyValidateCoord(){	
	int valid = 0;
	while(!valid){
		memset(coord, 0x0, sizeof(coord));
		printf("Informe suas coodenadas: ");
	    scanf("%s", coord);

		if(!valid){			
			printf("Coordenadas invalidas\n");			
		}
	}
}

int main(){
	system("clear");
	
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

		recv(local_sckt, buffer, sizeof(buffer), 0);
		printf("Msg do servidor: %s\n", buffer);

        recv(local_sckt, battleField, sizeof(battleField),0);
		recv(local_sckt, buffer, sizeof(buffer), 0);
		play_first = atoi(buffer);
		printf("Valor sorteado = %d\n", play_first);

		if (play_first == 0)        	
            printf("Voce sera o segundo a jogar\n");
        else{
        	printf("Voce joga primeiro\n");
        	printField();
        	applyValidateCoord();
        }

        
        
        
        	
/*
	    while(1){
		    memset(battleField, 0x0, sizeof(battleField));
	        recv (local_sckt, battleField, sizeof(battleField), 0);
            printf("Msg do servidor: %s\n", battleField[0] );

            memset(buffer, 0x0, B_LEN);
            scanf("%s", buffer);
            send(local_sckt, buffer, strlen(buffer), 0);
	    }*/
	}	
	printf("Conexao encerrada com servidor\n");
}