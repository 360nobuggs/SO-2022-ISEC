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
    int logged_in=0;

    //Leitura de users
    char*path= "utilizadores.txt";
    int op = loadUsersFile(path);
    fprintf(stderr, "\n Resultado de loadUsersGile %d , %s.\n", op,getLastErrorText());
    int saldo=0;
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
                int code=isUserValid(mensagemForServer.user, mensagemForServer.password);
                if(code==1)
                {
                    //user correcto
                    mensagemForClient.valor=1;
                    strcpy(mensagemForClient.palavra, "Login efetuado, bem vindo .\n\n"); 
                    fprintf(stderr, "\nUtilizador %s conectado.\n", mensagemForServer.user);                       
                    logged_in=1;
                }
                else if(code==0){
                    //login errado
                    mensagemForClient.valor=0;
                    strcpy(mensagemForClient.palavra, "Login incorreto.\n");
                }else{
                    //outro erro
                    strcpy(mensagemForClient.palavra, ("Erro encontrado:  %s.\n\n",getLastErrorText()));
                }
                res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
            }else if(strcpy(mensagemForServer.palavra, "registar"))
            {

            }
            else{
            fprintf(stderr, "\nComando nao reconhecido do cliente %d .\n", mensagemForServer.userPID);
            strcpy(mensagemForClient.palavra, "Comando não reconhecido.\n\n");
            res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                if (res < 0) {
                        perror("\n Erro a escrever para o cliente.\n");
                    }
            }
        }

     }while(logged_in==0);

 
    do{
        //aguarda comandos
        res = read(s_fifo, &mensagemForServer, sizeof(mensagemForServer));
        if (res < 0) {
            perror("\n Erro a ler do cliente.");
        }
        else
        {
            fprintf(stderr, "\nComando %s recebido.\n", mensagemForServer.palavra);
            if(strcpy(mensagemForServer.palavra, "saldo"))
            {
                char *ptr= mensagemForServer.user;
                if((saldo=getUserBalance(ptr))!=-1)
                {
                    //mensagem com o saldo
                    strcpy(mensagemForClient.palavra, "O saldo é ");

                    mensagemForClient.valor=saldo;
                    
                    //strcpy(mensagemForClient.palavra,"\n O seu saldo é:  \n");
                    fprintf(stderr, "\nValor do saldo %d ",saldo);
                    //atualiza saldo
                    op= updateUserBalance(mensagemForServer.user, saldo-1);
                    if(op==-1)
                    {
                        fprintf(stderr, "\n Erro a atualizar saldo %d , %s.\n", op,getLastErrorText());
                    }
                    op= saveUsersFile(path);
                    if(op==-1)
                    {
                        fprintf(stderr, "\n Erro a guardar %d , %s.\n", op,getLastErrorText());
                    }
                }
                else{
                   
                }
                fprintf(stderr, "\n palavra: %s , saldo :%d .\n", mensagemForClient.palavra,mensagemForClient.valor);
                res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                if (res < 0) {
                        perror("\n Erro a escrever para o cliente.");
                    }

            }else if(strcpy(mensagemForServer.palavra, "listar"))
            {

                //listar utilizadores

            }
            else if(strcmp(mensagemForServer.palavra,"exit"))
            {
                fprintf(stderr,"\nPrograma terminado por comando do cliente.\n");
                shutdown();
                exit(EXIT_SUCCESS);
            }else{
            fprintf(stderr, "\nComando nao reconhecido do cliente %d .\n", mensagemForServer.userPID);
            strcpy(mensagemForClient.palavra, "Comando não reconhecido.\n\n");
            res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                if (res < 0) {
                        perror("\n Erro a escrever para o cliente.");
                    }
        }}
    }while(1);
    shutdown();
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
    //FILE *p = fopen("utilizadores.txt","anymode");
    int op=0;
    char*nome= "filipe";
    char*pass= "pass";
    char*path= "utilizadores.txt";
    op = loadUsersFile(path);
    fprintf(stderr, "\n Resultado de loadUsersGile %d , %s.\n", op,getLastErrorText());

    op= getUserBalance(nome);
    fprintf(stderr, "\n Resultado de saldo %d , %s.\n", op,getLastErrorText());

    op = updateUserBalance(nome, op-1);


    op= getUserBalance(nome);
    fprintf(stderr, "\n Resultado de saldo %d , %s.\n", op,getLastErrorText());







    
    char* o= "utilizadores.txt";

    op= saveUsersFile(o);
    fprintf(stderr, "\n Resultado de saveUsers %d, %s.\n", op,getLastErrorText());














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