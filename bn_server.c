#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>

#define SERVER_PORT 4242
#define B_LEN 4096  ////tamanho do buffer para trocar mensagens

///Estrututra para conexões
struct sockaddr_in localAddr, remoteAddr;
socklen_t sizeRemoteAddr;

///Variáveis Globais
int local_sckt, remote_sckt, bind_sckt;
int i_recv, i_send, play_first = 0;
int i, j, align, collision, victory = 0;
char coord[2];
char buffer[B_LEN];

////O servidor controlará as jogadas e cuidará de replicar para o cliente o status atual
char battleField[10][24] = {
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W', '-','-', '-','-'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W', '-','-', '-','-'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W', '-','-', '-','-'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W', '-','-', '-','-'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W', '-','-', '-','-'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W', '-','-', '-','-'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W', '-','-', '-','-'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W', '-','-', '-','-'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W', '-','-', '-','-'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W', '-','-', '-','-'}
};   

///-------------------------------------------------------------------------///
///-------------------------------------------------------------------------///
int checkCoordinate(int i, int j){    
    return ((battleField[i][j]) != 'W');    
}

int checkCollision(int i, int j, int align, int shipSize){    
    int collision = 0;
    int side_ctrl = 0;

    if (j >= 10) side_ctrl = 10;
    ///Verifica se a coordenada X, Y é diferente de W    
    ///Precisa ser melhorado pois checa o mesmo ponto e fora da matriz    
    
    switch(align){
        case 0: ///Vertical
            for (int c = 0; c < shipSize; c++){                    
                collision = checkCoordinate((i + c) , j );
                if (collision) goto shipCollided;
                
                if(i > 0){
                    collision = checkCoordinate((i + c - 1), j);
                    if (collision) goto shipCollided;
                }
                if((i + shipSize - 1) < 9){
                    collision = checkCoordinate((i + c + 1), j);
                    if (collision) goto shipCollided;
                }
                if(j > 0){
                    collision = checkCoordinate((i + c), (j - 1));
                    if (collision) goto shipCollided;
                }
                if(j < (9 + side_ctrl)){
                    collision = checkCoordinate((i + c), (j + 1));
                    if (collision) goto shipCollided;
                }
            }            
            break;
        case 1: ///Horizontal
            for (int c = 0; c < shipSize; c++){                    
                collision = checkCoordinate(i, (j + c));
                if (collision) goto shipCollided;
                    
                if(i > 0){
                    collision = checkCoordinate((i - 1), (j + c));
                    if (collision) goto shipCollided;
                }
                if(i < 9){
                    collision = checkCoordinate((i + 1), (j + c));
                    if (collision) goto shipCollided;
                }
                if(j > (0 + side_ctrl)){
                    collision = checkCoordinate(i, (j + c - 1));
                    if (collision) goto shipCollided;
                }
                if((j + shipSize) < (9 + side_ctrl)){
                    collision = checkCoordinate(i, (j + c + 1));
                    if (collision) goto shipCollided;
                }
            }
        break;
    }    
    shipCollided:
    return collision;
}


void creatShip(int shipSize, char shipType){    
    if (align == 0){    
        for (int c = 0; c < shipSize; c++){
              battleField[i+c][j] = shipType;
        }
    }else{
        for (int c = 0; c < shipSize; c++){
            battleField[i][j+c] = shipType;
        }
    }    
}


void shuffleField(){
    ///A = Aircraft Carrier
    ///B = Battleship
    ///C = Cruiser
    ///S = Submarine
    ///W = Water    

    ///Para Aircraft Carrier. Não é necessario ver colisao pois é o primeiro
    align = (rand()%2);
    i = (rand()%7);
    j = (rand()%7);
    creatShip(4, 'A'); ///Para campo aliado
    
    align = (rand()%2);
    i = ((rand()%7));
    j = ((rand()%7)+10);
    creatShip(4, 'A'); ///Para campo inimigo    
    

    ///Para Battleship
    for (int s = 1; s <= 2; s++){
        collision = 1;
        align = (rand()%2);
        while(collision){
            i = (rand()%8);
            j = (rand()%8);
            collision = checkCollision(i, j, align, 3);
        }
        creatShip(3, 'B');

        collision = 1;
        align = (rand()%2);
        while(collision){
            i = (rand()%8);
            j = (rand()%8+10);
            collision = checkCollision(i, j, align, 3);
        }
        creatShip(3, 'B');
    }

    ///Para Crusador
    for(int s = 0; s < 3; s++){
        collision = 1;
        align = (rand()%2);
        while(collision){
            i = (rand()%9);
            j = (rand()%9);
            collision = checkCollision(i, j, align, 2);
        }
        creatShip(2, 'C');

        collision = 1;
        align = (rand()%2);
        while(collision){
            i = (rand()%9);
            j = (rand()%9+10);
            collision = checkCollision(i, j, align, 2);
        }
        creatShip(2, 'C');
    }


    //Para Submarinos
    for(int s = 0; s < 4; s++){
        collision = 1;
        align = (rand()%2);
        while(collision){
            i = (rand()%10);
            j = (rand()%10);
            collision = checkCollision(i, j, align, 1);
        }
        creatShip(1, 'S');

        collision = 1;
        while(collision){
            i = (rand()%10);
            j = (rand()%10+10);
            collision = checkCollision(i, j, align, 1);
        }
        creatShip(1, 'S');
    }
}

int tossCoin(){
    int coin;    
    coin = (((rand()%7)%3)%2);
    printf("Valor sorteado = %d\n", coin);
    return coin;
}

void msgToClient(char buffer[B_LEN]){
    //send(remote_sckt, buffer, strlen(buffer), 0);    
}

void printField(){
    system("clear");
    char c_aux;
    int i, j;

    printf("  |---------------------| ");    
    printf("  |----- SEU CAMPO -----|");
    printf("    |JOGADAS|\n");
    printf("  | A|B|C|D|E|F|G|H|I|J | ");
    printf("  | A|B|C|D|E|F|G|H|I|J |");
    printf("    |AD| |VC|\n");
    for(i = 0; i < 10; i++){        
        printf(" %d| ", i);
        ///Campo adversário
        for (j = 10; j < 20; j++){
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
        for(j = 0; j < 10; j++){
            c_aux = battleField[i][j];
            if (c_aux == 'W') c_aux = '~';
            printf("%c ", c_aux);
        }
        printf("|    |");

        for(j = 22; j < 24; j++){
            printf("%c", battleField[i][j]);
        }
        printf("| |");
        for(j = 20; j < 22; j++){
            printf("%c", battleField[i][j]);
        }
        printf("|\n");
    }    
    printf("  | A|B|C|D|E|F|G|H|I|J | ");
    printf("  | A|B|C|D|E|F|G|H|I|J |\n");
    printf("  |---------------------| ");    
    printf("  |----- SEU CAMPO -----|\n\n");
}

int upperCase(char str[]){
    for (int i = 0; str[i] != 0; i++){
        str[i] = toupper(str[i]);
    }
}
void changeField(){

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
    changeField();
}

void validateShot(int player){
    int cy, cx;
    char c_aux;
    cy = (coord[0] - 48);
    c_aux = coord[1];

    switch(c_aux){
        case 'A': cx = 0; break;
        case 'B': cx = 1; break;
        case 'C': cx = 2; break;
        case 'D': cx = 3; break;
        case 'E': cx = 4; break;
        case 'F': cx = 5; break;
        case 'G': cx = 6; break;
        case 'H': cx = 7; break;
        case 'I': cx = 8; break;
        case 'J': cx = 9; break;        
    }   

    if (player == 1){
        for (int i = 0; i < 9; i++){
            for(int j = 20; j < 22; j++){
                battleField[i][j] = battleField[i+1][j];
            }
        }
        battleField[9][20] = coord[0];
        battleField[9][21] = coord[1];
    }else{
        for (int i = 0; i < 9; i++){
            for(int j = 22; j < 24; j++){
                battleField[i][j] = battleField[i+1][j];
            }
        }
        battleField[9][22] = coord[0];
        battleField[9][23] = coord[1];
    }

    if (player == 1) cx += 10;    ////se for player 1 aplica coordenadas no campo 2
    c_aux = battleField[cy][cx];
    if (c_aux == 'A' || c_aux == 'B' || c_aux == 'C' || c_aux == 'S' || c_aux == 'X') 
        battleField[cy][cx] = 'X';
    else
        battleField[cy][cx] = '*';
}

int checkVictory(int player){
    printf("Player = %d \n", player);
    int cont = 0;
    int i, j, j_aux;
    
    for (i = 0; i < 10; i++){        
        j = 0;
        if (player == 1)  ///Olha campo do player 2 par ver acertos
            j = 10;
        j_aux = j + 10;

        for ( ;j < j_aux; j++){            
            if (battleField[i][j] == 'X') cont ++;
        }
    }
    printf("Cont = %d \n", cont );
    if (cont == 20)
        return player;
    else 
        return 0;
}

int main(){
    //system("clear");
    srand(time(NULL));
    shuffleField();

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
        printField();
        printf("Conexao estabelecida com cliente\n");

        memset(buffer, 0x0, B_LEN);
        strcpy(buffer, "Bem Vindo. Sua conexao foi aceita.\0");
        send(remote_sckt, buffer, sizeof(buffer),0);

        
        send(remote_sckt, battleField, sizeof(battleField), 0);        

        play_first = tossCoin();
        sprintf(buffer, "%d", play_first);
        send(remote_sckt, buffer, sizeof(buffer), 0);


        if (play_first == 1){
            printf("Voce sera o segundo a jogar\n\n");
            printf("Aguardando oponente...\n");
            recv(remote_sckt, coord, sizeof(coord), 0); ///É assumido que as coordenadas são validas
            validateShot(2);  ///player 2
            printf("Coordenadas informadas: %s\n", coord);
            send(remote_sckt, battleField, sizeof(battleField), 0);

        }else{
            printf("Voce joga primeiro\n\n");
        }

        while(!victory){
            applyValidateCoord();
            validateShot(1);  ///player 1
            victory = checkVictory(1);
            send(remote_sckt, battleField, sizeof(battleField), 0);

            memset(buffer, 0x0,B_LEN);
            sprintf(buffer, "%d", victory);
            send(remote_sckt, buffer, sizeof(buffer), 0);
            printField();

            printf("Aguardando oponente...\n");

            if(!victory){
                recv(remote_sckt, coord, sizeof(coord),0);
                validateShot(2);  ///player 2
                victory = checkVictory(2);
                send(remote_sckt, battleField, sizeof(battleField), 0);

                memset(buffer, 0x0,B_LEN);
                sprintf(buffer, "%d", victory);
                send(remote_sckt, buffer, sizeof(buffer), 0);
                printField();
            }
        }        
    }
    if(victory == 1)
        printf("Voce venceu!!! :)\n");
    else
        printf("Seu adversario venceu!!! \n");

    printf("Conexao encerrada com cliente\n");
}