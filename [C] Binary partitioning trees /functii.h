/* APOSTOAE Andrei – 311CB */
#include <stdlib.h>
#include <stdio.h>

typedef struct 
{
    int a,b,c;
} coord;

typedef struct nod 
{ 
	coord dreapta;
	char tip;
	int indice;
	struct nod*st, *dr;
} TNod, *TArb, **AArb;

void distruge(TArb r); /* functia de distrugere arbore */

void inserare_mod2(TArb p,coord dr,int indice); /* functia de inserare a unei drepte
												   in arbore prin modul 2 */

void adauga_punct(TArb *a,int x,int y,int indice); /* functia de inserare a unui punct
													  in arbore pentru ambele moduri */

void printare(TArb *a, FILE *f); /* functia de printare in fisier (postordine) */

int cauta(TArb a,int x, int y); /* functia de gasire a regiune din care un punct
									face parte */

void fill(TArb *a); /* functia care completeaza subarborii nodurilor dreptelor cu null-uri */

int inserare_mod1(TArb *a,
					int nod_stang,
						int nod_drept,
							coord dreapta,
								int indice); /* functia de inserare a unei drepte in arbore
												prin modul 1 */
