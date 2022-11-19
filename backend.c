//
// Created by Vasco on 10/31/2022.
//

#include "backend.h"

// Variáveis globais
int s_fifo, c_fifo, res;
int connectedUsers = 0;
int LeilaoStarted = 0;
int LeilaoFinished = 0;
struct LigacaoCliente utilizadores[20];

void shutdown() {
    printf("Exiting program...\n");
    close(s_fifo);
    unlink(SERVER_FIFO);
}

void* clientServerComm() {
    struct LigacaoServidor mensagemForClient;
    struct LigacaoCliente mensagemForServer;
    char nome_fifo_cliente[50];
    char auxMsg[TAM_MAX];
    //Mensagem establece comunicação
    res = read(s_fifo, &mensagemForServer, sizeof(mensagemForServer));
    if (res < 0) {
        perror("\n Erro a ler do cliente.");
    }
    if (mensagemForServer.status == 0)
        fprintf(stderr, "\n Cliente com o PID %d esta a tentar conectar.\n", mensagemForServer.userPID);
    else
        fprintf(stderr, "\n Mensagem recebida do cliente com o PID %d: [%s]\n", mensagemForServer.userPID,
                mensagemForServer.palavra);
            
    sprintf(nome_fifo_cliente, CLIENT_FIFO, mensagemForServer.userPID);
    fprintf(stderr, "\nPID %s .\n", nome_fifo_cliente);
     if ((c_fifo = open(nome_fifo_cliente, O_WRONLY)) < 0) {
            shutdown();
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "\nA enviar mensagem para cliente %d .\n", mensagemForServer.userPID);
            strcpy(mensagemForClient.palavra, "Bem-vindo! Pode agora inserir comandos.\n\n");
            res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                if (res < 0) {
                        perror("\n Erro a escrever para o cliente.");
                    }
           }

    //Login ou Registo
     do {
        res = read(s_fifo, &mensagemForServer, sizeof(mensagemForServer));
        if (res < 0) {
            perror("\n Erro a ler do cliente.");
        }
        else
        {
            if(strcpy(mensagemForServer.palavra, "login"))
            {

            }else if(strcpy(mensagemForServer.palavra, "registar"))
            {

            }else if(strcpy(mensagemForServer.palavra, "comando1"))
            {

            }else if(strcpy(mensagemForServer.palavra, "comando2"))
            {

            }else if(strcpy(mensagemForServer.palavra, "comando3"))
            {

            }else if(strcpy(mensagemForServer.palavra, "comando4"))
            {

            }else{
            fprintf(stderr, "\nComando nao reconhecido do cliente %d .\n", mensagemForServer.userPID);
            strcpy(mensagemForClient.palavra, "Comando não reconhecido.\n\n");
            res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                if (res < 0) {
                        perror("\n Erro a escrever para o cliente.");
                    }
            }
        }

     }while(1);
   








    //Comandos do cliente para servidor




    do {
        res = read(s_fifo, &mensagemForServer, sizeof(mensagemForServer));
        if (res < 0) {
            perror("\n Erro a ler do cliente.");
        }
        if (mensagemForServer.status == 0)
            fprintf(stderr, "\n Cliente com o PID %d esta a tentar conectar.\n", mensagemForServer.userPID);
        else
            fprintf(stderr, "\n Mensagem recebida do cliente com o PID %d: [%s]\n", mensagemForServer.userPID,
                    mensagemForServer.palavra);
               
        sprintf(nome_fifo_cliente, CLIENT_FIFO, mensagemForServer.userPID);
        fprintf(stderr, "\nPID %s .\n", nome_fifo_cliente);
       
        if ((c_fifo = open(nome_fifo_cliente, O_WRONLY)) < 0) {
            shutdown();
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "\nA enviar mensagem para cliente %d .\n", mensagemForServer.userPID);
            strcpy(mensagemForClient.palavra, "Bem-vindo! Pode agora inserir comandos.\n\n");
            res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
            //adicionar utilizador se nao existe
            if(strcmp(mensagemForServer.palavra, "login"))
            {
                //if(isUserValid(mensagemForServer.palavra,mensagemForServer.password)!=-1)
                if(1)
                {
                    //sprintf(auxMsg, "Bem vindo de volta %s.\n\n", mensagemForServer.user);
                    strcpy(mensagemForClient.palavra, auxMsg);
                    sleep(20);
                    res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                    if (res < 0) {
                        perror("\n Erro a escrever para o cliente.");
                    }
                }
                else
                {
                    //REGISTA NOVO UTILIZADOR
                    //sprintf(auxMsg, "Novo utilizador %s registado.\n\n", mensagemForServer.user);
                    strcpy(mensagemForClient.palavra, auxMsg);
                    res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                    if (res < 0) {
                        perror("\n Erro a escrever para o cliente.");
                    }
                }
            }

        }

    } while (1);
}

int main(int argc, char* argv[], char* envp[]) {
    int opt;
    char cmd[250];
    char *token;
    struct LigacaoServidor mensagemForCliente;
    char nome_fifo[50];
    struct LigacaoServidor mensagem_server;//pergunta
    struct LigacaoCliente mensagem_client;//resposta

    //criar um filho que roda em background e que está a escrever todas as promoções em uma ficheiro

    int id = fork();
    if(id == 0){
        int file, file2;
        int pidfilho = getpid();
        printf("%i", pidfilho);
        file = open("prome.txt", O_WRONLY | O_CREAT, 0777);
        if(file == -1){
            printf("erro na cricao da pasta");
            return 2;
        }
        printf("\nescrever as prome para o arquivo: prome.txt");
        file2 = dup2(file, 1);
        close(file);
        int error231 = execl("promotor_oficial", "promotor_oficial", NULL);
        if(error231 == -1){
            printf("codigo nao enconntador para execl correr");
            return 3;
        }
       
    }else {

        //clientServerComm();
        /* -- CRIAÇÃO DO FIFO SERVIDOR -- */

        if (res = mkfifo(SERVER_FIFO, 0777) < 0) {
            perror("\n Erro ao criar o FIFO do cliente.\n");
            shutdown();
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "\n FIFO do servidor criado.\n");

        s_fifo = open(SERVER_FIFO, O_RDWR);//abre o FIFO do servidor para escrita
        if (s_fifo < 0) {
            fprintf(stderr, "\n Erro a abrir o FIFO do servidor.\n");
            unlink(nome_fifo);
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "\n FIFO do servidor aberto para leitura.\n");

        clientServerComm();


        do {
            fgets(cmd, sizeof(cmd), stdin);
            strtok(cmd, "\n");
            for (int i = 0; i < strlen(cmd); i++) {
                cmd[i] = toupper(cmd[i]);
            }
            //IMPLEMENTAR VERIFICACAO DE ARGUMENTOS

            if (strcmp(cmd, "LOGIN") == 0) {


            } else if (strcmp(cmd, "KICK") == 0) {

            } else if (strcmp(cmd, "PROM") == 0) { //lista utilizadores promotores atuais

            } else if (strcmp(cmd, "REPROM") == 0) { //atualiza promotores

            } else if (strcmp(cmd, "CANCEL") == 0) { //cancela promotor

            } else if (strcmp(cmd, "CLOSE") == 0) { //termina execucao

            } else {
                printf("\nComando nao detetado!\n");
            }
        } while (!strcmp(cmd, "exit"));
    }
    return 0;

}