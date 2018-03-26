#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>

#define SERVER_PORT 4242
#define B_LEN 4096

struct sockaddr_in localAddr, remoteAddr;    
socklen_t sizeRemoteAddr;

int local_sckt, remote_sckt, play_first;
int iwork1, victory = 0;
char buffer[B_LEN];
char battleField[10][24];
char coord[2];


void printField(){
    system("clear");
    char c_aux;
    int i, j;

    printf("  |---------------------| ");    
    printf("  |----- SEU CAMPO -----|");
    printf("    |JOGADAS|\n");
    printf("  | A|B|C|D|E|F|G|H|I|J | ");
    printf("  |---------------------|");
    printf("    |AD| |VC|\n");
    for(i = 0; i < 10; i++){        
        printf(" %d| ", i);
        ///Campo adversário
        for (j = 0; j < 10; j++){
            c_aux = battleField[i][j];
            switch(c_aux){
                case 'W': 
                case 'A':
                case 'B':
                case 'C':
                case 'S': c_aux = '~'; break;
            }
            printf("%c ", c_aux);
        }

        printf("|%d  | ", i);

        ///Seu campo
        for(j = 10; j < 20; j++){
            c_aux = battleField[i][j];
            if (c_aux == 'W') c_aux = '~';
            printf("%c ", c_aux);
        }        
        printf("|    |");

        for(j = 20; j < 22; j++){
            printf("%c", battleField[i][j]);
        }
        printf("| |");
        for(j = 22; j < 24; j++){
            printf("%c", battleField[i][j]);
        }
        printf("|\n");
    }    
    printf("  | A|B|C|D|E|F|G|H|I|J | ");
    printf("  |---------------------|\n");
    printf("  |---------------------| ");    
    printf("  |----- SEU CAMPO -----|\n\n");
}

int upperCase(char str[]){
    for (int i = 0; str[i] != 0; i++){
        str[i] = toupper(str[i]);
    }
}

void applyValidateCoord(){    
    int valid = 0;
    while(!valid){
        memset(coord, 0x0, sizeof(coord));
        printf("Informe coodenadas YX: ");
        scanf("%s", coord);
        upperCase(coord);        
        printf("Y = %c | X = %c\n", coord[0], coord[1]);
        
        switch(coord[0]){
            case '0': valid = 1; break;
            case '1': valid = 1; break;
            case '2': valid = 1; break;
            case '3': valid = 1; break;
            case '4': valid = 1; break;
            case '5': valid = 1; break;
            case '6': valid = 1; break;
            case '7': valid = 1; break;
            case '8': valid = 1; break;
            case '9': valid = 1; break;
            default:  valid = 0;
        }
        if (valid){            
            switch(coord[1]){
                case 'A': valid = 1; break;
                case 'B': valid = 1; break;
                case 'C': valid = 1; break;
                case 'D': valid = 1; break;
                case 'E': valid = 1; break;
                case 'F': valid = 1; break;
                case 'G': valid = 1; break;
                case 'H': valid = 1; break;
                case 'I': valid = 1; break;
                case 'J': valid = 1; break;
                default:  valid = 0;
            }
        }
        if(!valid)
            printf("Coordenadas invalidas\n");        
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
        printField();
        play_first = atoi(buffer);
        printf("Valor sorteado = %d\n", play_first);

        if (play_first == 0)            
            printf("Voce sera o segundo a jogar\n");
        else{
            printf("Voce joga primeiro\n");
            printField();
            applyValidateCoord();
            send(local_sckt, coord, sizeof(coord), 0);
            recv(local_sckt, battleField, sizeof(battleField), 0);
            printField();
        }      
        
        while(!victory){
            printf("Aguardando oponente... \n");           
            recv(local_sckt, battleField, sizeof(battleField), 0);

            memset(buffer, 0x0, B_LEN);
            recv(local_sckt, buffer, sizeof(buffer), 0);
            victory = atoi(buffer);
            printField();

            if (!victory){
                applyValidateCoord();
                send(local_sckt, coord, sizeof(coord),0);
                recv(local_sckt, battleField, sizeof(battleField), 0);

                memset(buffer, 0x0, B_LEN);
                recv(local_sckt, buffer, sizeof(buffer), 0);
                victory = atoi(buffer);
                printField();
            }
        }
    }
    if(victory == 1)
        printf("Seu adversario venceu!!!\n");
    else
        printf("Voce venceu!!! :)\n");

    printf("Conexao encerrada com servidor\n");
}