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

    }else if(strcmp(opcao,"sell")){
        strcpy(mensagem_client.palavra, "sell");
        printf("\nQuer por um item a venda?");
        printf("\nsell <nome-item> <categoria> <preco-base> <preco-compre-ja> <duracao>");
        scanf("%s", mensagem_client.sell);
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
                fprintf(stderr, "%d \n",mensagem_server.valor);
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

    /*char opcao;
    if (strcmp(opcao,"list")==0){
        prinf("comados :\nsaldo\nlistar\nitems\nlicitar\nlisel\nlitime");
    }else if(strcmp(opcao,"licitar")==0){
        prinf("comando : licitar!!");
    }else if(strcmp(opcao,"lisel")==0){
        prinf("comando : lisel");
    }else if(strcmp(opcao,"lival")==0){
        prinf("comando : lival");
    }else if(strcmp(opcao,"litime")==0){
        prinf("comando : litime");
    }else{printf("\n O servidor nao entregou mensagem.\n ");}}*/


































/*
// agora o fifo deve estar aberto e por isso vai poder mandar mgs para o server;

    do {
        //leitura de comandos para o servidor
        fgets(cmd, sizeof(cmd), stdin);
        strtok(cmd, "\n");
        for (int i = 0; i < strlen(cmd); i++){
            cmd[i] = toupper(cmd[i]);
        }

//IMPLEMENTAR VERIFICACAO DE ARGUMENTOS

        if (strcmp(cmd, "LOGIN") == 0) {
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
        =======


    }   else if (strcmp(cmd, "KICK") == 0) {

        >>>>>>> 2cb1f5f5a1c871ebf070ebd4ad168185a33b6583

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



if ((res = mkfifo(SERVER_FIFO, 0777)) < 0) {
perror("\nErro ao criar o FIFO do Servidor.\n");
shutdown();
if (mkfifo(nome_fifo, 0777) < 0) {
perror("\n Erro ao criar o FIFO do cliente.\n");
exit(EXIT_FAILURE);
}
fprintf(stderr, "\nFIFO servidor criado com sucesso!");
fprintf(stderr, "\n FIFO do cliente criado.\n");

if ((s_fifo = open(SERVER_FIFO, O_RDWR)) < 0) {
perror("\nErro ao abrir o FIFO do servidor(RDWR/BLOCKING).\n");
shutdown();
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
fprintf(stderr, "\nFIFO aberto para leitura.\n");
fprintf(stderr, "\n FIFO do Cliente aberto para leitura.\n");

// agora o fifo deve estar aberto e por isso vai poder mandar mgs para o server;

do {
//leitura de comandos para o servidor
@ -122,9 +131,8 @@ int main(int argc, char* argv[], char* envp[]) {
}

//IMPLEMENTAR VERIFICACAO DE ARGUMENTOS
if  (strcmp(cmd, "players") == 0) {

}   else if (strcmp(cmd, "LOGIN") == 0) {
if (strcmp(cmd, "LOGIN") == 0) {
char user[18], password[18];
int certo;
do{
@ -132,18 +140,51 @@ int main(int argc, char* argv[], char* envp[]) {
    scanf("%s", mensagem_server.user);
    printf("Escreva a sua palavra-passas do user %s:", user);
    scanf("%s", mensagem_server.palavra;
    int fd = open("SERVER_FIFO", O_RDWR);
    if (write(server_fifo, &mensagem_client, sizeof(mensagem_client)) == -1){
        if (write(server_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
            printf("erro no envio da msg");
            //escreve no fifo do servidor para ele ler usando a mensagem para servidor
        }
        close(fd);
    } while (certo);
//fim do login-----------------------------------------------------------------------------------------------
}   else if (strcmp(cmd, "LIST") == 0) { //lista items á venda
}   else if (strcmp(cmd, "REGISTAR") == 0) {
char user[18], pass[18], pass2[18];
int voltar;
do{
voltar = 0;
printf("\nEscreva o username que quer:");
scanf("%s", mensagem_client.user);
printf("Escreva a palavra-pass do usar: %s:", user);
scanf("%s", mensagem_client.palavran ;
printf("confirme a palavra-pass:");
gets(pass2);
if (strcmp(pass, pass2) == 0){
for (int i = 0; i < strlen(user); i++){
if (user[i] == ' '){
voltar = 1;
printf("o user tem que ser uma palavra apenas");
}
}else{voltar = 1;}
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
printf("-----------------gg----------------------------------------");
}while(voltar);
if (write(server_fifo, &mensagem_client, sizeof(mensagem_client)) == -1) {
printf("erro no envio da msg");
}

}   else if (strcmp(cmd, "KICK") == 0) { //remove utilizador
token = strtok(cmd, "k");

}   else if (strcmp(cmd, "PROM") == 0) { //lista utilizadores promotores atuais

}   else if (strcmp(cmd, "REPROM") == 0) { //atualiza promotores*/

