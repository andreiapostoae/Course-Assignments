#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#define BUFLEN 256

void error(char *msg)
{
    perror(msg);
    exit(0);
}

char* convertPoint(long int x) {
    /* functia converteste un long int (reprezentand file size) intr-un string care
     * are formatul dorit in enunt: 123.456.789 */
    char *s = calloc(10, sizeof(char));
    sprintf(s, "%li", x);
    int i = 0;
    char *result = calloc(10, sizeof(char));

    int mod = strlen(s) % 3;
    int index_result = 0;
    int k = 0;
    for(i = 0; i < strlen(s); i++) {
        if(index_result % 3 == (mod + k) % 3) {
            result[index_result++] = '.';
            k++;
        }

        result[index_result++] = s[i];
    }
    free(s);
    if(result[0] == '.')
        return (result + 1);
    return result;
}


int main(int argc, char *argv[]) {
    /* fiecare client isi face propriul fisier de log */
    char *nume_log = calloc(20, sizeof(char));
    int pid = getpid();
    char aux[10];
    sprintf(aux, "%d", pid);
    strcpy(nume_log, "client-");
    strcat(nume_log, aux);
    strcat(nume_log, ".log");
    FILE *f = fopen(nume_log, "wt");

    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFLEN];
    if (argc < 3) {
       fprintf(stderr,"Usage %s server_address server_port\n", argv[0]);
       exit(0);
    }  
    
    /* parte de socketi nemodificata din laboratorul de TCP cu multiplexare */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &serv_addr.sin_addr);
    
    
    if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");    

    fd_set set, tmp;
    FD_ZERO(&set);
    FD_ZERO(&tmp);
    FD_SET(sockfd, &set);
    FD_SET(0, &set);
    int fdmax = sockfd;

    /* last_command retine ultima comanda valida */
    char *last_command = calloc(100, sizeof(char));

    /* current_username retine utilizatorul curent conectat */
    char *current_username = calloc(24, sizeof(char));
    int logged_in = 0;
    int current_chunk = 0;

    while(1){

    	tmp = set;
    	if (select(fdmax + 1, &tmp, NULL, NULL, NULL) == -1) 
			error("ERROR in select");
		
    	if(FD_ISSET(0,&tmp)) {
    		memset(buffer, 0 , BUFLEN);
    		fgets(buffer, BUFLEN-1, stdin);

            /* daca nu sunt logat, se afiseaza in fisier cu $ la inceput */
            if(logged_in == 0 && strncmp(buffer, "quit", 4) != 0)
                fprintf(f, "$ %s", buffer);
            else 
                /* daca sunt logat, se afiseaza in fisier cu username> la inceput */
                if(strncmp(buffer, "quit", 4) != 0)
                    fprintf(f, "%s> %s", current_username, buffer);


            if(strncmp (buffer, "quit", 4) == 0) {
                printf("Terminating.\n");
                break;
            }

            /* daca sunt deja logat si incerc login, afisez eroarea si nu trimit request 
               de login catre server */
            if(logged_in == 1 && strncmp(buffer, "login", 5) == 0) { 
                fprintf(f, "-2 Sesiune deja deschisa\n\n");
                printf("You are already logged in.\n\n");
            } else 
                /* daca nu sunt logat si incerc alte comenzi in afara de login, 
                   nu le trimit la server si afisez eroarea in fisier */
                if(logged_in == 0 && (strncmp(buffer, "logout", 6) == 0 || 
                                      strncmp(buffer, "getuserlist", 11) == 0 || 
                                      strncmp(buffer, "getfilelist", 11) == 0 || 
                                      strncmp(buffer, "share", 5) == 0 ||
                                      strncmp(buffer, "unshare", 7) == 0 ||
                                      strncmp(buffer, "delete", 6) == 0)) { 
                    fprintf(f, "-1 Clientul nu este autentificat\n\n");
                    printf("You are not logged in.\n\n");
                } else {
                    /* daca totul e in regula, trimit comanda */
                    int n = send(sockfd,buffer, 250, 0);
                    strcpy(last_command, buffer);     
                }
    	}
    	if(FD_ISSET(sockfd,&tmp)) {
    		memset(buffer, 0, BUFLEN);
    		int k = recv(sockfd, buffer, 250, 0);
    		printf("Received from server: %s\n\n",buffer);

            /* tratare mesaje de la server cu afisare in fisier */
            if(strncmp(buffer, "quit", 4) == 0) {
                printf("Terminating.\n");
                break;
            }
            if(strncmp(buffer, "-3", 2) == 0) 
                fprintf(f, "-3 User/parola gresita\n\n");

            if(strncmp(buffer, "-8", 2) == 0) {
                fprintf(f, "-8 Brute force detectat\n\n");
                break;
            }

            if(strncmp(buffer, "-6", 2) == 0) {
                fprintf(f, "-6 Fisierul este deja partajat\n\n");
            }

            if(strncmp(buffer, "-4", 3) == 0) {
                fprintf(f, "-4 Fisier inexistent\n\n");
            }

            if(strncmp(buffer, "-11", 3) == 0) {
                fprintf(f, "-11 Utilizator inexistent\n\n");
            }

            if(strncmp(buffer, "-7", 2) == 0) {
                fprintf(f, "-7 Fisier deja privat\n\n");
            }

            if(strncmp(buffer, "-10", 3) == 0) {
                fprintf(f, "-10 File is busy\n\n");
            }

            if(strncmp(buffer, "-9", 2) == 0) {
                fprintf(f, "-9 Fisier existent pe server\n\n");
            }


            if(strncmp(buffer, "0", 1) == 0) {
                /* tratez simplu login, logout, share,unshare */
                if (strncmp(last_command, "login", 5) == 0) {
                    char* aux = calloc(24, sizeof(char));
                    sscanf(last_command, "login %s %s", current_username, aux);
                    fprintf(f, "\n");
                    logged_in = 1;
                }

                if(strncmp(last_command, "logout", 5) == 0) {
                    char* aux = calloc(24, sizeof(char));
                    logged_in = 0;
                    fprintf(f, "\n");
                }

                if(strncmp(last_command, "share", 5) == 0) {
                    char* file_name = calloc(50, sizeof(char));
                    char *token;
                    const char separator[2] = " ";
                                            
                    token = strtok(last_command, separator);
                                            
                    while(token != NULL) {
                        token = strtok(NULL, separator);
                        if(token != NULL) {
                            strcat(file_name, token);
                            strcat(file_name, " ");
                        }
                    }

                    file_name[strlen(file_name) - 2] = '\0';
                    fprintf(f, "200 Fisierul %s a fost partajat\n\n", file_name);
                }

                if(strncmp(last_command, "unshare", 5) == 0) {
                    char *shared_file = calloc(24, sizeof(char));
                    sscanf(last_command, "unshare %s\n", shared_file);
                    fprintf(f, "200 Fisierul a fost setat ca PRIVATE\n\n");
                }
            }

            /* getuserlist si getfilelist functioneaza dupa urmatorul mecanism: 
             * primesc segmente de date pe care le afiseaza, si stiu cand sa se opreasca
             * deoarece primul segment este mereu dimensiunea */
            if(strncmp(last_command, "getuserlist", 11) == 0) {
                if(current_chunk == 0) {
                    sscanf(buffer, "%d", &current_chunk);
                    if(current_chunk == 0)
                        fprintf(f, "\n");
                }
                else {
                    fprintf(f, "%s\n", buffer);
                    current_chunk--;
                    if(current_chunk == 0)
                        fprintf(f, "\n");
                }
            }

            if(strncmp(last_command, "getfilelist", 11) == 0 && strncmp(buffer, "-11", 3) != 0) {
                if(current_chunk == 0) {
                    sscanf(buffer, "%d", &current_chunk);
                    if(current_chunk == 0)
                        fprintf(f, "\n");
                } else {
                    fprintf(f, "%s\n", buffer);
                    current_chunk--;
                    if(current_chunk == 0)
                        fprintf(f, "\n");
                }
            }

            /* tratez simplu delete */
            if(strncmp(last_command, "delete", 6) == 0 && strncmp(buffer, "-4", 2) != 0) {
                fprintf(f, "200 Fisier sters\n\n");
            }
    	}



    }

    fclose(f);
    return 0;
}


