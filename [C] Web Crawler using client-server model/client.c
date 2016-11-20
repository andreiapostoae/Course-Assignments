#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFLEN 20000

char** add_link(char** file_list, char* file, int *max_size) {
    /* functie de adaugare unui string intr-o lista de stringuri */
    if(file_list == NULL) {
        file_list = calloc(1, sizeof(char *));
        *max_size = 1;
        file_list[0] = calloc(100, sizeof(char));
        strcpy(file_list[0], file);
        return file_list;
    }

    int i = *max_size;
        
    file_list = realloc(file_list, ((*max_size) + 1) * sizeof(char *));
    file_list[i] = calloc(100, sizeof(char));
    strcpy(file_list[i], file);
    *max_size = *max_size + 1;
    return file_list;
}



int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int portno;


    int port_exists = 0;
    int address_exists = 0;
    int log_file_exists = 0;
    char* log_file_name = calloc(20, sizeof(char));
    char* ip_address = calloc(20, sizeof(char));

    int i;
    char buffer[BUFLEN];
    if (argc < 5 || argc > 7) {
       fprintf(stderr,"%s [-o <fisier_log>] -a <adresa ip server> -p <port>\n", argv[0]);
       exit(0);
    }  

     for(i = 0; i < argc - 1; i++) {
        if(strcmp(argv[i], "-p") == 0) {
            port_exists = 1;
            portno = atoi(argv[i + 1]);
        }

        if(strcmp(argv[i], "-a") == 0) {
            address_exists = 1;
            strcpy(ip_address, argv[i + 1]);
        }

        if(strcmp(argv[i], "-o") == 0) {
            log_file_exists = 1;
            strcpy(log_file_name, argv[i + 1]);
        }
     }


     if(port_exists == 0 || address_exists == 0) {
        fprintf(stderr,"%s [-o <fisier_log>] -a <adresa ip server> -p <port>\n", argv[0]);
        exit(1);
     }

      /* creare fisiere .stdout si .stderr folosindu-ma de PID */
    int pid_int = getpid();
    char pid[10];
    sprintf(pid, "%d", pid_int);
    strcat(log_file_name, "_");
    strcat(log_file_name, pid);

    char *log_file_stdout = calloc(30, sizeof(char));
    strcpy(log_file_stdout, log_file_name);
    strcat(log_file_stdout, ".stdout");

    char *log_file_stderr = calloc(30, sizeof(char));
    strcpy(log_file_stderr, log_file_name);
    strcat(log_file_stderr, ".stderr");

    FILE *log_stdout;
    FILE *log_stderr;

    if(log_file_exists == 1) {
        log_stdout = fopen(log_file_stdout, "wt");
        log_stderr = fopen(log_file_stderr, "wt");
    } 



    
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        if(log_file_exists == 1) {
            fprintf(log_stderr, "ERROR opening socket\n");
            fclose(log_stderr);
            fclose(log_stdout);
            exit(0);
        }
        else
            fprintf(stdout, "ERROR opening socket\n");
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    inet_aton(ip_address, &serv_addr.sin_addr);
    
    
    if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0) {
        if(log_file_exists == 1) {
            fprintf(log_stderr, "ERROR connecting to local server\n");
            fclose(log_stderr);
            fclose(log_stdout);
            exit(0);
        }
        else
            fprintf(stdout, "ERROR connecting to local server\n");
    }

    fd_set set, tmp;
    FD_ZERO(&set);
    FD_ZERO(&tmp);
    FD_SET(sockfd, &set);
    FD_SET(0, &set);
    int fdmax = sockfd;

    while(1){
    	tmp = set;
    	if (select(fdmax + 1, &tmp, NULL, NULL, NULL) == -1) 
			if(log_file_exists == 1) {
                fprintf(log_stderr, "ERROR in select\n");
                fclose(log_stderr);
                fclose(log_stdout);
                exit(0);
            }
            else
                fprintf(stdout, "ERROR in select\n");
    		
    	if(FD_ISSET(0,&tmp)) {
    		memset(buffer, 0 , BUFLEN);
    		fgets(buffer, BUFLEN-1, stdin);
    		int n = send(sockfd,buffer, 250, 0);
    	}
    	if(FD_ISSET(sockfd,&tmp)) {
    		memset(buffer, 0, BUFLEN);
    		int k = recv(sockfd, buffer, 250, 0);

            if(strncmp(buffer, "quit", 4) == 0) {
                printf("Terminating.\n");
                break;
            }

            if(log_file_exists == 1)
                fprintf(log_stdout, "Am primit %s\n",buffer);
            else
                fprintf(stdout, "Am primit %s\n",buffer);

            int sock_download, n_download;
            struct sockaddr_in serv_addr_download = {0};

            /* separ hostnameul de restul adresei pentru a putea realiza apelurile gethostbyname si GET */
            char *hostname = calloc(30, sizeof(char));
            char *address = calloc(50, sizeof(char));
            char *token;
            const char separator[2] = "/";
                                    
            token = strtok(buffer, separator);
            int count_slash = 0;
                                    
            while(token != NULL) {
                if(count_slash == 1)
                    strcpy(hostname, token);
                else
                    if(count_slash != 0) {
                        strcat(address, "/");
                        strcat(address, token);
                    }
                
                token = strtok(NULL, separator);
                count_slash++;
            }

            if(log_file_exists == 1) {
                fprintf(log_stdout, "Hostnameul este: %s\n", hostname);
                fprintf(log_stdout, "Pathul este: %s\n", address);
            } else {
                printf("Hostnameul este: %s\n", hostname);
                printf("Pathul este: %s\n", address);
            }
            struct hostent *server_download = gethostbyname(hostname);

            char buffer_download[BUFLEN];
         
            
            /* deschid socket nou pentru download, pe port 80 */
            sock_download = socket(AF_INET, SOCK_STREAM, 0);
            if (sock_download < 0) 
                if(log_file_exists == 1) {
                    fprintf(log_stderr, "ERROR opening socket\n");
                    fclose(log_stderr);
                    fclose(log_stdout);
                    exit(0);
                }
                else
                    fprintf(stdout, "ERROR opening socket\n");
            

            serv_addr_download.sin_family = AF_INET;
            serv_addr_download.sin_port = htons(80);
            memcpy(&serv_addr_download.sin_addr.s_addr, server_download->h_addr, server_download->h_length);
            

            if (connect(sock_download,(struct sockaddr*) &serv_addr_download,sizeof(serv_addr_download)) < 0) 
                if(log_file_exists == 1) {
                    fprintf(log_stderr, "ERROR connecting to http server\n");
                    fclose(log_stderr);
                    fclose(log_stdout);
                    exit(0);
                }
                else
                    fprintf(stdout, "ERROR connecting to http server\n"); 
            

            char* mesaj = calloc(100, sizeof(char));
            strcat(mesaj, "GET ");
            strcat(mesaj, address);
            strcat(mesaj, " HTTP/1.0\r\n\r\n");


            char *continut_html = calloc(200000, sizeof(char));
            memcpy(buffer_download, mesaj, strlen(address) + 17);

            n = send(sock_download, buffer_download, BUFLEN, 0);

            char *continut = calloc(200000, sizeof(char));
            strcat(continut, "path: ");
            strcat(continut, "/");
            strcat(continut, hostname);
 
            strcat(continut, address);
            strcat(continut, "\n");

           
            /* daca linkul trimis este fisier, se descarca, trimiterea catre server nefiind implementata */
            if(strstr(address, "htm") == NULL && strstr(address, ".") != NULL) {
                char separator2[2] = "/";
                token = strtok(address, separator2);
                char* file_name = calloc(100, sizeof(char));      
                while(token != NULL) {
                   strcpy(file_name, token);
                   token = strtok(NULL, separator2);
                } 
               

                FILE *downloaded_file = fopen(file_name, "wb+");
                int len;
                while((len = recv(sock_download, buffer_download, BUFLEN, 0)) > 0) {
                    fwrite(&buffer_download, len, 1, downloaded_file);
                    memset(buffer_download, 0, BUFLEN);
                }
                fclose(downloaded_file);

                free(continut);
                free(continut_html);
                continue;
            } 

           

            while(recv(sock_download, buffer_download, BUFLEN, 0) != 0) {
                strcat(continut_html, buffer_download);
                memset(buffer_download, 0, BUFLEN);
            }

            if(log_file_exists == 1)
                fprintf(log_stdout, "Am terminat de descarcat pagina.\n\n");
            else
                fprintf(stdout, "Am terminat de descarcat pagina.\n\n");



            char *continut_de_parsat = calloc(200000, sizeof(char));
            strcpy(continut_de_parsat, continut_html);

            /* se tin doua liste, una pentru linkuri gasite si alta pentru fisiere */
            char** lista_linkuri = NULL;
            char** lista_fisiere = NULL;
            int count_linkuri = 0;
            int count_fisiere = 0;

            
            /* parsare html tinand cont de enunt */
            while(continut_de_parsat[0]) {
                if(strncmp(continut_de_parsat, "<a href", 4) == 0) {
                    /* am gasit fisier de parsat */
                    char* resource = calloc(50, sizeof(char));
                    int c = 0;
                    int nu_afisez = 0;
                    int este_link = -1;
                    continut_de_parsat = continut_de_parsat + 9;
                    if(strncmp(continut_de_parsat, "http", 4) == 0) {
                        continut_de_parsat = continut_de_parsat + 1;
                        continue;
                    }
                    while(continut_de_parsat[0] != '\"' && continut_de_parsat[0] != '\'') {
                        if(continut_de_parsat[0] == '#')
                            nu_afisez = 1;
                        resource[c++] = continut_de_parsat[0];
                        continut_de_parsat = continut_de_parsat + 1;
                    }

                    char *token;
                    char *file_name = calloc(30, sizeof(char));
                    char *resursa_aux = calloc(50, sizeof(char));
                    strcpy(resursa_aux, resource);


                    char separator[2] = "/";
                    token = strtok(resursa_aux, separator);
                                            
                    while(token != NULL) {
                        strcpy(file_name, token);
                        token = strtok(NULL, separator);
                    }

                    if(strstr(file_name, ".") == NULL)
                        nu_afisez = 1;

                    if(nu_afisez == 0) {
                        char separator2[2] = ".";
                        token = strtok(file_name, separator2);
                        char *extension = calloc(4, sizeof(char));
                        while(token != NULL) {
                            strcpy(extension, token);
                            token = strtok(NULL, separator2);
                        }
                        if(strncmp(extension, "htm", 3) == 0) 
                            este_link = 1;
                        else
                            este_link = 0;
                    }

                    if(nu_afisez == 0) {
                        if(este_link == 0) {
                            if(resource[0] != '.' && resource[0] == '/')
                                lista_fisiere = add_link(lista_fisiere, resource, &count_fisiere);
                        }
                        else
                            if(este_link == 1) {
                                if(resource[0] == '/')
                                    lista_linkuri = add_link(lista_linkuri, resource, &count_linkuri);
                            }
                    }
                }
                continut_de_parsat = continut_de_parsat + 1;
            }

            /* concatenez linkurile si fisierul html propriu-zis intr-un singur sir de caractere pe care il trimit serverului */
            char* number = calloc(4, sizeof(char));
            sprintf(number, "%d", count_linkuri);
            strcat(continut, number);
            strcat(continut, "\n");
            int ind;
            for(ind = 0; ind < count_linkuri; ind++) {
                if(lista_linkuri[ind][0] == '/') {
                    if(log_file_exists == 1) 
                        fprintf(log_stdout, "Am descoperit un link: %s\n", lista_linkuri[ind]);
                    else
                        printf("Am descoperit un link: %s\n", lista_linkuri[ind]);
                    strcat(continut, lista_linkuri[ind]);
                    strcat(continut, "\n");
                }
            }

            number = calloc(4, sizeof(char));
            sprintf(number, "%d", count_fisiere);
            strcat(continut, number);
            strcat(continut, "\n");
            for(ind = 0; ind < count_fisiere; ind++) {
                if(lista_fisiere[ind][0] != '.') {
                    if(log_file_exists == 1) 
                        fprintf(log_stdout, "Am descoperit un fisier: %s\n", lista_fisiere[ind]);
                    else
                        printf("Am descoperit un fisier: %s\n", lista_fisiere[ind]);
                    
                    strcat(continut, lista_fisiere[ind]);
                    strcat(continut, "\n");
                }
            }

           
            continut_html = realloc(continut_html, strlen(continut_html));
            strcat(continut, continut_html);
            continut = realloc(continut, strlen(continut));

            if(log_file_exists == 1) 
                fprintf(log_stdout, "Am gasit %d resurse: %d fisiere si %d linkuri.\n", (count_fisiere + count_linkuri), count_fisiere, count_linkuri);
             else
                printf("Am gasit %d resurse: %d fisiere si %d linkuri.\n", (count_fisiere + count_linkuri), count_fisiere, count_linkuri);
            send(sockfd, continut, strlen(continut), 0);
        
            free(continut);
            free(continut_html);

    	}

    }

    if(log_file_exists == 1) {
        fclose(log_stderr);
        fclose(log_stdout);
    }
    return 0;
}


