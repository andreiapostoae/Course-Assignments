#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_CLIENTS	40
#define BUFLEN 256

void error(char *msg)
{
    perror(msg);
    exit(1);
}

struct pair {
	char *field1;
	char *field2;
} ;

/* functia converteste un long int (reprezentand file size) intr-un string care
 * are formatul dorit in enunt: 123.456.789 */
char* convertPoint(long int x) {
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

/* functie de adaugare in hashmap */
struct pair* add_file(struct pair* list, struct pair file, int *max_size) {
	int i;
	for(i = 0; i < *max_size; i++) 
		if(strcmp(list[i].field1, file.field1) == 0 && strcmp(list[i].field2, file.field2) == 0) 
			return list;
		
	list = realloc(list, ((*max_size) + 1) * sizeof(struct pair));
	list[i].field1 = calloc(strlen(file.field1), sizeof(char));
	list[i].field2 = calloc(strlen(file.field2), sizeof(char));
	strcpy(list[i].field1, file.field1);
	strcpy(list[i].field2, file.field2);
	*max_size = *max_size + 1;
	return list;
}

/* functie de stergere din hashmap */
struct pair* remove_file(struct pair* list, struct pair file, int *max_size) {
	int i;
	int ok = 0;
	for(i = 0; i < *max_size; i++)
		if(strcmp(list[i].field1, file.field1) == 0 && strcmp(list[i].field2, file.field2) == 0) {
			ok = 1;
			char *aux1 = calloc(50, sizeof(char));
			char *aux2 = calloc(50, sizeof(char));
			strcpy(aux1, list[i].field1);
			strcpy(aux2, list[i].field2);
			strcpy(list[i].field1, list[*max_size - 1].field1);
			strcpy(list[i].field2, list[*max_size - 1].field2);
			strcpy(list[*max_size - 1].field1, aux1);
			strcpy(list[*max_size - 1].field2, aux2);
			free(aux1);
			free(aux2);
			break;
		}

	if(ok == 1) {
		list = realloc(list, ((*max_size) - 1) * sizeof(struct pair));
		*max_size = *max_size - 1;
	}

	return list;
}


int main(int argc, char *argv[]) {
	int n, i, j;
	FILE *shared_files = fopen(argv[2], "rt");
	FILE *users_config = fopen(argv[3], "rt");
	int number_of_users;
	fscanf(users_config, "%d", &number_of_users);

	/* in credentials se salveaza perechi de (username, parola) citite din users_config */
	struct pair* credentials = calloc(number_of_users, sizeof(struct pair));
	for(i = 0; i < number_of_users; i++) {
		credentials[i].field1 = calloc(24, sizeof(char));
		credentials[i].field2 = calloc(24, sizeof(char));
		fscanf(users_config, "%s %s", credentials[i].field1, credentials[i].field2);
	}


	size_t len = 0;
	ssize_t read;
	char *line;
	int number_of_shared_files = 0;
	fscanf(shared_files, "%d", &number_of_shared_files);
	struct pair* files = calloc(number_of_shared_files, sizeof(struct pair));

	i = 0;
	int shared_files_size = 0;
	int curline = 0;
	/* citire linie cu linie, ignorand-o pe prima din fisierul shared_files
	 * retin in files perechile de fisiere de forma (username, nume_fisier) */
	while ((read = getline(&line, &len, shared_files)) != -1) {
		if(i == 0) {
			i++;
			continue;
		}
		curline++;
		struct pair newpair;
		newpair.field1 = calloc(24, sizeof(char));
		newpair.field2 = calloc(24, sizeof(char));

		char *token;
        const char separator[2] = ":";

        token = strtok(line, separator);
        strcpy(newpair.field1, token);
        token = strtok(NULL, separator);
        
        strcpy(newpair.field2, token);

        newpair.field2[strlen(newpair.field2) - 1] = '\0';
        files = add_file(files, newpair, &shared_files_size);
		i++;
		
		if(curline == number_of_shared_files)
			break;
	}

	int u = 0;
	/* parcurg fiecare fisier precizat in fisierul shared_files, deoarece unele
	 * e posibil sa nu existe, si in enunt se precizeaza ca in acest caz, un mesaj
	 * relevant trebuie afisat in consola */
	while(u < shared_files_size) {
		char *path = calloc(30, sizeof(int));
		char cwd[1024];
	    getcwd(cwd, sizeof(cwd));
	    strcat(path, cwd);
	    strcat(path, "/");
	 	strcat(path, files[u].field1);
		strcat(path, "/");
		strcat(path, files[u].field2);
		FILE *aux = fopen(path, "r");
		if(aux == NULL) {
			struct pair aux_pair;
			aux_pair.field1 = calloc(20, sizeof(char));
			aux_pair.field2 = calloc(20, sizeof(char));
			strcpy(aux_pair.field1, files[u].field1);
			strcpy(aux_pair.field2, files[u].field2);
			files = remove_file(files, aux_pair, &shared_files_size);
			printf("%s's file, named %s, could not be found. (%d)\n", files[u].field1, files[u].field2, i);
			continue;
		}
		u++;
		fclose(aux);
	}

	/* daca nu exista folderele utilizatorilor, le creez eu */
	mode_t rwx = 0777;
	for(i = 0; i < number_of_users; i++)
		mkdir(credentials[i].field1, rwx);

	struct pair *private_files;
	private_files = calloc(1, sizeof(struct pair));
	int private_files_size = 0;

	/* partea de socketi este exact rezolvarea laboratorului de TCP cu multiplexare, incluzand scheletul */
    int sockfd, newsockfd, portno, clilen;
    char buffer[BUFLEN];
    struct sockaddr_in serv_addr, cli_addr;

    fd_set read_fds;	//multimea de citire folosita in select()
    fd_set tmp_fds;	//multime folosita temporar 
    int fdmax;		//valoare maxima file descriptor din multimea read_fds

     
    //golim multimea de descriptori de citire (read_fds) si multimea tmp_fds 
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
     
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
     
    portno = atoi(argv[1]);

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
    serv_addr.sin_port = htons(portno);
     
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
            error("ERROR on binding");
     
    listen(sockfd, MAX_CLIENTS);

    //adaugam noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
    FD_SET(sockfd, &read_fds);
    FD_SET(0, &read_fds);
    fdmax = sockfd;


    /* in current_users memorez utilizatorii conectati la un moment dat de forma (utilizator, socket) */
    struct pair* current_users; 
    current_users = calloc(1, sizeof(struct pair));

    int current_tries = 0;
    int logged_users = 0;

    /* verific daca in foldere exista fisiere neprecizate in shared_files, deci private si le adaug
     * in lista fisierelor private, care are forma (utilizator, fisier) */
	DIR *dirp;
	struct dirent *ent;
	struct pair not_listed_file;

	for(i = 0; i < number_of_users; i++) {
		char *user_name = calloc(24, sizeof(char));
		strcpy(user_name, credentials[i].field1);
		if((dirp = opendir(user_name)) != NULL) {
			while((ent = readdir(dirp)) != NULL) {
					if(ent->d_name[0] != '.' && ent->d_name[strlen(ent->d_name) - 1] != '~') {
						not_listed_file.field1 = calloc(20, sizeof(char));
						not_listed_file.field2 = calloc(20, sizeof(char));
						strcpy(not_listed_file.field1, user_name);
						strcpy(not_listed_file.field2, ent->d_name);
						int ok = 0;
						int k;
						for(k = 0; k < shared_files_size; k++) {
							if(strcmp(ent->d_name, files[k].field2) == 0 && strcmp(user_name, files[k].field1) == 0)
								ok = 1;
						}
						if(ok == 0) 
							private_files = add_file(private_files, not_listed_file, &private_files_size);
					}
				}
			}
		closedir(dirp);
	}

	/* retin socketii ocupati in vectorul client_socket, pentru a sti la quit cui trebuie sa trimit */
	int client_socket[50];
	for(i = 0; i < MAX_CLIENTS; i++)
		client_socket[i] = 0;

    // main loop
	while (1) {
		tmp_fds = read_fds;

		if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
			error("ERROR in select");

		/* daca serverul primeste de la tastatura quit, trimite tuturor clientilor "quit", asteapta o secunda,
		 * timp in care clientii se vor inchide, dupa care se va inchide si el */
		if (FD_ISSET(0, &tmp_fds)) {
			  	memset(buffer, 0 , BUFLEN);
	    		fgets(buffer, BUFLEN-1, stdin);
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
	    			close(sockfd);
	 				return 0;
	    		} else {
	    			printf("Unrecognized command.\n");
	    			continue;
	    		}
			}  
		

		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
			
				if (i == sockfd) {
					// a venit ceva pe socketul inactiv(cel cu listen) = o noua conexiune
					// actiunea serverului: accept()
					clilen = sizeof(cli_addr);
					if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
						error("ERROR in accept");
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
					printf("\nNew connection from %s, port %d, socket %d.\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), newsockfd);
				}
					
				else {
					// am primit date pe unul din socketii cu care vorbesc cu clientii
					//actiunea serverului: recv()
					memset(buffer, 0, BUFLEN);
					if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {


						if (n == 0) {
							//conexiunea s-a inchis
							printf("\nSocket %d client hung up.\n", i);
						} else {
							error("ERROR in recv");
						}
						close(i); 
						FD_CLR(i, &read_fds); // scoatem din multimea de citire socketul pe care 
						int k;
						for(k = 0; k < MAX_CLIENTS; k++)
							if(client_socket[k] == i) {
								client_socket[k] = 0;
							}
					} 
					
					else { //recv intoarce >0
						buffer[strlen(buffer) - 1] = '\0';
						printf ("\nReceived from client: %s (%d)\n", buffer, i);
						if(strncmp(buffer, "login", 5) == 0) {
							/* LOGIN - daca userul si/sau parola sunt gresite, trimit -3
									 - daca sunt corecte, adaug userul in lista de useri activi si trimit 0
									 - daca detectez 3 incercari de login la rand gresite, inchid conexiunea
									   cu clientul respectiv si afisez un mesaj relevant */
							char* tried_pw = calloc(24, sizeof(char));
							char* tried_user = calloc(24, sizeof(char));

							sscanf(buffer, "login %s %s", tried_user, tried_pw);
							printf("Attempted login: user = %s, pw = %s. (%d)\n", tried_user, tried_pw, i);
							int k;
							int successful_login = -3;
							for(k = 0; k < number_of_users; k++) {
								if(strcmp(credentials[k].field1, tried_user) == 0 && strcmp(credentials[k].field2, tried_pw) == 0) {
									printf("Login as %s successful. (%d)\n", tried_user, i);
									successful_login = 0;
									current_tries = 0;
									break;
								}
							}
							if(successful_login == -3) {
								printf("Login as %s failed. (%d)\n", tried_user, i);
								current_tries++;
							}

							if(current_tries == 3) {
								successful_login = -8;
								sprintf(buffer, "%d", successful_login);
								send(i, buffer, 250, 0);
								FD_CLR(i, &read_fds);
								int k;
								for(k = 0; k < MAX_CLIENTS; k++)
									if(client_socket[k] == i) {
										client_socket[k] = 0;
									}
								printf("Brute forced detected. Connection closed. (%d)\n", i);
							}

							if(successful_login == 0) {
								struct pair current_user;
								current_user.field1 = calloc(24, sizeof(char));
								current_user.field2 = calloc(5, sizeof(char));
								strcpy(current_user.field1, tried_user);
								sprintf(current_user.field2, "%d", i);
								current_users = add_file(current_users, current_user, &logged_users);
							}

							sprintf(buffer, "%d", successful_login);
							send(i, buffer, 250, 0);
						} else {
							/* LOGOUT - trimit 0 dupa ce sterg userul din lista de useri activi */
							if(strncmp(buffer, "logout", 6) == 0) {
								sprintf(buffer, "%d", 0);
								char* current_user = calloc(24, sizeof(char));
								struct pair usr;
								int k;
								usr.field1 = calloc(24, sizeof(char));
								usr.field2 = calloc(5, sizeof(char));
								char* char_aux = calloc(24, sizeof(char));
								sprintf(char_aux, "%d", i);
								strcpy(usr.field2, char_aux);
								
								for(k = 0; k < logged_users; k++)
									if(strcmp(current_users[k].field2, char_aux) == 0)
										strcpy(usr.field2, current_users[k].field1);
								current_users = remove_file(current_users, usr, &logged_users);
								
								send(i, buffer, 250, 0);
								printf("Logged out. (%d)\n", i);
							} else {
								if(strncmp(buffer, "getuserlist", 11) == 0) {
									/* GETUSERLIST - trimit la intervale de 0.005 secunde cate un
													 username pentru a evita overflowul */
									printf("Client requested user list. (%d)\n", i);
									int k;
									char* aux = calloc(3, sizeof(char));
									sprintf(aux, "%d", number_of_users);
									send(i, aux, 3 * sizeof(char), 0);
									nanosleep((const struct timespec[]){{0, 5000000L}}, NULL);
									for(k = 0; k < number_of_users; k++) {
										send(i, credentials[k].field1, 24 * sizeof(char), 0);
										printf("SENDING %s\n", credentials[k].field1);
										nanosleep((const struct timespec[]){{0, 5000000L}}, NULL);
									}
								} else {
									/* GETFILELIST - numara sa vada daca exista fisiere 
									               - daca da, si userul exista, trimite mai intai
									                 numarul fisierelor si apoi, la interval
									                 de 0.005 secunde cate un fisier, impreuna cu 
									                 dimensiunea sa si tipul shared/private
									               - daca userul nu exista, trimite -11
									               - daca nu exista fisiere, dar userul da, trimite
									                 null */
									if(strncmp(buffer, "getfilelist", 11) == 0){
										char* requested_user = calloc(24, sizeof(char));
										sscanf(buffer, "getfilelist %s", requested_user);
										printf("Client requested file list. (%d)\n", i);
										int k;
										
										int found = 0;
										int found_user = 0;
										int count = 0;
										char* aux = calloc(3, sizeof(char));
										for(k = 0; k < shared_files_size; k++) {
											if(strcmp(files[k].field1, requested_user) == 0) {
												count++;
											}
										}

										for(k = 0; k < private_files_size; k++) {
											if(strcmp(private_files[k].field1, requested_user) == 0) {
												count++;
											}
										}

										sprintf(aux, "%d", count);
										send(i, aux, 3 * sizeof(char), 0);
										nanosleep((const struct timespec[]){{0, 5000000L}}, NULL);

										for(k = 0; k < number_of_users; k++)
											if(strcmp(requested_user, credentials[k].field1) == 0)
												found_user = 1;

										for(k = 0; k < shared_files_size; k++) {
											if(strcmp(files[k].field1, requested_user) == 0) {
												char* list = calloc(200, sizeof(char));
												found = 1;
												char *path = calloc(30, sizeof(int));
												char cwd[1024];
											    getcwd(cwd, sizeof(cwd));
											    strcat(path, cwd);
											    strcat(path, "/");
											 	strcat(path, files[k].field1);
												strcat(path, "/");
												strcat(path, files[k].field2);
											   	
												FILE *aux = fopen(path, "rb");
												fseek(aux, 0L, SEEK_END);
												long int fs = ftell(aux); //filesize
												fclose(aux);

												strcat(list, files[k].field2);
												strcat(list, "\t");
												strcat(list, convertPoint(fs));
												strcat(list, " bytes");
												strcat(list, "\tSHARED\t");
												send(i, list, 200 * sizeof(char), 0);
												printf("SENDING %s\n", list);
												nanosleep((const struct timespec[]){{0, 5000000L}}, NULL);
											} 
										}


										for(k = 0; k < private_files_size; k++) {
											if(strcmp(private_files[k].field1, requested_user) == 0) {
												char* list = calloc(200, sizeof(char));
												found = 1;
												char *path = calloc(30, sizeof(int));
												char cwd[1024];
											    getcwd(cwd, sizeof(cwd));
											    strcat(path, cwd);
											    strcat(path, "/");
											 	strcat(path, private_files[k].field1);
												strcat(path, "/");
												strcat(path, private_files[k].field2);
											   	
												FILE *aux = fopen(path, "rb");
												fseek(aux, 0L, SEEK_END);
												long int fs = ftell(aux); //filesize
												fclose(aux);

												strcat(list, private_files[k].field2);
												strcat(list, "\t");
												strcat(list, convertPoint(fs));
												strcat(list, " bytes");
												strcat(list, "\tPRIVATE\t");
												send(i, list, 200 * sizeof(char), 0);
												printf("SENDING %s\n", list);
												nanosleep((const struct timespec[]){{0, 5000000L}}, NULL);
											} 
										}


										if(found == 0 && found_user == 0) {
											sprintf(buffer, "%d", -11);
											send(i, buffer, 250, 0);
											printf("Found no files. (%d)\n", i);
										} 

									} else {
										if(strncmp(buffer, "share", 5) == 0) {
											/* SHARE - trimite 0 in cazul in care fisierul a fost gasit si este privat, iar
											           fisierul devine public
											         - trimite -4 in cazul in care fisierul nu exista in folderul utilizatorului
											           care incearca sa-l faca public
											         - trimite -6 in cazul in care fisierul exista, dar este deja public */
											char* file_name = calloc(50, sizeof(char));
											char *token;
											const char separator[2] = " ";
											
											token = strtok(buffer, separator);
											
											while(token != NULL) {
											    token = strtok(NULL, separator);
											    if(token != NULL) {
											    	strcat(file_name, token);
											    	strcat(file_name, " ");
											    }
											}

											file_name[strlen(file_name) - 1] = '\0';

											int found = 0;
											int found_shared = 0;
											int k;

											char* current_user = calloc(24, sizeof(char));
											char* char_aux = calloc(24, sizeof(char));
											sprintf(char_aux, "%d", i);
											for(k = 0; k < logged_users; k++)
											if(strcmp(current_users[k].field2, char_aux) == 0)
												strcpy(current_user, current_users[k].field1);

											for(k = 0; k < shared_files_size; k++)
												if(strcmp(files[k].field2, file_name) == 0 && strcmp(files[k].field1, current_user) == 0) {
													found = 1;
													found_shared = 1;
													break;
												}

											for(k = 0; k < private_files_size; k++)
												if(strcmp(private_files[k].field2, file_name) == 0 && strcmp(private_files[k].field1, current_user) == 0) {
													found = 1;
													struct pair pair1;
													pair1.field1 = calloc(24, sizeof(char));
													pair1.field2 = calloc(50, sizeof(char));
													strcpy(pair1.field1, current_user);
													strcpy(pair1.field2, file_name);
													private_files = remove_file(private_files, pair1, &private_files_size);
													files = add_file(files, pair1, &shared_files_size);
													break;
												}

											printf("Client wants to share file: %s. (%d)\n", file_name, i);

											if(found == 0 && found_shared == 0) {
												printf("The requested file could not be found. (%d)\n", i);
												sprintf(buffer, "%d", -4);
											}

											if(found == 1 && found_shared == 0) {
												printf("The requested file was successfully shared. (%d)\n", i);
												sprintf(buffer, "%d", 0);
											}

											if(found == 1 && found_shared == 1) {
												printf("The requested file is already shared. (%d)\n", i);
												sprintf(buffer, "%d", -6);
											}

											send(i, buffer, 20, 0);
											
										} else 
											if(strncmp(buffer, "unshare", 7) == 0) {
												/* UNSHARE - trimite 0 in cazul in care fisierul a fost gasit si este public, iar
											           fisierul devine privat
											         - trimite -4 in cazul in care fisierul nu exista in folderul utilizatorului
											           care incearca sa-l faca privat
											         - trimite -7 in cazul in care fisierul exista, dar este deja privat */
												char* file_name = calloc(50, sizeof(char));
												char *token;
												const char separator[2] = " ";
											
												token = strtok(buffer, separator);
											
												while(token != NULL) {
											    	token = strtok(NULL, separator);
											    	if(token != NULL) {
											    		strcat(file_name, token);
											    		strcat(file_name, " ");
											   		}
												}

												file_name[strlen(file_name) - 1] = '\0';

												int found = 0;
												int found_private = 0;
												int k;

												char* current_user = calloc(24, sizeof(char));
												char* char_aux = calloc(24, sizeof(char));
												sprintf(char_aux, "%d", i);
												for(k = 0; k < logged_users; k++)
												if(strcmp(current_users[k].field2, char_aux) == 0)
													strcpy(current_user, current_users[k].field1);

												for(k = 0; k < private_files_size; k++)
													if(strcmp(private_files[k].field2, file_name) == 0 && strcmp(private_files[k].field1, current_user) == 0) {
														found = 1;
														found_private = 1;
														break;
													}

												for(k = 0; k < shared_files_size; k++)
													if(strcmp(files[k].field2, file_name) == 0 && strcmp(files[k].field1, current_user) == 0) {
														found = 1;
														struct pair pair1;
														pair1.field1 = calloc(24, sizeof(char));
														pair1.field2 = calloc(50, sizeof(char));
														strcpy(pair1.field1, current_user);
														strcpy(pair1.field2, file_name);
														files = remove_file(files, pair1, &shared_files_size);
														private_files = add_file(private_files, pair1, &private_files_size);
														break;
													}


												printf("Client wants to unshare file: %s. (%d)\n", file_name, i);

												if(found == 0 && found_private == 0) {
													printf("The requested file could not be found. (%d)\n", i);
													sprintf(buffer, "%d", -4);
												}

												if(found == 1 && found_private == 0) {
													printf("The requested file was successfully unshared. (%d)\n", i);
													sprintf(buffer, "%d", 0);
												}

												if(found == 1 && found_private == 1) {
													printf("The requested file is already unshared. (%d)\n", i);
													sprintf(buffer, "%d", -7);
												}

												send(i, buffer, 20, 0);
											} else
												if(strncmp(buffer, "delete", 6) == 0)  {
													/* DELETE - cauta fisierul in folderul utilizatorului curent si incearca sa il stearga
													          - daca il gaseste, o face si trimite 0
													          - daca nu, trimite -4 */
													int k;
													char* file_name = calloc(24, sizeof(char));
													char *token;
													const char separator[2] = " ";
												
													token = strtok(buffer, separator);
												
													while(token != NULL) {
												    	token = strtok(NULL, separator);
												    	if(token != NULL) {
												    		strcat(file_name, token);
												    		strcat(file_name, " ");
												   		}
													}

													file_name[strlen(file_name) - 1] = '\0';

													char* current_user = calloc(24, sizeof(char));
													char* char_aux = calloc(24, sizeof(char));
													sprintf(char_aux, "%d", i);
													printf("Client requested %s deletion.(%d)\n", file_name, i);
													for(k = 0; k < logged_users; k++)
														if(strcmp(current_users[k].field2, char_aux) == 0)
															strcpy(current_user, current_users[k].field1);

													int file_exists = 0;

													for(k = 0; k < shared_files_size; k++)
														if(strcmp(files[k].field1, current_user) == 0 && strcmp(files[k].field2, file_name) == 0 ) {
															file_exists = 1;
															char *path = calloc(30, sizeof(int));
															char cwd[1024];
															getcwd(cwd, sizeof(cwd));
															strcat(path, cwd);
															strcat(path, "/");
															strcat(path, files[k].field1);
															strcat(path, "/");
															strcat(path, files[k].field2);
															unlink(path);
															struct pair pair_aux;
															pair_aux.field1 = calloc(20, sizeof(char));
															pair_aux.field2 = calloc(20, sizeof(char));
															strcpy(pair_aux.field1, current_user);
															strcpy(pair_aux.field2, file_name);
															files = remove_file(files, pair_aux, &shared_files_size);
															break;
														}

													for(k = 0; k < private_files_size; k++)
														if(strcmp(private_files[k].field1, current_user) == 0 && strcmp(private_files[k].field2, file_name) == 0 ) {
															file_exists = 1;
															char *path = calloc(30, sizeof(int));
															char cwd[1024];
															getcwd(cwd, sizeof(cwd));
															strcat(path, cwd);
															strcat(path, "/");
															strcat(path, private_files[k].field1);
															strcat(path, "/");
															strcat(path, private_files[k].field2);
															unlink(path);
															struct pair pair_aux;
															pair_aux.field1 = calloc(20, sizeof(char));
															pair_aux.field2 = calloc(20, sizeof(char));
															strcpy(pair_aux.field1, current_user);
															strcpy(pair_aux.field2, file_name);
															private_files = remove_file(private_files, pair_aux, &private_files_size);
															break;
														}

													if(file_exists == 0)
														sprintf(buffer, "%d", -4);
													else 
														sprintf(buffer, "%d", 0);

													send(i, buffer, 20, 0);

												} else 
														printf("Unrecognized command.\n");
									}
								}
							}
						}
					}
				} 
			}
		}
     }


    return 0; 
}


