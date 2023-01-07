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
struct Item Items[31];
int items_disponiveis=0;
//Variáveis de Ambiente
int MAXUSERS = DEFAULT_MAXUSERS;
char* GAMEDIR = DEFAULT_LEILAODIR;
pthread_mutex_t mutex;
pthread_mutex_t mutex2;
int tempoLeilao, tempoEspera;

void shutdown() {
    printf("Exiting program...\n");
    close(s_fifo);
    unlink(SERVER_FIFO);
}

void sigHandler(int sig) { //servidor vai fechar
    if (sig == SIGINT) {
        for (int i = 0; i < connectedUsers; i++) {

            kill(userList[i].userPID, SIGUSR1);
        }
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

int GetTime() //obter tempo atual
{
    FILE *fp;
    int var=0;
    fp= fopen("time.txt","r+");
    if(fp!=NULL)
    {
        fscanf(fp,"%d",&var);
    }
    return var;
}

int itemdiv(){
    const int tam = 30;
    FILE *iteml;
    char linha[125], palavra[tam], id[tam], nomeitem[tam], categ[tam], valor_atual[tam], valor_compra[tam];
    char tempo_leilao[tam], username_vendedoravra[tam], username_comprador[tam];
    int g2 = 0, g3 = 0, numeroitem = 0;
    int g5 = 0;
    iteml = fopen("itemteste.txt","r");
    while (!feof(iteml)){
        fgets(linha, 125, iteml);
        g2 = 0;
        //printf("\nteste:: %s", linha);
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
                strcpy(id , palavra);
                //printf("\n id = %s", id);
                strcpy(palavra, " ");

            }
            if( j == 1){
                strcpy(nomeitem, palavra);
                //printf("\n nome do item = %s", nomeitem);
                strcpy(palavra, " ");

            }
            if( j == 2){
                strcpy(categ, palavra);
                //printf("\n categoria = %s", categ);
            }
            if( j == 3){
                strcpy(valor_atual, palavra);
                //printf("\n valor_atual = %s", valor_atual);
                strcpy(palavra, " ");

            }
            if( j == 4){
                strcpy(valor_compra, palavra);
                //printf("\n valor_compra = %s", valor_compra);
                strcpy(palavra, " ");

            }
            if( j == 5){
                strcpy(tempo_leilao, palavra);
                //printf("\n tempo_leilao = %s", tempo_leilao);
                strcpy(palavra, " ");

            }
            if( j == 6){
                strcpy(username_vendedoravra, palavra);
                //printf("\n username_vendedoravra = %s", username_vendedoravra);
                strcpy(palavra, " ");

            }
            if( j == 7){
                strcpy(username_comprador, palavra);
                //printf("\n username_comprador = %s", username_comprador);
                strcpy(palavra, " ");

            }
            strcpy(palavra, " ");
            g2++;
            g5++;
            g3 = 0;
        }
    }
    numeroitem -= 1;
    items_disponiveis=numeroitem;
    //printf("\n numeroitem:%i \n ", numeroitem);
    fclose(iteml);
    return numeroitem;
}

void Gera_Item()
{
    const int tam = 30;
    FILE *gyy;
    char linha[125], palavra[tam];
    int g2 = 0, g3 = 0, numeroitem = 0;
    int g5 = 0;
    gyy = fopen("itemteste.txt","r");
    while (!feof(gyy)){
        fgets(linha, 125, gyy);
        g2 = g3 =0;
        //printf("\nteste:: %s", linha);
        for(int j = 0; j < 8; j++){
            while (linha[g2] != ' ' && linha[g2] != '\0' && linha[g2] != '\n')
            {
                palavra[g3] = linha[g2];
                g3++;
                g2++;
            }
            palavra[g3] = '\0';
            if( j == 0){
               Items[numeroitem].id= atoi(palavra);
               //fprintf(stderr, "\nID: %d .\n",Items[numeroitem].id);
            }
            if( j == 1){
                strcpy(Items[numeroitem].nome, palavra);  
                //fprintf(stderr, "\nNome: %s .\n",Items[numeroitem].nome);

            }
            if( j == 2){
               strcpy(Items[numeroitem].categoria, palavra); 
            }
            if( j == 3){
                Items[numeroitem].valor_atual= atoi(palavra); 
            }
            if( j == 4){
                Items[numeroitem].valor_compra= atoi(palavra); 
            }
            if( j == 5){
                Items[numeroitem].tempo_leilao= atoi(palavra); 
            }
            if( j == 6){
               strcpy(Items[numeroitem].username_vendedor, palavra); 
            }
            if( j == 7){
                strcpy(Items[numeroitem].username_comprador, palavra); 
            }
            g2++;
            g3 = 0;
        }
    numeroitem++;
    }
    numeroitem -= 1;
    fclose(gyy);
    items_disponiveis=numeroitem;
}

void Atualiza_Items()
{
    pthread_mutex_lock(&mutex2);
    FILE *gui;
    const char* mode="w";
    gui = fopen("itemteste.txt", mode);
    for(int iii = 0; iii < items_disponiveis; iii++){
        fprintf(gui, "%i %s %s %i %i %i %s %s", Items[iii].id, Items[iii].nome, Items[iii].categoria, Items[iii].valor_atual, Items[iii].valor_compra, Items[iii].tempo_leilao, Items[iii].username_vendedor, Items[iii].username_comprador);
        fprintf(gui, "\n");
    }
    fclose(gui);

    pthread_mutex_unlock(&mutex2);
}

struct LigacaoServidor funcaoitems(struct LigacaoServidor itemsstr){

    FILE *istr;
    int j3 = 0;
    char linha[125];
    istr = fopen("itemteste.txt","r");
    while (!feof(istr)){
        fgets(linha, 125, istr);
        strcpy(itemsstr.Todoitem[j3], linha);
        j3++;
    }
    fclose(istr);
    return itemsstr;
}

void UserManager(int choice)//funções relacionadas com alteração de utilizadores
{
    pthread_mutex_lock(&mutex);
     if(choice==0)
    {
        //adiciona utilizador
    }
    else if(choice==1)
    {
        //remove utilizador
    }
    pthread_mutex_unlock(&mutex);
}

void* clientServerComm() {
    struct LigacaoServidor mensagemForClient;
    struct LigacaoCliente mensagemForServer;
    char nome_fifo_cliente[50];
    char auxMsg[TAM_MAX];
    int logged_in=0;
    
    //Leitura de users
    //FILE *p = fopen("utilizadores.txt","anymode");
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
                userList[i].status=GetTime();
            }
        }
        if(flag==0)
        {
            userList[connectedUsers]=mensagemForServer;
            userList[connectedUsers].status=GetTime();
            connectedUsers++;
            //fica a mensagem guardada como a primeira do utilizador
             fprintf(stderr, "\n Novo utilizador com PID %d conectado. Numero de utilizadores online: %d.\n", mensagemForServer.userPID,connectedUsers);
        }
        //HEARTBEAT
    
        //pipe para responder ao cliente
        sprintf(nome_fifo_cliente, CLIENT_FIFO, mensagemForServer.userPID);
        if ((c_fifo = open(nome_fifo_cliente, O_WRONLY)) < 0) {
            shutdown();
            exit(EXIT_FAILURE);
        }

            fprintf(stderr, "\nComando %s recebido do utilizador %d\n", mensagemForServer.palavra,mensagemForServer.userPID);
            char *aux =mensagemForServer.palavra;
            fprintf(stderr, "\n user: %s \n", mensagemForServer.user);
            if(strcmp(mensagemForServer.palavra, "login")==0)
            {
                int code=isUserValid(mensagemForServer.user, mensagemForServer.password);
                if(code==1)
                {
                    //user correcto
                    mensagemForClient.valor=1;
                    strcpy(mensagemForClient.palavra, "Login efetuado, bem vindo .\n"); 
                    fprintf(stderr, "\nUtilizador %s conectado.\n", mensagemForServer.user);                       
                    logged_in=1;
                }
                else if(code==0){
                    //login errado
                    mensagemForClient.valor=0;
                    strcpy(mensagemForClient.palavra, "Login incorreto.\n");
                }else{
                    //outro erro
                    strcpy(mensagemForClient.palavra, ("Erro encontrado:  %s.\n",getLastErrorText()));
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
                
            }else if(strcmp(mensagemForServer.palavra, "items")==0)
            {
                int tx = itemdiv();
                mensagemForClient = funcaoitems(mensagemForClient);
                strcpy(mensagemForClient.palavra, "Numero de items:  ");
                mensagemForClient.valor = tx;
                res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                if (res < 0) {
                        perror("\n Erro a escrever para o cliente.");
                    }
                
            }else if(strcmp(mensagemForServer.palavra, "buy")==0) //licitar
            {
                //encontrar o item desejado
                //fprintf(stderr, ("\n 1.\n"));
                int saldo=0;
                char *ptr= mensagemForServer.user;
                if((saldo=getUserBalance(ptr))!=-1)
                {
                     //fprintf(stderr, "\n 2.\n");
                    for(int i=0;i<items_disponiveis;i++)
                    {
                         //fprintf(stderr, "\n 3.\n");
                        if(saldo>mensagemForServer.bidding)
                        {
                            if(Items[i].id==mensagemForServer.id)
                            {
                                 if (Items[i].tempo_leilao==0)//ja foi vendido
                                {
                                    strcpy(mensagemForClient.palavra, "O Item pretendido já foi vendido.\n");
                                    break;
                                }
                                //printf(("id: %d buy:%d\n"), Items[i].id,mensagemForServer.id);
                                if (mensagemForServer.bidding>Items[i].valor_compra)
                                {
                                    //vai comprar o item
                                    Items[i].tempo_leilao=0;
                                    updateUserBalance(mensagemForServer.user, saldo-Items[i].valor_compra);
                                    strcpy(Items[i].username_comprador, mensagemForServer.user);
                                    char mns[]="Parabens comprou o item ";
                                    strcat(mns,Items[i].nome);
                                    strcat(mns,"\n");
                                    strcpy(mensagemForClient.palavra, mns);
                                    //ATUALIZA OS ITEMS
                                    Atualiza_Items();
                                    break;
                                }
                                else if(mensagemForServer.bidding>Items[i].valor_atual)
                                {
                                    updateUserBalance(mensagemForServer.user,saldo-mensagemForServer.bidding);
                                    Items[i].valor_atual= mensagemForServer.bidding;
                                    strcpy(Items[i].username_comprador, mensagemForServer.user);
                                    char mns[]="Licitou no item ";
                                    strcat(mns,Items[i].nome);
                                    strcat(mns,"\n");
                                    strcpy(mensagemForClient.palavra, mns);
                                    Atualiza_Items();
                                    break;
                                }else{
                                strcpy(mensagemForClient.palavra, ("Quantia não é suficiente para licitar.\n"));}
                                break;

                            }else{
                            strcpy(mensagemForClient.palavra, "Item pretendido não encontado.\n");}
                        
                        }
                        else{
                            strcpy(mensagemForClient.palavra, "Nao possui saldo para licitar essa quantia.\n");
                        }
                    
                    } //fprintf(stderr, "\n 4.\n");
                     op= saveUsersFile(path);
                    if(op==-1)
                    {
                        fprintf(stderr, "\n Erro a guardar %d , %s.\n", op,getLastErrorText());
                    }
                }
                else{
                   fprintf(stderr, "\n Erro a ler saldo %d , %s.\n", op,getLastErrorText());
                }
                res = write(c_fifo, &mensagemForClient, sizeof(mensagemForClient));
                if (res < 0) {
                        perror("\n Erro a escrever para o cliente.");
                    }
            }
            else if(strcmp(mensagemForServer.palavra, "registar")==0)
            {

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
    } while (strcmp(cmd, "exit"));
}

void *timer() //incrementa tempo
{
    FILE *fp;
    int var=0;
    fp= fopen("time.txt","r+");
    while(1)
    {
        sleep(1);
        if(fp != NULL)
        {
            fscanf(fp,"%d",&var);
            fclose(fp);
            fp= fopen("time.txt","w");
            fprintf(fp,"%d",var+1); //atualiza
            fclose(fp);
            fp= fopen("time.txt","r+");
        }
        else{fprintf(stderr, "\n time errror.\n");}
    }
    fclose(fp);
    
}


int GetTime() //obter tempo atual
{
    FILE *fp;
    int var=0;
    fp= fopen("time.txt","r+");
    if(fp!=NULL)
    {
        fscanf(fp,"%d",&var);
    }
    return var;
}

void *Gestao_leiloes()
{
  while(1)
  {
    Gera_Item();
    int tempo_atual= GetTime();
    for(int i=0;i<items_disponiveis;i++)
    {
        if((Items[i].tempo_leilao<=tempo_atual)&&(Items[i].tempo_leilao!=0)) //time is over
        {
            //vai vender item ao comprador mais elevado //se vendido mudar o tempo para 0
          
        }
    }
    //ATUALIZA ITEMS TXT
  }
}

int main(int argc, char* argv[], char* envp[]) {
    int opt;
    char cmd[250];
    char *token;
    Gera_Item();
    Atualiza_Items();
    struct LigacaoServidor mensagemForCliente;
    char nome_fifo[50];
    struct LigacaoServidor mensagem_server;//pergunta
    struct LigacaoCliente mensagem_client;//resposta
    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_init(&mutex2,NULL);
    //criar um filho que roda em background e que está a escrever todas as promoções em uma ficheiro
    //FILE *p = fopen("utilizadores.txt","anymode");
    char*path= "utilizadores.txt";
    int op=0;
    op=loadUsersFile(path);
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
    pthread_t leilao;
    //pthread_create(&leilao, NULL, Gestao_leiloes, NULL);
    pthread_t tempo;
    pthread_create(&tempo, NULL, timer, NULL);
    Com_Servidor();

    pthread_cancel(commThread);
    pthread_cancel(leilao);
    pthread_cancel(tempo);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);




    return 0;

}