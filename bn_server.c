#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER_PORT 4242
#define B_LEN 4096  ////tamanho do buffer para trocar mensagens

///Estrututra para conexões
struct sockaddr_in localAddr, remoteAddr;
socklen_t sizeRemoteAddr;

///Variáveis Globais
int local_sckt, remote_sckt, bind_sckt;
int i_recv, i_send, play_first = 0;
int i, j, align, collision;
char coord[2];
char buffer[B_LEN];

////O servidor controlará as jogadas e cuidará de replicar para o cliente o status atual
char battleField[10][20] = {
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','W','W','W','W','W','X',  'W','W','W','W','W','W','W','W','W','*'},
    {'W','W','W','W','W','W','W','W','W','W',  'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','W','W','W','W','W','*',  'W','W','W','W','W','W','W','W','X','W'}
};   

///-------------------------------------------------------------------------///
///-------------------------------------------------------------------------///
int checkCoordinate(int i, int j){
	printf("Checando %dx%d = %c\n",i, j, battleField[i][j] );
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
    printField();
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
    printField();
    

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

    printField();
}



int tossCoin(){
    int coin;    
    coin = (((rand()%7)%3)%2);
    printf("Valor sorteado = %d\n", coin);
    return 1;
}

void msgToClient(char buffer[B_LEN]){
    //send(remote_sckt, buffer, strlen(buffer), 0);    
}

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
        for (j = 10; j < 20; j++){
            c_aux = battleField[i][j];
            if ((c_aux == 'W') || (c_aux != '*'))
            	c_aux = '~';
            printf("%c ", c_aux);
        }

        printf("| %s| ", cy);

        ///Seu campo
        for(j = 0; j < 10; j++){
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
        printf("Conexao estabelecida com cliente\n");

        memset(buffer, 0x0, B_LEN);
        strcpy(buffer, "Bem Vindo. Sua conexao foi aceita.\0");
        send(remote_sckt, buffer, sizeof(buffer),0);

        
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