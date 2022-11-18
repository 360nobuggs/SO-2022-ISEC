//
// Created by Vasco on 10/31/2022.
//

#include "frontend.h"
#include <wait.h>
#include <unistd.h>
/* -- CRIAÇÃO DO FIFO SERVIDOR -- */

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

}   else if (strcmp(cmd, "REPROM") == 0) { //atualiza promotores
