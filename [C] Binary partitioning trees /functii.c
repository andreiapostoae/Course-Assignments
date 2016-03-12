/* APOSTOAE Andrei – 311CB */
#include "functii.h"

void distruge(TArb r)   /* functie auxiliara - distruge toate nodurile */
{ 
	if (!r) return;
	distruge (r->st);   /* distruge subarborele stang */
	distruge (r->dr);   /* distruge subarborele drept */
	free (r);           /* distruge nodul radacina */
}

void adauga_punct(TArb *a,int x,int y,int indice)
/* functia primeste ca parametri arborele in care trebuie facuta inserarea, coordonatele x si y 
   ale punctului de inserat si indicele (regiunea) pe care o reprezinta */
{
	TArb p=*a;
	int m=(*a)->dreapta.a;
	int n=(*a)->dreapta.b;
	int q=(*a)->dreapta.c;

	if(m*x+n*y+q>0 && p->dr!=NULL) /* daca ax+by+c>0, continua parcurgerea la dreapta, daca exista */
	{
		adauga_punct(&p->dr,x,y,indice);
	}

	if(m*x+n*y+q<0 && p->st!=NULL) /* daca ax+by+c<0, continua parcurgerea la stanga, daca exista */
	{
		adauga_punct(&p->st,x,y,indice);
	}

	if(m*x+n*y+q<0 && p->st==NULL && p->tip=='d') 
	/* daca ax+by+c<0, nodul nu are subarbore stang si nodul este reprezentat de o dreapta, atunci 
	   se aloca si insereaza punctul la stanga */
	{
		p->st=calloc(1,sizeof(TNod));
		if(!p->st) return;
		p->st->dreapta.a=x;
		p->st->dreapta.b=y;
		p->st->dreapta.c=0;
		p->st->tip='p';
		p->st->indice=indice;
	}

	if(m*x+n*y+q>0 && p->dr==NULL && p->tip=='d')
	/* daca ax+by+c?0, nodul nu are drept stang si nodul este reprezentat de o dreapta, atunci 
	   se aloca si insereaza punctul la dreapta */	
	{
		p->dr=calloc(1,sizeof(TNod));
		if(!p->dr) return;
		p->dr->dreapta.a=x;
		p->dr->dreapta.b=y;
		p->dr->dreapta.c=0;
		p->dr->tip='p';
		p->dr->indice=indice;
	}
}


void printare(TArb *a, FILE *f)
/* functia primeste ca parametri arborele ce trebuie printat si fisierul */
/* se parcurge arborele SDR (postordine) recursiv */
{
	TArb p=*a;
	if((*a)->st) printare(&p->st,f);
	if((*a)->dr) printare(&p->dr,f);
	if((*a)->indice==-1) 
		fprintf(f,"null"); /* afisarea pentru punctele care nu are atribuita o regiune */
	else
		if((*a)->tip=='p') 
		/* afisarea pentru punctele care au atribuita o regiune */	
			fprintf(f,"((%d,%d)-%d)",(*a)->dreapta.a,(*a)->dreapta.b,(*a)->indice);
		else
		/* afisarea pentru drepte */
			fprintf(f,"(%d,%d,%d)",(*a)->dreapta.a,(*a)->dreapta.b,(*a)->dreapta.c);
}


int cauta(TArb a,int x, int y)
/* functia cauta punctul de coordonate x si y in arborele dat ca parametru si returneaza
   indicele, adica regiunea */
{
	while(a->tip!='p')
	{
		int m=a->dreapta.a;
		int n=a->dreapta.b;
		int q=a->dreapta.c;
		if(m*x+n*y+q<0) 
			a=a->st;
		else 
			a=a->dr;
	}
	return a->indice;
}


void fill(TArb *a)
/* functia primeste ca parametru un arbore si completeaza recursiv dreptele care nu au ambii
   fii cu noduri ce reprezinta puncte de regiune -1, adica null-urile din problema */
{
	TArb p=*a;
	if(!p) return;
	if(p->tip=='d' && p->dr==NULL)
	{
		p->dr=calloc(1,sizeof(TNod));
		if(!p->dr) return;
		p->dr->dreapta.a=0;
		p->dr->dreapta.b=0;
		p->dr->dreapta.c=0;
		p->dr->indice=-1;
		p->dr->tip='p';
	}
	if(p->tip=='d' && p->st==NULL)
	{
		p->st=calloc(1,sizeof(TNod));
		if(!p->st) return;
		p->st->dreapta.a=0;
		p->st->dreapta.b=0;
		p->st->dreapta.c=0;
		p->st->indice=-1;
		p->st->tip='p';
	}
	fill(&p->st); /* recursivitate la stanga */
	fill(&p->dr); /* recursivitate la dreapta */
}

int inserare_mod1(TArb *a,
					int nod_stang,
						int nod_drept,
							coord dreapta,
								int indice)
/* functia primeste ca parametri arborele in care se face inserarea, existenta nodului stang,
   existenta nodului drept, coordonatele dreptei ce trebuie inserata si indicele acesteia */
/* modul de lucru al functiei este descris detaliat in README */
{
	TArb p=*a;
	int succes=0;
	if(!p) return 0;

	if(p->tip=='d')
	{
		succes=inserare_mod1(&p->st,nod_stang,nod_drept,dreapta,indice);
		if(succes==1) 
			return succes; /* odata ce inserarea s-a realizat, se face iesirea din functie */
		succes=inserare_mod1(&p->dr,nod_stang,nod_drept,dreapta,indice);
		if(succes==1) 
			return succes; /* odata ce inserarea s-a realizat, se face iesirea din functie */
	}

	if(p->tip=='x') /* adica nodul curent este un placeholder - vezi README */
	{
		p->dreapta=dreapta;
		p->indice=indice;
		p->tip='d'; /* nodul devine dreapta, inserandu-se coordonatele corecte */

		if(nod_stang==1)
		{
			p->st=calloc(1,sizeof(TNod));
			if(!p->st) return 0;
			p->st->tip='x'; /* daca nod_stang este 1, atunci la stanga se insereaza un alt placeholder */
			p->st->dreapta=dreapta;
			p->st->indice=0;
		}

		if(nod_drept==1)
		{
			p->dr=calloc(1,sizeof(TNod));
			if(!p->dr) return 0;
			p->dr->tip='x'; /* daca nod_drept este 1, atunci la dreapta se insereaza un alt placeholder */
			p->dr->dreapta=dreapta;
			p->dr->indice=0;
		}

		succes=1;
		return succes;
	}
	return succes;
}

void inserare_mod2(TArb p,coord dr,int indice)
/* functia primeste ca parametri arborele in care dreapta trebuie inserata, coordonatele si indicele acesteia */
/* neavand informatii despre fii, acestia vor trebui dedusi din paralelism */
{
	coord dr2=p->dreapta;
	int k=dr.a*dr2.b-dr.b*dr2.a; /* k=0 este echivalent cu propozitia "dreptele sunt paralele" */
	if(p->st==NULL || p->dr==NULL) // cand cel putin unul din subarborbori nu exista
	{
		if(k!=0) // adica dr si dr2 nu sunt paralele
		{
			if(!p->st) // daca arborele din stanga nu exista, va fi alocat si introdus
			{
				p->st=calloc(1,sizeof(TNod));
				if(!p->st) return;
				p->st->dreapta=dr;
				p->st->tip='d';
				p->st->indice=indice;
			}
			if(!p->dr) // daca arborele din dreapta nu exista, va fi alocat si introdus
			{
				p->dr=calloc(1,sizeof(TNod));
				if(!p->dr) return;
				p->dr->dreapta=dr;
				p->dr->tip='d';
				p->dr->indice=indice;
			}
		}

		if(k==0 && dr.c>dr2.c) /* daca dreptele sunt paralele, iar dreapta introdusa se afla la dreapta 
								  dreptei curente */
		{
			if(!p->st) // daca arborele din stanga nu exista, va fi alocat si introdus
			{	
				p->st=calloc(1,sizeof(TNod));
				if(!p->st) return;
				p->st->dreapta=dr;
				p->st->tip='d';
				p->st->indice=indice;
			}
			else
				inserare_mod2(p->st,dr,indice); // daca exista, atunci se introduce la stanga in mod recursiv
		}

		if(k==0 && dr2.c>dr.c) /* daca dreptele sunt paralele, iar dreapta introdusa se afla la stanga
								  dreptei curente */
		{
			if(!p->dr) // daca arborele din stanga nu exista, va fi alocat si introdus
			{
				p->dr=calloc(1,sizeof(TNod));
				if(!p->dr) return;
				p->dr->dreapta=dr;
				p->dr->tip='d';
				p->dr->indice=indice;
			}
			else
				inserare_mod2(p->dr,dr,indice); // daca exista, atunci se introduce la dreapta in mod recursiv
		}
	}

	/* recursivitate pentru fiecare caz, atunci cand nodul curent are ambii fii */

	if(k!=0) 
	{
		inserare_mod2(p->st,dr,indice);
		inserare_mod2(p->dr,dr,indice);
	}

	if(k==0 && dr.c>dr2.c)
		inserare_mod2(p->st,dr,indice);

	if(k==0 && dr.c<dr2.c)
		inserare_mod2(p->dr,dr,indice);
}

