/* APOSTOAE Andrei – 311CB */
#include <stdlib.h>
#include <stdio.h>
#include "functii.h"

int main(int argc, char *argv[])
{
	/* declaratii variabile */

	int N; /* numar drepte furnizate */
	int M; /* numar puncte de identificare */
	int Q; /* numar puncte interogare */
	TArb arb; /* arborele cu care lucrez */
	FILE *input;
	FILE *output;
	input=fopen(argv[2],"rt");
	output=fopen(argv[3],"wt");
	int i,x,y; /* auxiliare */
	coord *dr; /* vectorul de structuri ce contine coordonatele dreptelor */
	coord *pct; /* vectorul de structuri ce contine coordonatele punctelor, iar pe a 3-a pozitie 0 */
	coord noduri[32];
	int regiune; /* va reprezenta regiunea in care se afla fiecare punct */


	/* citire din fisier si alocari dinamice pentru *dr si *pct */
	
	fscanf(input,"%d",&N);
	dr=calloc(N,sizeof(coord));
	if(!dr) return 0;
	for(i=1;i<=N;i++)
	{
		fscanf(input,"%d",&dr[i-1].a);
		fscanf(input,"%d",&dr[i-1].b);
		fscanf(input,"%d",&dr[i-1].c);
	}

	fscanf(input,"%d",&M);
	pct=calloc(M,sizeof(coord));
	if(!pct) return 0;
	for(i=1;i<=M;i++)
	{
		fscanf(input,"%d",&pct[i-1].a);
		fscanf(input,"%d",&pct[i-1].b);
		pct[i-1].c=0;
	}
 
	
	/* alocarea primei drepte in arbore */

	arb=calloc(1,sizeof(TNod));
	if(!arb) return 0;
	arb->tip='d';
	arb->indice=0;
	arb->dreapta=dr[0];
	

	/* distinctia modurilor de lucru */

	if(atoi(argv[1])==2)
	/* modul 2 de lucru */
	/* se citeste numarul punctelor de interogare, se introduc dreptele in afara de prima 
	   in arbore, se introduc si puntele (regiunile), dupa care se filleaza cu null-uri
	   acolo unde dreptele nu au doi subarbori */
	{
		fscanf(input,"%d",&Q);
		for(i=1;i<=N-1;i++)
			inserare_mod2(arb,dr[i],i);
		for(i=0;i<=M-1;i++)
			adauga_punct(&arb,pct[i].a,pct[i].b,i);
		fill(&arb);
	}
	else
	{
		/* modul 1 de lucru */
		/* nestiindu-se numarul de informatii ce trebuie citit, voi citi pana cand gasesc
		   un element mai mare decat N, adica Q (numarul punctelor de interogare) */
		noduri[0].a=1;
		i=0;
		int nr_noduri;
		fscanf(input,"%d",&noduri[0].a);
		fscanf(input,"%d",&noduri[0].b);
		fscanf(input,"%d",&noduri[0].c);
		while(noduri[i].a<=N-1 && noduri[i].a>=0)
		{
			i++;
			fscanf(input,"%d",&noduri[i].a);
			if(noduri[i].a>=N) 
			{
				Q=noduri[i].a;
				nr_noduri=i;
				break;
			}
			fscanf(input,"%d",&noduri[i].b);
			fscanf(input,"%d",&noduri[i].c);
		}

		/* pentru prima dreapta se verifica daca are nod in stanga sau/si nod in dreapta */
		if(noduri[0].b==1) 
		{
			arb->st=calloc(1,sizeof(TNod));
			if(!arb->st) return 0;
			arb->st->tip='x';
			arb->st->indice=0;
			arb->st->dreapta=dr[0];
		}

		if(noduri[0].c==1)
		{
			arb->dr=calloc(1,sizeof(TNod));
			if(!arb->dr) return 0;
			arb->dr->tip='x';
			arb->dr->indice=0;
			arb->dr->dreapta=dr[0];
		} 
		
		/* restul dreptelor se vor insera in arbore	tinand cont de ce fel au nodurile in stanga
		   si in dreapta, dupa care adauga punctele, iar apoi se filleaza ca la modul 2 */
		for(i=1;i<=nr_noduri-1;i++)
			inserare_mod1(&arb, // arborele in care se insereaza
							noduri[i].b, // existenta fiului stang
								noduri[i].c, // existenta fiului drept
									dr[noduri[i].a], // informatia dreptei de inserat
										noduri[i].a); // indicele dreptei
		for(i=0;i<=M-1;i++)
			adauga_punct(&arb,pct[i].a,pct[i].b,i);
		fill(&arb);
	}

	printare(&arb,output); /* functie de printare in fisier a parcurgerii postordine */
	fprintf(output,"\n");
	
	
	/* pentru fiecare punct de interogare se citeste x si y, se parcurge arborele si se obtine 
	   regiunea, care ulterior este afisata in fisier */
	
	for(i=0;i<Q;i++)
	{
		fscanf(input,"%d",&x);
		fscanf(input,"%d",&y);
		regiune=cauta(arb,x,y);
		fprintf(output,"%d\n",regiune);
	}


	/* eliberari de memorie */

	free(pct);
	free(dr);
	fclose(input);
	fclose(output);
	distruge(arb); 

	return 1;
}