/* APOSTOAE Andrei Alexandru - 311CB */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* definire structuri de baza */
typedef struct celg
{ 
struct celg *urm;
void* info;
} TCelulaG, *TLG, **ALG; 

typedef int (*TFHASH)(char*,int);

typedef void (*TF)(void*); 

typedef int (*TFCOMP)(const void *, const void *);

typedef struct 
{
size_t M;
TFHASH fd;
TLG *v;
} TD;



/* definire structura web */

typedef struct
{
char *IP;
char *adresa;
} TWebsite;



/* functii utile din laborator ce vor fi folosite */

void DistrugeLG(ALG aL)
{ 
TLG aux = *aL;
if (!aux) return;
while (*aL)
	{
	aux=*aL;
  	free(((TWebsite*)(aux->info))->IP);
  	free(((TWebsite*)(aux->info))->adresa);
  	free(aux->info);
  	*aL = aux->urm;
  	free(aux);
    }
}

int InsLG(ALG aL, TWebsite* ae, size_t d)
{
TLG aux = malloc(sizeof(TCelulaG));
if(!aux) return 0;
aux->info = malloc(d);
if (!aux->info)          
	{
	free(aux); 
	return 0;
	}
memcpy(aux->info, ae, d);
aux->urm = *aL;
*aL = aux;
return 1;
}


TD* InitTD(size_t aM,TFHASH ffd)
{
TD* atd;
atd=(TD*)malloc(sizeof(TD));
if(!atd)
	return NULL;
atd->v=(TLG*)calloc(aM,sizeof(TLG));
if(!atd->v)
	{	
	free(atd);
	return NULL;
	}
atd->M=aM;
atd->fd=ffd;
return atd;
}

void DistrugeTD(TD** atd)
{
int k;
TLG L;
for(k=0;k<(*atd)->M;k++)
	{
	L=(*atd)->v[k];
	DistrugeLG(&L);
	}
free((*atd)->v);
free(*atd);
*atd=NULL;
}

void Afisare(ALG aL, TF afiEL)
{ 
if(!*aL) 
	{ 
	printf("\nBucketul este gol.\n"); 
	return;
	}
printf("\n");
for(; *aL; aL = &(*aL)->urm)
	afiEL((*aL)->info);
printf("\n");
}

void AfiEl(void * ae)
{
TWebsite p = *(TWebsite*)ae;
printf("IP: %s\nAdresa: %s\n\n", p.IP, p.adresa);
}



/* functia hash a problemei */

int hash(char *s,int m)
{
int i;
int p=0;
for(i=0;i<strlen(s);i++)
	p=p+s[i];
return p%m;
}


/* functiile principale ale problemei */

TD* put(TD* td, char *IP, char *Adresa)
//adauga adresa in tabela hash utilizand inserarea ordonata
//returneaza noua tabela
{
int k=((*td).fd)(Adresa,(*td).M);
char *adr;
adr=malloc(20*sizeof(char));
TLG aux2=td->v[k];
//daca adresa este gasita deja in tabela, aceasta nu va mai fi adaugata:
for(;aux2;aux2=aux2->urm)
	{
		if(strcmp(((TWebsite*)(aux2->info))->adresa,Adresa)==0) return td;
	}
TWebsite ae;
ae.IP=malloc(20*sizeof(char));
ae.adresa=malloc(20*sizeof(char));
strcpy(ae.IP,IP);
strcpy(ae.adresa,Adresa);
ALG aL=&(td->v[k]);
TLG p,ant,aux;
if(!*aL) //daca lista este vida, atunci se insereaza elementul cu functia de la curs InsLG
	InsLG(&(td->v[k]),&ae,sizeof(TWebsite));
else  //daca nu este vida, se insereaza ordonat folosindu-se functia de inserare ordonata
	  //tot de la curs
	{
	p=*aL;
	ant=NULL;
	for(;p;ant=p,p=p->urm)
		{
		adr=((TWebsite*)(p->info))->adresa;
		if(strcmp(Adresa,adr)<0) break;
		}
	aux=(TLG)malloc(sizeof(TCelulaG));
	if(!aux) return 0;
	aux->info=malloc(sizeof(TWebsite));
	if(!aux->info)
		{
		free(aux);
		return 0;
		}
	memcpy(aux->info,&ae,sizeof(TWebsite));
	if(ant==NULL) 
		{
		aux->urm=p;
		*aL=aux;
		}
	else
		{
		aux->urm=p;
		ant->urm=aux;
		}
	}
return td;
}

char* get(TD* td, char* Adresa) 
//cauta adresa si returneaza IP-ul daca este gasit, respectiv NULL in caz contrar
{
int k;
char *IP;
IP=malloc(20*sizeof(char));
char *adr;
adr=malloc(20*sizeof(char));
k=((*td).fd)(Adresa,(*td).M);
TLG l;
l=td->v[k];
int ok=0;
for(;l;l=l->urm)
	{
	adr=((TWebsite*)(l->info))->adresa;
	if(strcmp(adr,Adresa)==0)
		{
		ok=1;
		strcpy(IP,((TWebsite*)(l->info))->IP);
		break;
		}
	}
if(ok==1) return IP;
return NULL;
}

int find(TD* td, char* Adresa)
//cauta adresa si returneaza 1 daca a gasit, 0 in caz contrar
{
int k;
char *IP;
IP=malloc(20*sizeof(char));
char *adr;
adr=malloc(20*sizeof(char));
k=((*td).fd)(Adresa,(*td).M);
TLG l;
l=td->v[k];
int ok=0;
for(;l;l=l->urm)
	{
	adr=((TWebsite*)(l->info))->adresa;
	if(strcmp(adr,Adresa)==0) 
		{
		ok=1;
		strcpy(IP,((TWebsite*)(l->info))->IP);
		break;
		}
	}
return ok;
}

void print_bucket(TD* td, int bucket,FILE *f)
//se cauta bucketul: daca este gasit, se afiseaza valorile, iar daca nu, nu se afiseaza nimic
{
if(bucket<=(*td).M-1)
	{
	TLG l;
	l=td->v[bucket];
	char *IP=malloc(40*sizeof(char));
	if(!l)
		fprintf(f,"VIDA\n");
	else 
		{
		for(;l!=NULL;l=l->urm)
			{
			IP=((TWebsite*)(l->info))->IP;
			fprintf(f,"%s ",IP);
			}
		fprintf(f,"\n");
		}
	}	
}

void print(TD* td,FILE *f)
//se afiseaza toata tabela hash
{
int m;
m=(*td).M;
int i=0;
TLG l;
char *IP=malloc(20*sizeof(char));
for(i=0;i<=m-1;i++) //pentru fiecare bucket se realizeaza afisarea
	{
	l=td->v[i];
	if(l)
		{
		fprintf(f,"%d: ",i);
		for(;l && l->urm;l=l->urm)
			{
			IP=((TWebsite*)(l->info))->IP;
			fprintf(f,"%s ",IP);
			}
		IP=((TWebsite*)(l->info))->IP;
		fprintf(f,"%s ",IP);
		fprintf(f,"\n");
		}
	}
}

int removead(TD* td, char* Adresa)
//cauta adresa, dupa care o sterge; daca nu o gaseste, nu se intampla nimic
//returneaza 1 pentru succes, 0 in caz contrar
{
int k;
int ok=0;
k=((*td).fd)(Adresa,(*td).M);
TLG p;
ALG ap=&(td->v[k]);
if(strcmp(((TWebsite*)((*ap)->info))->adresa,Adresa)==0)
//cazul in care adresa ce trebuie stearsa se afla pe prima pozitie a listei
	{
	*ap=(*ap)->urm;
	}
else
	{
	//cazul in care adresa ce trebuie stearsa nu se afla pe prima pozitie a listei
	p=td->v[k];
	while(p!=NULL && (p->urm!=NULL))
		{
		if(strcmp(((TWebsite*)((p->urm)->info))->adresa,Adresa)==0)
			{
			ok=1;
			break;
			}
		p=p->urm;
		}
	if (ok==1) p->urm=(p->urm)->urm; //avansul peste elementul sters
	else return 0;
	}
return 1;
}

int main(int argc, char *argv[])
{
/* declararea variabilelor */
	
TD* tabela;
int marimehash;
marimehash=atoi(argv[1]); //marimea tabelei este extrasa din linia de comanda
//functia atoi transforma un string format din cifre intr-un int
tabela=InitTD(marimehash,hash); //initiere tabela
char *inputf=malloc(20*sizeof(char));
char *outputf=malloc(20*sizeof(char));
strcpy(inputf,argv[2]);
strcpy(outputf,argv[3]);
FILE * input;
input=fopen(inputf,"rt");
FILE * output;
output=fopen(outputf,"wt");
char * line = NULL;
size_t len = 0;
ssize_t read;
int i,p;
char *arg=malloc(30*sizeof(char));
char *arg1=malloc(30*sizeof(char));
char *arg2=malloc(30*sizeof(char));


/* citirea comenzilor din fisierul de input */

while ((read = getline(&line, &len, input)) != -1) //cat timp exista o noua linie in fisier
	{
        if(line[0]=='g') //get
        	{
        	memset(arg,0,strlen(arg)); //reinitializarea cu valori nule a unui string
        	i=4;
        	while(line[i]!='\0' && line[i]!='\n') 
			{
        		arg[i-4]=line[i];
        		i++;
        		}
        	if(get(tabela,arg)==NULL) fprintf(output,"NULL\n");
        		else fprintf(output,"%s\n",get(tabela,arg));
        	}
        
	if(line[1]=='u') //put
		{
		memset(arg,0,strlen(arg));
		memset(arg1,0,strlen(arg1));
		memset(arg2,0,strlen(arg2));
		i=4;
		while(line[i]!='\0') 
			{
        		arg[i-4]=line[i];
        		i++;
        		}
        	i=0;
        	while(arg[i]!=' ')
			{
        		arg1[i]=arg[i];
        		i++;
        		}
            	i++;
            	p=0;
            	while(arg[i]!='\0' && arg[i]!='\n')
            		{
            		arg2[p]=arg[i];
        		i++;
        		p++;
        		}
        	tabela=put(tabela,arg2,arg1);
		}  
		
	if(line[0]=='f') //find
		{
		memset(arg,0,strlen(arg));
        	i=5;
        	while(line[i]!='\0' && line[i]!='\n') 
			{
        		arg[i-5]=line[i];
        		i++;
        		}
        	p=find(tabela,arg);
        	if(p==1) fprintf(output,"True\n");
        		else fprintf(output,"False\n");
		}
		
	if(line[1]=='r' && line[5]!='_') //print    
		 print(tabela,output);
		
	if(line[1]=='r' && line[5]=='_') //print_bucket
		{
		memset(arg,0,strlen(arg));
		i=13;
        	while(line[i]!='\0' && line[i]!='\n')
        		{
        		arg[i-13]=line[i];
        		i++;
        		};
        	p=atoi(arg);
		print_bucket(tabela,p,output);
		}
		
	if(line[0]=='r') //remove
		{
		memset(arg,0,strlen(arg));
		i=7;
        	while(line[i]!='\0' && line[i]!='\n' && line[i]!=' ')
        		{
        		arg[i-7]=line[i];
        		i++;
        		}
        	if (find(tabela,arg)==1) removead(tabela,arg);
		}
	}


/* eliberari de memorie */

free(inputf);
free(outputf);
free(arg);
free(arg1);
free(arg2);
free(line);
fclose(input);
fclose(output);
DistrugeTD(&tabela);

return 0;
}
