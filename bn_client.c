#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>

#define LOCAL_PORT  4343
#define REMOTE_PORT 4242
#define REMOTE_IP   "10.0.0.2"
#define B_LEN       4096

///-----Estructures para conexão-----///
struct sockaddr_in localAddr, remoteAddr;    
int sizeLocalAddr, sizeRemoteAddr;


///-----Variaveis globais-----///
int  local_sckt, remote_sckt, bind_sckt, play_first;
int  iwork1, i_send, i_recv, victory = 0;
char buffer[B_LEN];
char battleField[10][24];
char coord[2];
char c;


///----------------------------------------------///
///Funcoes
///----------------------------------------------///
int  upperCase(char str[]);
void getCoordinates();
void printField();
///----------------------------------------------///
///----------------------------------------------///

int main(){
    system("clear");

    local_sckt = socket(AF_INET, SOCK_DGRAM, 0);
    if (local_sckt < 0){
        printf("Falha não foi possível criar socket\n");
        return 1;       
    }
    else
        printf("Sucesso. Socket criado \n");

    localAddr.sin_family       = AF_INET;
    localAddr.sin_port         = htons(LOCAL_PORT);
    memset(localAddr.sin_zero, 0x0, 8);

    remoteAddr.sin_family      = AF_INET;
    remoteAddr.sin_port        = htons(REMOTE_PORT);
    remoteAddr.sin_addr.s_addr = inet_addr(REMOTE_IP);
    memset(remoteAddr.sin_zero, 0x0, 8);

    sizeLocalAddr  = sizeof(localAddr);
    sizeRemoteAddr = sizeof(remoteAddr);

    bind_sckt = bind(local_sckt, (struct sockaddr*)&localAddr, sizeLocalAddr);
    if (bind_sckt < 0){
        printf("Falha. Erro ao vincular porta com socket\n");
        return 1;
    }
    else
        printf("Sucesso. Vinculo de porta com socket efetuado. \nTentando conexao com servidor\n");

    memset(buffer, 0x0, B_LEN);
    strcpy(buffer, "Saudacoes servidor, estou pronto. Quem jogará primeiro?");
    sendto(local_sckt, buffer, B_LEN, 0, (struct sockaddr*)&remoteAddr, sizeRemoteAddr);

    recvfrom(local_sckt, buffer, B_LEN, 0, (struct sockaddr*)&remoteAddr, &sizeRemoteAddr);
    play_first = atoi(buffer);
    printf("Valor sorteado = %d\n", play_first);

    if (play_first == 0)            
            printf("Voce sera o segundo a jogar\n");
    else{
        recvfrom(local_sckt, battleField, sizeof(battleField), 0, (struct sockaddr*)&remoteAddr, &sizeRemoteAddr);
        printField();
        printf("Voce joga primeiro\n");
        getCoordinates();        
        sendto(local_sckt, coord, sizeof(coord), 0, (struct sockaddr*)&remoteAddr, sizeRemoteAddr);
        recvfrom(local_sckt, battleField, sizeof(battleField), 0, (struct sockaddr*)&remoteAddr, &sizeRemoteAddr);
        printField();
    }

    while(!victory){
        printf("Aguardando oponente...\n");
        recvfrom(local_sckt, battleField, sizeof(battleField), 0, (struct sockaddr*)&remoteAddr, &sizeRemoteAddr);
        printField();

        recvfrom(local_sckt, buffer, B_LEN, 0, (struct sockaddr*)&remoteAddr, &sizeRemoteAddr);
        victory = atoi(buffer);

        if (!victory){
            getCoordinates();           
            sendto(local_sckt, coord, sizeof(coord), 0, (struct sockaddr*)&remoteAddr, sizeRemoteAddr);
            recvfrom(local_sckt, battleField, sizeof(battleField), 0, (struct sockaddr*)&remoteAddr, &sizeRemoteAddr);
            printField();

            recvfrom(local_sckt, buffer, B_LEN, 0, (struct sockaddr*)&remoteAddr, &sizeRemoteAddr);
            victory = atoi(buffer);            
        }
    }

    if (victory == 1) printf("Seu adversário venceu!!! \n");
    if (victory == 2) printf("Voce venceu!!! :) \n");

    printf("---- FIM DE JOGO ----");
}

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

void getCoordinates(){    
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