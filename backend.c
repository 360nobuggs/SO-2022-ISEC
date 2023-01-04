//
// Created by Vasco on 10/31/2022.
//

#include "backend.h"

// Variáveis globais
int s_fifo, c_fifo, res;
int connectedUsers = 0;
int LeilaoStarted = 0;
int LeilaoFinished = 0;
struct LigacaoCliente userList[20];
//Variáveis de Ambiente
int MAXUSERS = DEFAULT_MAXUSERS;
char* GAMEDIR = DEFAULT_LEILAODIR;

int tempoLeilao, tempoEspera;

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
        //Tem de mandar um sigUSR1 a todos os clientes e todos os leioloes para notificar que o sevidor vai fechar.
        for (int i = 0; i < connectedUsers; i++) {

            kill(userList[i].userPID, SIGUSR1);
            //kill(userList[i].PID, SIGUSR1); matar processo de leilão
        }
    }
}

int itemdiv(){
    const int tam = 30;
    FILE *iteml;
    char linha[125], palavra[tam], id[tam], nomeitem[tam], categ[tam], valor_atual[tam], valor_compra[tam];
    char tempo_leilao[tam], username_vendedoravra[tam], username_comprador[tam];
    int g2 = 0, g3 = 0, numeroitem = 0;
    iteml = fopen("itemteste.txt","r");
    while (!feof(iteml)){
        fgets(linha, 125, iteml);
        g2 = 0;
        printf("\nteste:: %s", linha);
        numeroitem++;
        for(int j = 0; j < 8; j++){
            while (linha[g2] != ' ' && linha[g2] != '\0' && linha[g2] != '\n')
            {
                palavra[g3] = linha[g2];
                g3++;
                g2++;
            }
            palavra[g3] = '\0';
            if( j == 0){
                strcpy(id, palavra);
                printf("\n id = %s", id);
                strcpy(palavra, " ");

            }
            if( j == 1){
                strcpy(nomeitem, palavra);
                printf("\n nome do item = %s", nomeitem);
                strcpy(palavra, " ");

            }
            if( j == 2){
                strcpy(categ, palavra);
                printf("\n categoria = %s", categ);
            }
            if( j == 3){
                strcpy(valor_atual, palavra);
                printf("\n valor_atual = %s", valor_atual);
                strcpy(palavra, " ");

            }
            if( j == 4){
                strcpy(valor_compra, palavra);
                printf("\n valor_compra = %s", valor_compra);
                strcpy(palavra, " ");

            }
            if( j == 5){
                strcpy(tempo_leilao, palavra);
                printf("\n tempo_leilao = %s", tempo_leilao);
                strcpy(palavra, " ");

            }
            if( j == 6){
                strcpy(username_vendedoravra, palavra);
                printf("\n username_vendedoravra = %s", username_vendedoravra);
                strcpy(palavra, " ");

            }
            if( j == 7){
                strcpy(username_comprador, palavra);
                printf("\n username_comprador = %s", username_comprador);
                strcpy(palavra, " ");

            }
            g2++;
            g3 = 0;
        }
    }
    numeroitem -= 1;
    printf("\n numeroitem:%i \n ", numeroitem);
}



void* clientServerComm() {
    struct LigacaoServidor mensagemForClient;
    struct LigacaoCliente mensagemForServer;
    char nome_fifo_cliente[50];
    char auxMsg[TAM_MAX];
    int logged_in=0;

    //Leitura de users
    char*path= "utilizadores.txt";
    int op=0;
    int saldo=0;
    //Mensagem establece comunicação
    /*
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
    */
    //Login ou Registo
     do {
        res = read(s_fifo, &mensagemForServer, sizeof(mensagemForServer));
        int flag=0;
        if (res < 0) {
            perror("\n Erro a ler do cliente.");
        }
        else
        {
        //verifica se conhece o recipiente
        for(int i=0;i<connectedUsers;i++)
        {
            if(userList[i].userPID==mensagemForServer.userPID)
            {
                flag=1;
            }
        }
        if(flag==0)
        {
            userList[connectedUsers]=mensagemForServer;
            connectedUsers++;
            //fica a mensagem guardada como a primeira do utilizador
        }
            fprintf(stderr, "\nComando %s recebido do utilizador %d\n", mensagemForServer.palavra,mensagemForServer.userPID);
            char *aux =mensagemForServer.palavra;
            if(strcmp(mensagemForServer.palavra, "login")==0)
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
            }else if(strcmp(mensagemForServer.palavra, "registar")==0)
            {

            }
            else if(strcmp(mensagemForServer.palavra, "saldo")==0)
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
                   fprintf(stderr, "\n Erro a ler saldo %d , %s.\n", op,getLastErrorText());
                }
                fprintf(stderr, "\n palavra: %s , saldo :%d .\n", mensagemForClient.palavra,mensagemForClient.valor);
                res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                if (res < 0) {
                        perror("\n Erro a escrever para o cliente.");
                    }

            }else if(strcmp(mensagemForServer.palavra, "listar")==0){
                //listar utilizadores
                char *ptr= mensagemForServer.user;
                if((saldo=getUserBalance(ptr))!=-1)
                {
                    op=loadUsersFile(path);
                    if(op>-1)
                    {
                    strcpy(mensagemForClient.palavra, "O numero de utilizadores é: ");
                    mensagemForClient.valor=op;
                    fprintf(stderr, "\n Resultado de loadUsersFile %d , %s.\n", op,getLastErrorText());
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
                        fprintf(stderr, "\n Erro a ler %d , %s.\n", op,getLastErrorText());
                    }
                    res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                    if (res < 0) {
                            perror("\n Erro a escrever para o cliente.");
                        }
                    }
                else{
                    fprintf(stderr, "\n Erro a ler saldo %d , %s.\n", op,getLastErrorText());
                }
                
            }else if(strcmp(aux, "items")==0)
            {
                int tx = itemdiv();
                strcpy(mensagemForClient.palavra, "Numero de items:  ");
                mensagemForClient.valor=tx;
                res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                if (res < 0) {
                        perror("\n Erro a escrever para o cliente.");
                    }
                
            }
            else if(strcmp(aux, "promo")==0)
            {
                //promotor
                printf("teste promotor");
                strcpy(mensagemForClient.palavra, "Promotor oficial iniciado.\n\n");
                    res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                    if (res < 0) {
                        perror("\n Erro a escrever para o cliente.");
                    }
                int id = fork();
                if(id == 0){
                    int file, file2;
                    int pidfilho = getpid();
                    printf("%i", pidfilho);
                    file = open("prome.txt", O_WRONLY | O_CREAT, 0777);
                    if(file == -1){
                        printf("erro na cricao da pasta");
                       
                    }
                    printf("\nescrever as prome para o arquivo: prome.txt");
                    file2 = dup2(file, 1);
                    close(file);
                    int error231 = execl("promotor_oficial", "promotor_oficial", NULL);
                    if(error231 == -1){
                        printf("codigo nao enconntador para execl correr");
                       
                    }
                
                    }
            }else if(strcmp(mensagemForServer.palavra,"exit")==0)
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

void Com_Servidor()
{
    char cmd[250];
    do {
        fgets(cmd, sizeof(cmd), stdin);
        strtok(cmd, "\n");
        for (int i = 0; i < strlen(cmd); i++) {
            cmd[i] = toupper(cmd[i]);
        }
        //IMPLEMENTAR VERIFICACAO DE ARGUMENTOS

        if (strcmp(cmd, "USERS") == 0) {


        } else if (strcmp(cmd, "LIST") == 0) {

        } else if (strcmp(cmd, "KICK") == 0) {

        }else if (strcmp(cmd, "PROM") == 0) { //lista utilizadores promotores atuais

        } else if (strcmp(cmd, "REPROM") == 0) { //atualiza promotores

        } else if (strcmp(cmd, "CANCEL") == 0) { //cancela promotor

        } else if (strcmp(cmd, "CLOSE") == 0) { //termina execucao

        } else {
            printf("\nComando nao detetado!\n");
        }
    } while (!strcmp(cmd, "exit"));
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


    signal(SIGINT, sigHandler);
    signal(SIGALRM, alarmHandler); //verificar implementação

    //VERIFICAR FUNCIONAMENTO DE VARIAVEIS DE AMBIENTE





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

    pthread_t commThread; //thread de comunicação
    pthread_create(&commThread, NULL, clientServerComm, NULL);
    Com_Servidor();





    return 0;

}