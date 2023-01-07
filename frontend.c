#include "frontend.h"
#include <wait.h>
#include <unistd.h>

// Variáveis globais
int s_fifo, c_fifo, res, rs2;
int connectedUsers = 0;
int LeilaoStarted = 0;
int LeilaoFinished = 0;
int alarmPID;
int clientfifo;
int serverfifo;
char nome_fifo[50];

void handler(int sig,siginfo_t* si, void*unctext)
{
    close(clientfifo);
    close(serverfifo);
    unlink(nome_fifo);
    exit(1);
}
void Shutdown(int sig)
{
    fprintf(stderr,"\n O servidor desconectou.\n");
}
void shutdown() {
    printf("Exiting program...\n");
    close(s_fifo);
    unlink(SERVER_FIFO);
}

int main(int argc, char* argv[], char* envp[]) {

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction= handler;
    sa.sa_flags= SA_SIGINFO;
    sigaction(SIGINT,&sa,NULL);
    signal(SIGUSR1,Shutdown);






    int logged_in=0;
    char cmd[50];
    struct LigacaoServidor mensagem_server;//pergunta
    struct LigacaoCliente mensagem_client;//resposta
    int read_res;
    /* -- CRIAÇÃO DO FIFO SERVIDOR -- */
    mensagem_client.userPID= getpid();
    sprintf(nome_fifo,CLIENT_FIFO,mensagem_client.userPID);
    ;//abre o FIFO do servidor para escrita
    if(mkfifo(nome_fifo, 0777)<0)
    {
        perror("\n Erro a criar FIFO cliente.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\n FIFO do cliente criado.\n");

    if ((s_fifo = open(SERVER_FIFO, O_WRONLY)) < 0) {
        fprintf(stderr, "\n O cliente não consegue ligar-se ao servidor.\n");
        unlink(nome_fifo);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\n FIFO do servidor aberto para escrita.\n");

    if ((c_fifo = open(nome_fifo, O_RDWR)) < 0) { //o fifo do cliente so le
        perror("\n Erro ao abrir o FIFO do cliente.\n");
        close(s_fifo);
        unlink(nome_fifo);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\n FIFO do Cliente aberto para leitura.\n");
    
    mensagem_client.status=0;
    strcmp(mensagem_client.palavra,"login");
    //PRIMEIRA MENSAGEM SO PARA ESTABLECER LIGACAO COM SERVER
    /*if (write(s_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                    printf("erro no envio da msg");
                }else{
    
    fprintf(stderr, "\nMensagem enviada para servidor.\n");
  
    read_res=read(c_fifo,&mensagem_server,sizeof(mensagem_server));
    if(read_res==sizeof(mensagem_server))
    {
        fprintf(stderr,"\n mensagem do servidor recebida\n ");
        fprintf(stderr, mensagem_server.palavra);
    }
    else
    {
        fprintf(stderr,"\n O servidor nao entregou mensagem.\n ");
    }
    
    */
    //LOGIN
    do {
        printf("faca login com o comando <login> ou registo com o comando <registar>");
        gets(cmd);
        for (int i = 0; i < strlen(cmd); i++){
            cmd[i] = toupper(cmd[i]);
        }
    // verifica sintaxe;
        if (strcmp(cmd, "LOGIN") == 0) {
            do{
                strcpy(mensagem_client.palavra, "login");
                printf("\nEscreva o seu username:");
                scanf("%s", mensagem_client.user);
                printf("Escreva a sua palavra-passas do user %s :", mensagem_client.user);
                scanf("%s", mensagem_client.password);
                //envia login
                if (write(s_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                    printf("erro no envio da msg\n");
                }else{
                    fprintf(stderr, "\nMensagem enviada para servidor.\n");
                    read_res=read(c_fifo,&mensagem_server,sizeof(mensagem_server));
                    if(read_res==sizeof(mensagem_server))
                    {
                        fprintf(stderr,"\n mensagem do servidor recebida\n ");
                        fprintf(stderr, mensagem_server.palavra);
                        if(mensagem_server.valor==1)
                        {
                            logged_in=1;
                            
                        }
                    }
                    else
                    {
                        fprintf(stderr,"\n O servidor nao entregou mensagem.\n ");
                    }
               }
            } while (logged_in==0);

        //fim do login-----------------------------------------------------------------------------------------------
        }   else if (strcmp(cmd, "REGISTAR") == 0) {
            strcpy(mensagem_client.palavra, "registar");
            char pass2[50];
            int voltar;
            
            do{
                voltar = 0;
                printf("\nEscreva o username que quer:");
                scanf("%s", mensagem_client.user);
                printf("Escreva a palavra-pass do usar: %s:", mensagem_client.user);
                scanf("%s", mensagem_client.password) ;
                printf("confirme a palavra-pass:");
                gets(pass2);

                if (strcmp(mensagem_client.palavra, pass2) == 0){
                    for (int i = 0; i < strlen(mensagem_client.user); i++){
                        if (mensagem_client.user[i] == ' '){
                            voltar = 1;
                            printf("o user tem que ser uma palavra apenas");
                        }
                    }
// em cima verifica se a mesmo palavra-passe que ele que se não for repete o processo de pedir as info se for;
// verifica se e a apenas uma palavra no user;

                    for (int i = 0; i < strlen(pass2); i++){
                        if (pass2[i] == ' '){
                            voltar = 1;
                            printf("o user tem que ser uma palavra apenas");
                        }
                    }
// verifica se e apenas uma palavra na palavra-pass
                }else{
                    voltar = 1;
                    printf("Enganou se na palavra-pass");
                }
            }while(voltar);
            if ((write(s_fifo, &mensagem_client, sizeof(mensagem_client))) == -1) {
                printf("erro no envio da msg");
            }
//manda msg para i servidor com os nome do novo user e palavra-pass;
        }else{ printf("esse comado nao existe\n");}
    } while (logged_in==0);

//logged in comandos para o servidor
//recebe lista de comandos

do{

    fprintf(stderr,"Lista de comandos:\n ->saldo :Ver saldo na conta.\n ->listar :Lista numero utilizadores.\n ->items :Numero de items disponiveis.\n ->promo :Lança promotor oficial.\n ->exit :Termina programa\n");
    fprintf(stderr,"\n Indroduza o comando pretendido:");
    char opcao[10];
    int flag=1;
    scanf("%s",opcao);
    printf("\n palavra reconhecida:%s\n",opcao);
    if(strcmp(opcao, "saldo") == 0) 
    {
         printf("\nsaldo\n");
        strcpy(mensagem_client.palavra, "saldo");
        if (write(s_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                printf("erro no envio da msg\n");
            }
    }
    else if(strcmp(opcao, "buy") == 0) 
    {
        strcpy(mensagem_client.palavra, "buy");
        printf("\nEscreva o ID do item que pertende licitar:");
        scanf("%d", &mensagem_client.id);
        printf("\nEscreva a quantia que pretende usar:");
        scanf("%d", &mensagem_client.bidding) ;
        if (write(s_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                printf("erro no envio da msg\n");
            }
    }
    else if(strcmp(opcao,"listar")==0)
    {
        strcpy(mensagem_client.palavra,"listar");
        printf("\n comando  %s \n",mensagem_client.palavra);
        if (write(s_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                printf("erro no envio da msg\n");
            }
    }
    else if(strcmp(opcao,"items")==0)
    {
        strcpy(mensagem_client.palavra,"items");
        printf("\n comando  %s \n",mensagem_client.palavra);
        if (write(s_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                printf("erro no envio da msg\n");
            }
    }
    else if(strcmp(opcao,"promo")==0)
    {
        strcpy(mensagem_client.palavra, "promo");
        printf("\n comando  %s \n",mensagem_client.palavra);
        if (write(s_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                printf("erro no envio da msg\n");
            }
    }
    else if(strcmp(opcao,"exit")==0) //trigger saida
    {
        strcpy(mensagem_client.palavra, "exit");
        if (write(s_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                printf("erro no envio da msg\n");
            }
        shutdown();
        exit(EXIT_SUCCESS);

    }else if(strcmp(opcao,"sell")== 0){
        strcpy(mensagem_client.palavra, "sell");
        printf("\nQuer por um item a venda?");
        printf("\n<nome-item> <categoria> <preco-base> <preco-compre-ja> <duracao>\n");
        scanf("%s %s %i %i %i", &mensagem_client.nomeitem, &mensagem_client.catgitem, &mensagem_client.pbi[0], &mensagem_client.pbi[1], &mensagem_client.pbi[2]);
        
        if (write(s_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
                printf("erro no envio da msg\n");
        }
    }else{
         printf("comando não reconhecido.\n");
         flag=0;
    }
    if(flag==1)
    {
    read_res=read(c_fifo,&mensagem_server,sizeof(mensagem_server));
    if(read_res==sizeof(mensagem_server))
        {
            fprintf(stderr,"\n mensagem do servidor recebida\n ");
            fprintf(stderr, mensagem_server.palavra);
            if(strcmp(opcao,"saldo")==0||strcmp(opcao,"listar")==0)
            {
                fprintf(stderr, "%d \n",mensagem_server.valor);
            }else if (strcmp(opcao,"items")==0){
                fprintf(stderr, "%d",mensagem_server.valor);
                int numero = 1;
                numero = mensagem_server.valor;
                for (int gre = 0; gre < numero; gre++){
                    fprintf(stderr, "%s \n",mensagem_server.Todoitem[gre]);
                }
            }
        }
        else
        {
            fprintf(stderr,"\n O servidor nao entregou mensagem.\n ");
        }
    }

    
}while(1);}