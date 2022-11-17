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

   if ((res = mkfifo(SERVER_FIFO, 0777)) < 0) {
        perror("\nErro ao criar o FIFO do Servidor.\n");
        shutdown();
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\nFIFO servidor criado com sucesso!");

    if ((s_fifo = open(SERVER_FIFO, O_RDWR)) < 0) {
        perror("\nErro ao abrir o FIFO do servidor(RDWR/BLOCKING).\n");
        shutdown();
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\nFIFO aberto para leitura.\n");


    do {
        //leitura de comandos para o servidor
        fgets(cmd, sizeof(cmd), stdin);
        strtok(cmd, "\n");
        for (int i = 0; i < strlen(comando); i++){
            comando[i] = toupper(comando[i]);
        }

        //IMPLEMENTAR VERIFICACAO DE ARGUMENTOS
        if  (strcmp(cmd, "players") == 0) {

        }   else if (strcmp(cmd, "LOGIN") == 0) {
            char user[18], password[18];
            int certo;
            do{
                printf("\nEscreva o seu username:");
                scanf("%s", mensagem_server.user);
                printf("Escreva a sua palavra-passas do user %s:", user);
                scanf("%s", mensagem_server.palavra;
                int fd = open("SERVER_FIFO", O_RDWR);
                if (write(server_fifo, &mensagem_client, sizeof(mensagem_client)) == -1){
                    printf("erro no envio da msg");
                    //escreve no fifo do servidor para ele ler usando a mensagem para servidor
                }
                close(fd);
            } while (certo);
//fim do login-----------------------------------------------------------------------------------------------
        }   else if (strcmp(cmd, "LIST") == 0) { //lista items á venda

        }   else if (strcmp(cmd, "KICK") == 0) { //remove utilizador
            token = strtok(cmd, "k");
        }   else if (strcmp(cmd, "PROM") == 0) { //lista utilizadores promotores atuais

        }   else if (strcmp(cmd, "REPROM") == 0) { //atualiza promotores

        }   else if (strcmp(cmd, "CANCEL") == 0) { //cancela promotor

        }   else if (strcmp(cmd, "CLOSE") == 0) { //termina execucao

        }
        else {
            printf("\nComando nao detetado!\n");
        }
    } while (strcmp(cmd, "exit"));
    return 0;

}