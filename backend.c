//
// Created by Vasco on 10/31/2022.
//
#include "backend.h"

// Variáveis globais
int s_fifo, c_fifo, res;
int connectedUsers = 0;
int LeilaoStarted = 0;
int LeilaoFinished = 0;
int alarmPID;
struct LigacaoC userList[30];

void shutdown() {
    printf("Exiting program...\n");
    close(s_fifo);
    unlink(SERVER_FIFO);
}
void sigHandler(int sig) {
    if (sig == SIGINT) {
        shutdown();
        exit(0);
    }
}

void alarmHandler(int sig) {
    if (sig == SIGALRM) {
        //Tem de mandar um sigUSR1 a todos os clientes e todos os jogos para notificar que o jogo terminou.
        for (int i = 0; i < connectedUsers; i++) {
#ifdef DEBUG
            printf("Sending USR1 to user %s (%d)\n", userList[i].nome, userList[i].userPID);
      printf("Sending USR1 to game (%d)\n", userList[i].gamePID);
#endif
            kill(userList[i].userPID, SIGUSR1);
            kill(userList[i].gamePID, SIGUSR1);
        }
    }
}
void listAllLeiloes(char* leidir) {
    DIR* d; // Usado para ler os ficheiros existentes
    struct dirent* dir;

    d = opendir(leidir);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            //Verifica se o ficheiro começa por g e se acaba em c
            if (dir->d_name[0] == 'g' && dir->d_name[strlen(dir->d_name) - 1] == 'c')
                printf("-> %s\n", dir->d_name);
        }
        closedir(d);
    }
}
//exemplo de função de verificação
int verifInt(char* arg) {
    int num;
    if ((num = atoi(arg)) == 0 && arg != "0") {
        printf("[ERRO] Argumento invalido!\n");
        printf("Utilize -h para mais informacoes.\n");
        exit(4); // Erro Invalid Arguments
    }
    return num;
}
void* clientServerComm(void* list) {
    struct LigacaoServidor mensagemForClient;
    struct LigacaoCliente mensagemForServer;
    char nome_fifo_cliente[50];
    char auxMsg[TAM_MAX];

    do {
        res = read(s_fifo, &mensagemForServer, sizeof(mensagemForServer));
        if (res < 0) {
            perror("\n Erro a ler do cliente.");
        }
        if (mensagemForServer.status == 0)
            fprintf(stderr, "\n Cliente com o PID %d esta a tentar conectar.\n", mensagemForServer.userPID);
        else
            fprintf(stderr, "\n Mensagem recebida do cliente com o PID %d: [%s]\n", mensagemForServer.userPID, mensagemForServer.palavra);
        sprintf(nome_fifo_cliente, CLIENT_FIFO, mensagemForServer.userPID);

        // Remover cliente da lista de clientes
        if ((c_fifo = open(nome_fifo_cliente, O_WRONLY)) < 0) {
            shutdown();
            exit(EXIT_FAILURE);
        }

}

int main(int argc, char* argv[], char* envp[]) {
    int opt;
    char cmd[250];
    char *token;
    struct LigacaoServidor mensagemForCliente;
    char nome_fifo_cliente[50];
    struct LigacaoServidor mensagem_server;//pergunta
    struct LigacaoCliente mensagem_client;//resposta
    signal(SIGINT, sigHandler);
    signal(SIGALRM, alarmHandler);

    /* -- CRIAÇÃO DO FIFO SERVIDOR -- */

    if (mkfifo(nome_fifo, 0777) < 0) {
        perror("\n Erro ao criar o FIFO do cliente.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\n FIFO do cliente criado.\n");

    s_fifo = open(SERVER_FIFO, O_WRONLY);//abre o FIFO do servidor para escrita
    if (server_fifo < 0) {
        fprintf(stderr, "\n O cliente não consegue ligar-se ao servidor.\n");
        unlink(nome_fifo);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\n FIFO do servidor aberto para escrita.\n");

    if ((c_fifo = open(nome_fifo, O_RDWR)) < 0) { //o fifo do cliente so le
        perror("\n Erro ao abrir o FIFO do cliente.\n");
        close(server_fifo);
        unlink(nome_fifo);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\n FIFO do Cliente aberto para leitura.\n");

// agora o fifo deve estar aberto e por isso vai poder mandar mgs para o server;

    do {
        //leitura de comandos para o servidor
        fgets(cmd, sizeof(cmd), stdin);
        strtok(cmd, "\n");
        for (int i = 0; i < strlen(comando); i++){
            comando[i] = toupper(comando[i]);
        }

        //IMPLEMENTAR VERIFICACAO DE ARGUMENTOS

        if (strcmp(cmd, "LOGIN") == 0) {
            char user[18], password[18];
            int certo;

            do{
                printf("\nEscreva o seu username:");
                scanf("%s", mensagem_server.user);
                printf("Escreva a sua palavra-passas do user %s:", user);
                scanf("%s", mensagem_server.palavra;
                if (write(server_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                    printf("erro no envio da msg");
                }
            } while (certo);
//fim do login-----------------------------------------------------------------------------------------------
        }   else if (strcmp(cmd, "REGISTAR") == 0) {
            char pass2[50];
            int voltar;
            do{
                voltar = 0;
                printf("\nEscreva o username que quer:");
                scanf("%s", mensagem_client.user);
                printf("Escreva a palavra-pass do usar: %s:", user);
                scanf("%s", mensagem_client.palavran) ;
                printf("confirme a palavra-pass:");
                gets(pass2);
                if (strcmp(mensagem_client.palavran, pass2) == 0){
                    for (int i = 0; i < strlen(user); i++){
                        if (user[i] == ' '){
                            voltar = 1;
                            printf("o user tem que ser uma palavra apenas");
                        }
                    }
// em cima verifica se a mesmo palavra-passe que ele que se não for repete o processo de pedir as info se for;
// verifica se e a apenas uma palavra no user;

                    for (int i = 0; i < strlen(pass); i++){
                        if (pass[i] == ' '){
                            voltar = 1;
                            printf("o user tem que ser uma palavra apenas");
                        }
                    }
// verifica se e apenas uma palavra na palavra-pass
                }else{
                    voltar = 1;
                    printf("enganouse");
                }
            }while(voltar);
            mensagem_client.palavra = registar;
            if (write(server_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                printf("erro no envio da msg");
            }
//manda msg para i servidor com os nome do novo user e palavra-pass;
        }   else if (strcmp(cmd, "KICK") == 0) {
// primero if verifica se esta logado como status se for = 1;
            if (mensagem_client.status == 1){
                char comkick;
                mensagem_client.palavra = kick;
                do{
                    printf("quer mesmo apagar o user %s?\n"s"/"n" ", mensagem_client.user);
                    scanf("%c", &comkick);
                }while(comkick != 's' || comkick != 'n'|| comkick != 'S' || comkick != 'N');
                if(comkick != 's' || comkick != 'S'){
                    if (write(server_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                        printf("erro no envio da msg");
                    }
                }
            }
        }

        }   else if (strcmp(cmd, "PROM") == 0) { //lista utilizadores promotores atuais
        printf("comado valido");

        }   else if (strcmp(cmd, "COMPAR") == 0){
        printf("comado valido");

        }   else if (strcmp(cmd, "CANCEL") == 0) {
        printf("comado valido");

        }   else if (strcmp(cmd, "CLOSE") == 0) {//termina execucao
        printf("comado valido");

        }
        else {
            printf("\nComando nao detetado!\n");
        }
    } while (strcmp(cmd, "exit"));
    return 0;

}