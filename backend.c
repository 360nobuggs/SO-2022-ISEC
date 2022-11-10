//
// Created by Vasco on 10/31/2022.
//
#include "backend.h"
#include <wait.h>
#include <unistd.h>

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
