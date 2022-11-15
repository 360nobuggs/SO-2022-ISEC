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
int main(int argc, char* argv[], char* envp[]) {
    int opt;
    char cmd[250];
    char *token;
    struct LigacaoS mensagemForCliente;
    char nome_fifo_cliente[50];

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

        //IMPLEMENTAR VERIFICACAO DE ARGUMENTOS
        if  (strcmp(cmd, "players") == 0) {

        }   else if (strcmp(cmd, "users") == 0) { //lista utilizadores cliente atuais

        }   else if (strcmp(cmd, "list") == 0) { //lista items á venda

        }   else if (strcmp(cmd, "kick") == 0) { //remove utilizador
            token = strtok(cmd, "k");
        }   else if (strcmp(cmd, "prom") == 0) { //lista utilizadores promotores atuais

        }   else if (strcmp(cmd, "reprom") == 0) { //atualiza promotores

        }   else if (strcmp(cmd, "cancel") == 0) { //cancela promotor

        }   else if (strcmp(cmd, "close") == 0) { //termina execucao

        }
        else {
            printf("\nComando nao detetado!\n");
        }
    } while (strcmp(cmd, "exit"));
    return 0;

}