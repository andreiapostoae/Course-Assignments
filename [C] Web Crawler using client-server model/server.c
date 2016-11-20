#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS	10
#define BUFLEN 2000000
#define MIN_CLIENTS 5
#define MAX_LINK_NO 100



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


void print_list(char** file_list, char *mesaj, int max_size, int log_file_exists, FILE* log_file) {
	/* afiseaza lista file_list in stdout sau fisier .stdout, respectiv stderr si afiseaza un mesaj */
	int i;
	if(log_file_exists == 0) {
		printf("%s\n", mesaj);
		for(i = 0; i < max_size; i++) {
			printf("%d. %s\n", (i+1), file_list[i]);
		}
		printf("\n");
	} else {
		fprintf(log_file, "%s\n", mesaj);
		for(i = 0; i < max_size; i++) {
			fprintf(log_file, "%d. %s\n", (i+1), file_list[i]);
		}
		fprintf(log_file, "\n");
	}
}

int get_connected_clients(int client_socket[50]) {
	/* returneaza numarul de clienti conectati la un moment dat */
	int i;
	int number = 0;
	for(i = 0; i < 40; i++)
		if(client_socket[i] != 0)
			number++;
	return number;
}

int get_socket(int client_socket[50], int index) {
	/* returneaza socketul pe care e conectat al index-lea client in ordine a conectarii */
	int count = 0;
	int i;
	for(i = 0; i < 50; i++) {
		if(client_socket[i] != 0) 
			count++;
		if(count == index) 
			return client_socket[i];
	}
	return -1;
}

void mkdir_recursive(char *dir) {
	/* functie care creeaza directoare recursiv */
    char tmp[256];
    char *p = NULL;
    size_t len;
    mode_t rwx = 0777;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if(tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    for(p = tmp + 1; *p; p++)
        if(*p == '/') {
            *p = 0;
            mkdir(tmp, rwx);
            *p = '/';
        }
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[BUFLEN];
     struct sockaddr_in serv_addr, cli_addr;
     int n, i, j;

     fd_set read_fds;	//multimea de citire folosita in select()
     fd_set tmp_fds;	//multime folosita temporar 
     int fdmax;		//valoare maxima file descriptor din multimea read_fds


     int everything = 0;
     int recursive = 0;
     int port_exists = 0;
     int log_file_exists = 0;
     char* log_file_name = calloc(20, sizeof(char));

     /* PARSARE ARGUMENTE */
     if (argc < 3 || argc > 7) {
         fprintf(stderr,"Usage: %s [-r] [-e] [-o <fisier_log>] -p <port> ", argv[0]);
         exit(1);
     }

     for(i = 0; i < argc - 1; i++) {
     	if(strcmp(argv[i], "-p") == 0) {
     		port_exists = 1;
     		portno = atoi(argv[i + 1]);
     	}
     }

     for(i = 0; i < argc - 1; i++) {
     	if(strcmp(argv[i], "-p") == 0) {
     		port_exists = 1;
     		portno = atoi(argv[i + 1]);
     	}
     }

     if(port_exists == 0) {
     	fprintf(stderr,"Usage: %s [-r] [-e] [-o <fisier_log>] -p <port> ", argv[0]);
        exit(1);
     }

     for(i = 0; i < argc - 1; i++) {
     	if(strcmp(argv[i], "-o") == 0) {
     		log_file_exists = 1;
     		strcpy(log_file_name, argv[i + 1]);
     	}
     }

     for(i = 0; i < argc; i++) {
     	if(strcmp(argv[i], "-r") == 0)
     		recursive = 1;
     	if(strcmp(argv[i], "-e") == 0)
     		everything = 1;
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

     //golim multimea de descriptori de citire (read_fds) si multimea tmp_fds 
     FD_ZERO(&read_fds);
     FD_ZERO(&tmp_fds);
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) {
     	if(log_file_exists == 1) {
     	  	fprintf(log_stderr, "ERROR opening socket\n");
     	  	fclose(log_stderr);
            fclose(log_stdout);
            exit(0);
     	}
     	else {
			fprintf(stdout, "ERROR opening socket\n");
			exit(0);
     	}
     }
      

     memset((char *) &serv_addr, 0, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
        if(log_file_exists == 1) {
     	  	fprintf(log_stderr, "ERROR on binding\n");
     	  	fclose(log_stderr);
            fclose(log_stdout);
            exit(0);
        }
     	else {
			fprintf(stdout, "ERROR on binding\n");
			exit(0);
     	}
     
     listen(sockfd, MAX_CLIENTS);

     //adaugam noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
     FD_SET(sockfd, &read_fds);
     FD_SET(0, &read_fds);
     fdmax = sockfd;

    char** download_list = NULL;
    int download_list_size = 0;

    char** client_list = NULL;
    int client_list_size = 0;

    char **finished_downloading = NULL;
    int finished_downloading_size = 0;

    char **waiting_files = NULL;
   	int waiting_files_size = 0;

    int client_socket[50];
	for(i = 0; i < MAX_CLIENTS; i++)
		client_socket[i] = 0;


	int ready_to_download = 1;
	int current_download = 0;
	int received_chunks = 0;
	int* assignments = calloc(MAX_LINK_NO, sizeof(int));
	int assignments_index = 0;
	int nivel_recursivitate = 1;

	int temp_index = 0;
     // main loop
	while (1) {
		tmp_fds = read_fds; 
		if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
			if(log_file_exists == 1) {
     	  		fprintf(log_stderr, "ERROR in select\n");
     	  		fclose(log_stderr);
                fclose(log_stdout);
                exit(0);
			}
     		else {
				fprintf(stdout, "ERROR in select\n");
				exit(0);
     		}

		if (FD_ISSET(0, &tmp_fds)) {
		  	memset(buffer, 0 , BUFLEN);
    		fgets(buffer, BUFLEN-1, stdin);

    		if(strncmp(buffer, "download", 8) == 0) {
    			/* se adauga in lista de downloaduri linkul aferent comenzii download */
    			char* link = calloc(100, sizeof(char));
    			sscanf(buffer, "download %s\n", link);
    			if(log_file_exists == 1)
     	  			fprintf(log_stdout, "Linkul %s a fost adaugat in coada de download.\n", link);
     			else
					fprintf(stdout, "Linkul %s a fost adaugat in coada de download.\n", link);
    			download_list = add_link(download_list, link, &download_list_size);
    			print_list(download_list, "Lista de downloaduri este: ", download_list_size, log_file_exists, log_stdout);

    		} else {
    			/* comanda status */
    			if(strncmp(buffer, "status", 6) == 0) {
    				print_list(client_list, "Lista de clienti este: ", client_list_size, log_file_exists, log_stdout);
    			} else 
    				/* comanda quit care anunta clientii sa se inchida, si apoi se inchide dupa o secunda */
    				if(strncmp(buffer, "quit", 4) == 0) {
		    			printf("Terminating server.\n");
		    			int k;
		    			
		    			for(k = 0; k < MAX_CLIENTS; k++) {
		    				int sd = client_socket[k];
		    				if(sd != 0 && FD_ISSET(sd, &read_fds)) {
		    					send(sd, "quit", 4 * sizeof(char), 0);
		    					FD_CLR(sd, &read_fds);
		    				}
		    			}
		    			sleep(1);
		    			if(log_file_exists == 1) {
		    				fclose(log_stdout);
		    				fclose(log_stderr);
		    			}
		    			close(sockfd);
		 				return 0;
		    		} else {
		    			printf("Unrecognized command.\n");
		    			continue;
		    		}
    		}
    		continue;
		}  
	
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
			
				if (i == sockfd) {
					// a venit ceva pe socketul inactiv(cel cu listen) = o noua conexiune
					// actiunea serverului: accept()
					clilen = sizeof(cli_addr);
					if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
						if(log_file_exists == 1) {
     	  					fprintf(log_stderr, "ERROR in accept\n");
     	  					fclose(log_stderr);
                    		fclose(log_stdout);
                    		exit(0);
						}
     					else{
							fprintf(stdout, "ERROR in accept\n");
							exit(0);
     					}
					} 
					else {
						//adaug noul socket intors de accept() la multimea descriptorilor de citire
						
						FD_SET(newsockfd, &read_fds);
						if (newsockfd > fdmax) { 
							fdmax = newsockfd;
						}
						
					}

					int k;
					for(k = 0; k < MAX_CLIENTS; k++)
						if(client_socket[k] == 0) {
							client_socket[k] = newsockfd;
							break;
						}

					if(log_file_exists == 1)
     	  				fprintf(log_stdout, "Noua conexiune de la %s, port %d, socket_client %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), newsockfd);
     				else
						fprintf(stdout, "Noua conexiune de la %s, port %d, socket_client %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), newsockfd);

					char *aux = calloc(20, sizeof(char));
					strcat(aux, inet_ntoa(cli_addr.sin_addr));
					strcat(aux, " ");
					char *port_aux = calloc(10, sizeof(char));
					sprintf(port_aux, "%d", ntohs(cli_addr.sin_port));
					strcat(aux, port_aux);
					client_list = add_link(client_list, aux, &client_list_size);

					if(client_list_size >= MIN_CLIENTS && ready_to_download == 1) {
						/* impart downloadurile clientilor dupa ce se conecteaza minimul*/
						ready_to_download = 0;
						int m = client_list_size;
						int n = download_list_size;
						int clusters = n / m;
						int q, r;
						for(q = 0; q < clusters; q++)
							for(r = 0; r < m; r++)
								assignments[assignments_index++] = r + 1;

						r = 0;
						for(q = clusters * m; q < n; q++) {
							assignments[assignments_index++] = r + 1;
							r++;
						}

						if(log_file_exists == 1) {
     	  					fprintf(log_stdout, "\n");
							fprintf(log_stdout, "S-a conectat minimul necesar de clienti.\n");
						} else {
							printf("\n");
							printf("S-a conectat minimul necesar de clienti.\n");
     					}

						for(q = 0; q < assignments_index; q++) {
							if(log_file_exists == 1)
     	  						fprintf(log_stdout, "Asociez downloadul linkului %s clientului de pe socketul %d.\n", download_list[q], get_socket(client_socket, assignments[q]));
     						else
								fprintf(stdout, "Asociez downloadul linkului %s clientului de pe socketul %d.\n", download_list[q], get_socket(client_socket, assignments[q]));

							char *temp = calloc(100, sizeof(char));
							char *hostname = calloc(30, sizeof(char));
					        char *address = calloc(50, sizeof(char));
					        char *token;
					        const char separator[2] = "/";
					        strcpy(temp, download_list[q]);
					                                
					        token = strtok(temp, separator);
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

					        strcat(hostname, address);
					        mkdir_recursive(hostname);
						}


						if(log_file_exists == 1) {
     	  					fprintf(log_stdout, "Creez structura de directoare necesara.\n\n");
							fprintf(log_stdout, "\nIncep downloadul...\n\n");
						} else {
							printf("Creez structura de directoare necesara.\n\n");
							printf("\nIncep downloadul...\n\n");
     					}

						for(q = 0; q < assignments_index; q++) {
							send(get_socket(client_socket, assignments[q]), download_list[q], strlen(download_list[q]), 0);
							if((q + 1) % m == 0) {
								current_download = q + 1;
								break;
							}
						}
					}
				}
					
				else {
					// am primit date pe unul din socketii cu care vorbesc cu clientii
					//actiunea serverului: recv()
					memset(buffer, 0, BUFLEN);
					if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {

						if (n == 0) {
							//conexiunea s-a inchis
							if(log_file_exists == 1)
     	  						fprintf(log_stdout, "selectserver: socket %d hung up\n", i);
     						else
								fprintf(stdout, "selectserver: socket %d hung up\n", i);
						} else {
							if(log_file_exists == 1) {
     	  						fprintf(log_stderr, "ERROR in recv\n");
     	  						fclose(log_stderr);
                    			fclose(log_stdout);
                    			exit(0);
							}
     						else {
								fprintf(stdout, "ERROR in recv\n");
								exit(0);
     						}
						}
						close(i); 
						FD_CLR(i, &read_fds);  
						int k;
						for(k = 0; k < MAX_CLIENTS; k++)
							if(client_socket[k] == i) {
								client_socket[k] = 0;
							}
					} 
					
					else { //recv intoarce >0
						received_chunks++; // retine a cata bucata a fost primita, deoarece programul e facut sa trimita fiecarui client,
										   // daca este posibil, un link, sa astepte raspuns si apoi sa retrimita
						if(log_file_exists == 1)
     	  					fprintf(log_stdout, "Am primit de la clientul de pe socketul %d o pagina html.\n", i);
     					else
							fprintf(stdout, "Am primit de la clientul de pe socketul %d o pagina html.\n", i);

						char* path = calloc(50, sizeof(char));
						char *aux = calloc(50, sizeof(char));
						char cwd[1024];
						getcwd(cwd, sizeof(cwd));

						strcat(path, cwd);
						sscanf(buffer, "path: %s\n", aux);

						char *link = aux + 1;
						if(log_file_exists == 1)
     	  					fprintf(log_stdout, "Am salvat-o in: %s\n\n", link);
     					else
							fprintf(stdout, "Am salvat-o in: %s\n\n", link);


						char *http_path = calloc(100, sizeof(char));
						strcat(http_path, "http://");
						strcat(http_path, link);
						strcat(path, aux);
						
						int numar_linkuri = 0;
						int numar_fisiere = 0;

						/* se parseaza pagina html primita:
						    - pe primul rand este pathul paginii respective (ele nu vor sosi in aceeasi ordine cu trimiterea)
						    - pe al doilea rand numarul de linkuri 
						    - pe urmatoarele numar_linkuri randuri, cate un link html gasit in pagina respectiva 
						    - pe urmatorul rand numarul de fisiere
						    - pe urmatoarele numar_fisiere randuri, cate un link catre un fisier gasit
						    - headerul
						    - pagina html propriu-zisa */

						char *doar_html = buffer;
						do {
							doar_html = doar_html + 1;
						} while(doar_html[0] != '\n');
						
						sscanf(doar_html, "%d\n", &numar_linkuri);
						int g;

						for(g = 0; g < numar_linkuri; g++) {
							char* link_temp = calloc(70, sizeof(char));
							do {
								doar_html = doar_html + 1;
							} while(doar_html[0] != '/');

							sscanf(doar_html, "%s\n", link_temp);

							do {
								doar_html = doar_html + 1;
							} while(doar_html[0] != '\n');
							waiting_files = add_link(waiting_files, link_temp, &waiting_files_size);
						}

						do {
							doar_html = doar_html + 1;
						} while(doar_html[1] < '0' || doar_html[1] > '9');
						
						sscanf(doar_html, "%d\n", &numar_fisiere);
						for(g = 0; g < numar_fisiere; g++) {
							char* fisier_temp = calloc(70, sizeof(char));

							do {
								doar_html = doar_html + 1;
							} while(doar_html[0] != '/');

							sscanf(doar_html, "%s\n", fisier_temp);
							
							do {
								doar_html = doar_html + 1;
							} while(doar_html[0] != '\n');

							waiting_files = add_link(waiting_files, fisier_temp, &waiting_files_size);
						} 


						FILE* f = fopen(path, "wb");
						
						fprintf(f, "%s", doar_html);
						fclose(f);

						int q;
						int m = client_list_size;
						if(received_chunks % client_list_size == 0) {
							for(q = current_download; q < assignments_index; q++) {
								send(get_socket(client_socket, assignments[q]), download_list[q], strlen(download_list[q]), 0);
								if((q + 1) % m == 0) {
									current_download = q + 1;
									break;
								}
							}
						}

						finished_downloading = add_link(finished_downloading, http_path, &finished_downloading_size);
						
						if(finished_downloading_size == download_list_size) {
							/* aici se termina programul in cazul in care nu se gasesc alte linkuri/fisiere si optiunile de recursivitate/everything nu sunt activate */
							nivel_recursivitate++;
							print_list(finished_downloading, "Lista de downloaduri terminate: ", finished_downloading_size, log_file_exists, log_stdout);
							int count_aux;
							for(count_aux = 0; count_aux < download_list_size; count_aux++)
								free(download_list[count_aux]);
							free(download_list);
							download_list_size = 0;

							if(log_file_exists == 1) 
								fprintf(log_stdout, "Lista de downloaduri a fost golita.\n");
							else
								printf("Lista de downloaduri a fost golita.\n");


							print_list(waiting_files, "\nFisiere in asteptare pentru modurile everything si recursive: ", waiting_files_size, log_file_exists, log_stdout);

							if(recursive == 0 && everything == 0) {
								if(log_file_exists == 1) 
									fprintf(log_stdout, "Am terminat downloadurile si modurile recursive si everything nu sunt activate.\n\nInchid serverul.\n");
								else
									printf("Am terminat downloadurile si modurile recursive si everything nu sunt activate.\n\nInchid serverul.\n");
								
								int k;
		    			
				    			for(k = 0; k < MAX_CLIENTS; k++) {
				    				int sd = client_socket[k];
				    				if(sd != 0 && FD_ISSET(sd, &read_fds)) {
				    					send(sd, "quit", 4 * sizeof(char), 0);
				    					FD_CLR(sd, &read_fds);
				    				}
				    			}
				    			sleep(1);
				    			if(log_file_exists == 1) {
				    				fclose(log_stdout);
				    				fclose(log_stderr);
				    			}
				    			close(sockfd);
				 				return 0;
							}
						}
						
					}
				} 
			}
		}
     }


     close(sockfd);
     fclose(log_stdout);
     fclose(log_stderr);


     return 0; 
}


