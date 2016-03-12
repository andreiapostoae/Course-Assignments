#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


/* definire structuri de baza */

typedef struct
{
	int ID;
	int prioritate;
	int pas;
} TProces;

typedef struct celg
{ 
	struct celg *urm;
	void* info;
} TCelulaG, *TLG, **ALG; 

#define ID(x) ((TProces*)(x->info))->ID
#define prioritate(x) ((TProces*)(x->info))->prioritate
#define pas(x) ((TProces*)(x->info))->pas

typedef void (*TF)(void*); 


/* functie de afisare pentru o coada sau o stiva de tip ALG */

int AfisareCoada(ALG coada, FILE *f)
/* functia primeste ca parametri o structura de tip ALG si fisierul in care vor fi scrise informatiile */
/* functia returneaza 0 daca lista data ca parametru este vida si 1 daca nu */
// bineinteles, conform enuntului problemei, afisarea stivei sau cozii de tip ALG prin parcurgere este permisa
{
	TLG p=*coada;
	if(!p) return 0; 
	for(;p;p=p->urm)
		fprintf(f,"%d ",ID(p));
	fprintf(f,"\n");
	return 1;
}

/* functia DistrugereLG elibereaza continutul unei liste generice */
void DistrugereLG(TLG r)
/* functia din curs */
{
	TLG aux=NULL;
	TLG current=NULL;
	while (current!=NULL)
	{
		aux=current;
		current=current->urm;
		free(aux->info);
		free(aux);
	}
}

/* functiile Pop, Push pentru stiva si ExtrQ si InsQ pentru coada */

/* functia Push adauga un element in stiva (in varful ei) */
void Push(ALG aL, TLG r)
/* functia primeste ca parametru o lista de tip ALG (adica stiva) 
   si o lista TLG care sa fie adaugata la inceputul ei */
// functia nu are nevoie de alocari de memorie, deoarece al doilea parametru, cand va fi dat in main, va fi si alocat
{
	r->urm=*aL;
	*aL=r;
}

/* functia Pop scoate un element in stiva (din varful ei) */
TLG Pop(ALG aL) 
/* functia primeste ca parametru lista din care trebuie scos elementul */
/* returneaza elementul scos sub forma de TLG */
/* daca lista data ca parametru este vida, returneaza NULL */
/* IMPORTANT: functia Pop pentru stive este aceeasi ca functia ExtrQ 
   pentru cozi */
{
	if(!*aL) return NULL;
	TLG aux= *aL;
	*aL=(*aL)->urm;
	aux->urm=NULL;
	return aux;
}

/* functia InsQ insereaza ordonat dupa prioritate un element de tipul TLG */
int InsQ(ALG aL, TLG r)
/* functia primeste ca parametru lista in care trebuie adaugat elementul 
 si elementul de tipul TLG adaugat */
/* sortarea se face descrescator dupa prioritate si apoi crescator dupa pas 
 functia returneaz a 1 pentru adaugare executata cu succes si 0 altfel */
{
	if (r==NULL)
		return 0;
	if(!*aL)
	/* daca lista data ca parametru e nula, ea va deveni chiar elementul r */
	{
		*aL=r;
		return 1; /* adaugare realizata cu succes, deci iesire din functie */
	}

	TLG p=*aL,ant=NULL,v=NULL,ant2=NULL;

	/* for-ul determina pozitia unde trebuie inserat elementul */
	for(;p;ant=p,p=p->urm)
	{
		if(prioritate(p)<prioritate(r)) /* daca elementul la care se afla are */
			break;                      /* prioritate mai mica decat cel care trebuie */
										/* adaugat, atunci pozitia e corecta => break */
		if(prioritate(p)==prioritate(r)) 
		/* daca prioritatile sunt egale, atunci sortarea se executa dupa 
		   ordinea crescatoare a pasului */
		{
			v=p;
			ant2=ant;
			for(;v;ant2=v,v=v->urm) 
				if(pas(v)>pas(r) && prioritate(v)==prioritate(r)) 
					break;
			if(v!=NULL)
			{															
				p=v;
				ant=ant2;
				break;
			}
	
		/* daca niciunul din cazurile de mai sus nu are loc, atunci cautarea trebuie sa 
		   continue pana unul din ele va avea loc */
		}
	}
	
	/* inserarea se face exact ca la curs */
	if(ant==NULL) 
	{
		r->urm=p;
		*aL=r;
	}
	else
	{
		r->urm=p;
		ant->urm=r;
	}
	return 1;
}


/* functii responsabile de comenzile primite in input */

/* functia end */
TLG end(ALG ap, int ID)
/* functia cauta intr-o lista informatia cu ID-ul "ID" si o sterge */
/* stergerea se realizeaza mutand elementele intr-o stiva auxiliara pana 
   la elementul cautat, stergerea acestuia, si apoi reintroducerea din
   elementelor ordonat in coada initiala */
/* returneaza coada rezultata dupa stergere */
{
	TLG p=*ap;
	TLG aux=NULL;
	TLG stivaaux=NULL;

	while(p!=NULL) 
	{
		aux=Pop(&p); /* se extrage celula */
		if(ID(aux)==ID)	/* daca e cea cautata, o distrug */ 
		{
			free(aux->info);
			free(aux);
			break;
		}
		Push(&stivaaux,aux); /* daca nu, o adaug in stiva si merg mai departe */
	}
	
	while(stivaaux!=NULL)
	{
		aux=Pop(&stivaaux); /* extrag celula */
		InsQ(&p,aux); /* reintroduc celula in coada */
	}

	return p;
}

/* functia wait */
TLG wait(int ID, ALG coada, ALG stiva)
/* functia primeste ca parametri ID-ul celulei ce trebuie sa intre in asteptare,
   coada in care trebuie cautata, si stiva in care va trebui mutata */ 
/* procedeu asemanator cu end, doar ca nu se elibereaza celula gasita,
   ci se muta in stiva data ca parametru */
{
	TLG p=*coada;
	TLG stivaaux=NULL;
	TLG aux=NULL;
	while (p!=NULL)
	{
		aux=Pop(&p); 
		if(ID(aux)==ID) 
		{
			Push(stiva,aux);
			break;
		}
		Push(&stivaaux,aux);
	}
	while(stivaaux!=NULL)
	{
		aux=Pop(&stivaaux);
		InsQ(&p,aux);
	}
	return p;
}

/* functia event */
void event(ALG stiva, ALG coada)
/* primeste ca parametri o stiva si o coada si insereaza varful stivei
   in coada, ordonat */
{
	TLG m=NULL;
	m=Pop(stiva);
	InsQ(coada,m);
}

int main(int argc, char *argv[])
{
	/* declararea variabilelor */
	TLG coada=NULL;
	TLG aux=NULL;
	TProces ae;
	int evenimente;
	int i,j;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int timestamp=1;
	int pas=0;
	char* par1;
	char* par2;
	FILE *input;
	FILE *output;
	input=fopen(argv[1],"rt");
	output=fopen(argv[2],"wt");

	fscanf(input,"%d",&evenimente); /* citire numar evenimente de pe primul rand */
	TLG stiva[evenimente+1];
	for(i=0;i<=evenimente; i++) 
		stiva[i]=NULL;
	
	while ((read = getline(&line, &len, input)) != -1)
	/* functie de citire rand cu rand */
	/* sursa: http://stackoverflow.com/questions/3501338/c-read-file-line-by-line */
	{
		if(line[0]=='s')   // start
		{
			par1 = calloc(6,sizeof(char)); /* reinitializare stringuri parametri */
			par2 = calloc(6,sizeof(char));
        	i=6;
        	while(line[i]!=' ')
        	/* citirea primului parametru se face de la al 7-lea caracter la spatiu */
        	{
        		par1[i-6]=line[i];
        		i++;
        	}
        	i++;
      		j=i;
        	while(line[i]!='\0' && line[i]!='\n')
        	/* citirea celui de-al doilea parametru se face de la spatiu la sfarsitul
        	de linie */ 
        	{
        		par2[i-j]=line[i];
        		i++;
        	}
        	ae.ID=atoi(par1); /* atoi(x) transforma un string intr-un numar */                           
			ae.prioritate=atoi(par2);
			ae.pas=timestamp;
			aux = (TLG)calloc(1,sizeof(TCelulaG));
			if(!aux) return 0;
			aux->info = (TProces*)calloc(1,sizeof(TProces));
			if(!aux->info) 
			{
				free(aux);
				return 0;
			}
			memcpy(aux->info, &ae, sizeof(TProces));
			InsQ(&coada,aux); /* adaugare in coada */ 
			timestamp++; /* timestamp reprezinta pasul la care ma aflu */
			free(par1);
			free(par2);
		}

		if(line[1]=='n')	 // end
		{
			par1=calloc(6,sizeof(char));
        	i=4;
        	while(line[i]!='\0' && line[i]!='\n') 
			/* se procedeaza analog cu start */
			{
        		par1[i-4]=line[i];
        		i++;
        	}
        	coada=end(&coada,atoi(par1));
        	free(par1);
		}

		if(line[0]=='w')      // wait
		{
			par1 = calloc(6,sizeof(char)); //reinitializarea cu valori nule a unui string
			par2 = calloc(6,sizeof(char));
        	i=5;
        	while(line[i]!=' ') 
        	/* analog cu start */
        	{
        		par1[i-5]=line[i];
        		i++;
        	}
        	i++;
      		j=i;
        	while(line[i]!='\0' && line[i]!='\n') 
        	{
        		par2[i-j]=line[i];
        		i++;
        	}
        	coada=wait(atoi(par2),&coada,&stiva[atoi(par1)]);
			free(par1);
			free(par2);
		}

		if(line[1]=='v')      // event
		{
			par1=calloc(6,sizeof(char));
        	i=6;
        	while(line[i]!='\0' && line[i]!='\n') 
			/* analog cu start */
			{
        		par1[i-6]=line[i];
        		i++;
        	}
           	while(stiva[atoi(par1)]!=NULL)
           	/* se goleste stiva iterativ deoarece functia event muta cate o singura celula */
				event(&stiva[atoi(par1)],&coada);
			free(par1);
		}
		
		/* afisare cu format pentru output dupa cerinta: */
		if(line[1]<=122 && line[1]>=48)
		{
			if(pas!=0) fprintf(output,"%d\n",pas);
			if (coada==NULL && pas!=0) fprintf(output,"\n");
			AfisareCoada(&coada,output);
			for (i=0;i<=evenimente-1;i++)
				if(stiva[i]!=NULL)
				{
					fprintf(output,"%d: ",i);
					AfisareCoada(&stiva[i],output);
				}
			if(pas!=0) fprintf(output,"\n");
		}
		pas++;
	}

	/* eliberari de memorie */
	DistrugereLG(coada);
	for(i=0;i<=evenimente;i++)
		DistrugereLG(stiva[i]);
	free(line);
	fclose(input);
	fclose(output);
	return 1; 
}

