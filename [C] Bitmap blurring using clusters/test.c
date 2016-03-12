#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Informs the compiler to not add padding for these structs
#pragma pack(1)

struct bmp_fileheader
{
    unsigned char  fileMarker1; /* 'B' */
    unsigned char  fileMarker2; /* 'M' */
    unsigned int   bfSize; /* File's size */
    unsigned short unused1;
    unsigned short unused2;
    unsigned int   imageDataOffset; /* Offset to the start of image data */
};

struct bmp_infoheader
{
    unsigned int   biSize; /* Size of the info header - 40 bytes */
    signed int     width; /* Width of the image */
    signed int     height; /* Height of the image */
    unsigned short planes;
    unsigned short bitPix;
    unsigned int   biCompression;
    unsigned int   biSizeImage; /* Size of the image data */
    int            biXPelsPerMeter;
    int            biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biClrImportant;
};

#pragma pack()

struct pixel{
	unsigned char B;
	unsigned char G;
	unsigned char R;
	};

struct map{
	char **BB;
	char **GG;
	char **RR;
	};

int bun(struct pixel pver,struct pixel pref,struct pixel poff){//verificare, referinta, offset; calculeaza daca un pixel e "bun"
	if(pref.R-poff.R<=pver.R && pver.R<=pref.R+poff.R && pref.G-poff.G<=pver.G && pver.G<=pref.G+poff.G && pref.B-poff.B<=pver.B && pver.B<=pref.B+poff.B) return 1;
	else return 0;
	}

int min(int a, int b){
	if(a<b) return a;
		else return b;
	}

int main(void){
	//declaratii variabile:
	FILE *img;
	struct pixel pix_ref;
	struct pixel pix_off;
	double P;
	img=fopen("input.bmp","rb");
	FILE *input;
	input=fopen("input.txt","rt");
	struct bmp_fileheader file_main;
	struct bmp_infoheader info_main;

	//citire din fisierul input.txt:
	fscanf(input,"%hhu %hhu %hhu",&pix_ref.R,&pix_ref.G,&pix_ref.B);
	fscanf(input,"%hhu %hhu %hhu",&pix_off.R,&pix_off.G,&pix_off.B);
	fscanf(input,"%lf",&P);
	fclose(input); //se termina lucrul cu fisierul input.txt

	fread(&file_main,sizeof(struct bmp_fileheader),1,img); //citire date corespunzatoare headerelor din fisier
	fread(&info_main,sizeof(struct bmp_infoheader),1,img);
	fseek(img,file_main.imageDataOffset,0);
	int h=info_main.height;
	int w=info_main.width;
	struct pixel pixels[h+1][w+1];
	int i,j;
	for(i=h;i>=1;i--)
		{for(j=1;j<=w;j++)
		      fread(&pixels[i][j],sizeof(struct pixel),1,img);
		fseek(img,8*(w%4),1);} //creare matrice de pixeli
	fclose(img); //lucrul cu fisierul binar input.bmp se termina
	int **a; //matricea auxiliara (vezi readme)
	a=malloc((h+1)*sizeof(int *));
	for(i=1;i<=h;i++) a[i]=calloc(w+1,sizeof(int));
	int c,changes;
	c=1;
	for(i=1;i<=h;i++)
		for(j=1;j<=w;j++) if(bun(pixels[i][j],pix_ref,pix_off)) {a[i][j]=c;
									c++;}
	changes=1;
	while(changes!=0)
	   {changes=0;
	    for(i=1;i<=h;i++)
		for(j=1;j<=w;j++) if(a[i][j]!=0) {if(a[i][j-1]!=0 && a[i][j]!=a[i][j-1]) {a[i][j-1]=min(a[i][j-1],a[i][j]);
								   a[i][j]=a[i][j-1];
								   changes++;}
						  if(a[i][j+1]!=0 && a[i][j]!=a[i][j+1]) {a[i][j+1]=min(a[i][j+1],a[i][j]);
								   a[i][j]=a[i][j+1];
                                                                   changes++;}
						  if(a[i-1][j]!=0 && a[i][j]!=a[i-1][j]) {a[i-1][j]=min(a[i-1][j],a[i][j]);
								   a[i][j]=a[i-1][j];
                                                                   changes++;}
						  if(a[i+1][j]!=0 && a[i][j]!=a[i+1][j]) {a[i+1][j]=min(a[i+1][j],a[i][j]);
								   a[i][j]=a[i+1][j];
                                                                   changes++;}
						  }
	   } //parcurgere matrice auxiliara pentru gasirea si notarea clusterelor
	int *v;
	v=calloc(3000000,sizeof(int)); //vectorul stocheaza numarul de elemente al fiecarui cluster
	for(i=1;i<=h;i++)
		for(j=1;j<=w;j++) if(a[i][j]!=0) v[a[i][j]]++;
	int *final;
	final=calloc(3000000,sizeof(int));
	c=1; int aux;
	for(i=1;i<=3000000;i++) if(v[i]>(P*w*h)) {final[c]=v[i];c++;} //vectorul final contine elementele vectorului v, dar care indeplinesc conditia de la punctul a
	for(i=1;i<=c-2;i++)
		for(j=i+1;j<=c-1;j++) if(final[i]<final[j]) {aux=final[i];
							     final[i]=final[j];
							     final[j]=aux;} //ordonare vector final
	FILE *outputtxt;
	outputtxt=fopen("output.txt","wt");
	int pozf;  
	c=0;
	for(i=1;i<=3000000;i++) if(final[i]==0 && c==0) {pozf=i;c=1;} //determinarea pozitiei finale pana la care trebuie afisate dimensiunile clusterelor
	for(i=pozf-1;i>=1;i--) fprintf(outputtxt,"%d ",final[i]);
	fclose(outputtxt); 
	free(final); //se termina subpunctul a
	
	
	c=0; 
	int k,z;
	int *r=calloc(100000,sizeof(int));
	for(i=1;i<=3000000;i++) if(v[i]>P*h*w) c++; //c=numarul clusterelor
	k=1;
	for(i=1;i<=h;i++)
		for(j=1;j<=w;j++) if(a[i][j]!=0 && v[a[i][j]]>P*h*w) {aux=0;
								      for(z=1;z<=1000;z++) if(r[z]==a[i][j]) aux=1;
								      if(aux==0) {r[k]=a[i][j];
								                  k++;}
								     } //r stocheaza fiecare numar de identificare a clusterelor
	k=1;
	FILE **outputcrop=malloc(sizeof(FILE *)*(c+1));
	char **outputnames=malloc(sizeof(char *)*(c+1));
	for(i=1;i<=c;i++) outputnames[i]=calloc(20,sizeof(char));
	char c1[]="output_crop";
	char c2[]=".bmp";
	int xmin,ymin,xmax,ymax;
	char **number=malloc(sizeof(char *)*(c+1));
	for(i=1;i<=c;i++) {number[i]=calloc(10,sizeof(char)); //creare siruri de caractere de forma output_crop1.bmp
	                   sprintf(number[i],"%d",i);
                           strcpy(outputnames[i],c1);
                           strcat(outputnames[i],number[i]);
	                   strcat(outputnames[i],c2);
		          }
	for(i=1;i<=c;i++) {outputcrop[i]=fopen(outputnames[i],"wb");}
	signed int w1,h1; 
	int size,size2; 
	int trash=0;
	for(i=1;i<=c;i++) {xmin=w;ymin=h;xmax=0;ymax=0;
			  for(j=1;j<=h;j++)
				for(k=1;k<=w;k++) if(a[j][k]==r[i]) {if(j>ymax) ymax=j; //determinare coordonate pt fiecare zona
								  if(k>xmax) xmax=k;
								  if(j<ymin) ymin=j;
								  if(k<xmin) xmin=k;}
			  w1=xmax-xmin+1; 
			  h1=ymax-ymin+1; 
			  size=3*h1*w1+54+h1*(w1%4); //bfSize
			  size2=3*h1*w1+h1*(w1%4); //biSizeImage
		          fwrite(&file_main.fileMarker1,sizeof(unsigned char),1,outputcrop[i]);
			  fwrite(&file_main.fileMarker2,sizeof(unsigned char),1,outputcrop[i]);
			  fwrite(&size,sizeof(int),1,outputcrop[i]);
			  fwrite(&file_main.unused1,sizeof(unsigned short),1,outputcrop[i]);
			  fwrite(&file_main.unused2,sizeof(unsigned short),1,outputcrop[i]);
		          fwrite(&file_main.imageDataOffset,sizeof(unsigned int),1,outputcrop[i]);
			  
			  fwrite(&info_main.biSize,sizeof(unsigned int),1,outputcrop[i]);
			  fwrite(&w1,sizeof(signed int),1,outputcrop[i]);
			  fwrite(&h1,sizeof(signed int),1,outputcrop[i]);
			  fwrite(&info_main.planes,sizeof(unsigned short),1,outputcrop[i]);
                          fwrite(&info_main.bitPix,sizeof(unsigned short),1,outputcrop[i]);
                          fwrite(&info_main.biCompression,sizeof(unsigned int),1,outputcrop[i]);
		          fwrite(&size2,sizeof(unsigned int),1,outputcrop[i]);
			  fwrite(&trash,sizeof(int),1,outputcrop[i]);
			  fwrite(&trash,sizeof(int),1,outputcrop[i]);
			  fwrite(&info_main.biClrUsed,sizeof(unsigned int),1,outputcrop[i]);
			  fwrite(&info_main.biClrImportant,sizeof(unsigned int),1,outputcrop[i]);//scrierea headerelor in fisierele croppate
			  for(j=ymax;j>=ymin;j--)
				{for(k=xmin;k<=xmax;k++) fwrite(&pixels[j][k],sizeof(struct pixel),1,outputcrop[i]);
				fwrite("\0\0\0\0",(w1%4),1,outputcrop[i]); //padding
				}
			  }
	for(i=1;i<=c;i++) {fclose(outputcrop[i]);
			  free(outputnames[i]);
			  free(number[i]);}  //se termina subpunctul c
	
			  
	z=0;	
	FILE *outputblur;
	outputblur=fopen("output_blur.bmp","wb");
	fwrite(&file_main,sizeof(struct bmp_fileheader),1,outputblur);
        fwrite(&info_main.biSize,sizeof(unsigned int),1,outputblur);
                      	  fwrite(&w,sizeof(signed int),1,outputblur);
                          fwrite(&h,sizeof(signed int),1,outputblur);
                          fwrite(&info_main.planes,sizeof(unsigned short),1,outputblur);
                          fwrite(&info_main.bitPix,sizeof(unsigned short),1,outputblur);
                          fwrite(&info_main.biCompression,sizeof(unsigned int),1,outputblur);
                          fwrite(&info_main.biSizeImage,sizeof(unsigned int),1,outputblur);
                          fwrite(&z,sizeof(int),1,outputblur);
                          fwrite(&z,sizeof(int),1,outputblur);
                          fwrite(&info_main.biClrUsed,sizeof(unsigned int),1,outputblur);
                          fwrite(&info_main.biClrImportant,sizeof(unsigned int),1,outputblur); //scrierea headerului in outputblur
	struct map mapblur; //harta de pixeli pentru imaginea blurata
	mapblur.BB=malloc(1502*sizeof(char *));
	mapblur.GG=malloc(1502*sizeof(char *));
	mapblur.RR=malloc(1502*sizeof(char *));
	for(i=0;i<=1502;i++) {mapblur.BB[i]=calloc(1502,sizeof(char));
			      mapblur.GG[i]=calloc(1502,sizeof(char));
			      mapblur.RR[i]=calloc(1502,sizeof(char));
			     }
	for(i=1;i<=h;i++)
                for(j=1;j<=w;j++) if(a[i][j]==0 || v[a[i][j]]<=P*h*w) {mapblur.RR[i][j]=pixels[i][j].R;
                                                                       mapblur.GG[i][j]=pixels[i][j].G;
                                                                       mapblur.BB[i][j]=pixels[i][j].B;
								      } 
	int blurs;
	int neighbours;
	for(blurs=1;blurs<=100;blurs++) //blurs=nr. proceselor de blurare
		{for(i=1;i<=h;i++)
		       for(j=1;j<=w;j++) 
				 if(a[i][j]!=0 && v[a[i][j]]>P*h*w)
					{neighbours=4; //trebuie numarati vecinii fiecarui pixel care va fi blurat
					if(j==w) neighbours--;
					if(j==1) neighbours--;
					if(i==1) neighbours--;
					if(i==h) neighbours--;
					mapblur.RR[i][j]=(pixels[i][j-1].R+pixels[i][j+1].R+pixels[i-1][j].R+pixels[i+1][j].R)/neighbours;
					mapblur.GG[i][j]=(pixels[i][j-1].G+pixels[i][j+1].G+pixels[i-1][j].G+pixels[i+1][j].G)/neighbours;
					mapblur.BB[i][j]=(pixels[i][j-1].B+pixels[i][j+1].B+pixels[i-1][j].B+pixels[i+1][j].B)/neighbours;
					}
		for(i=1;i<=h;i++)
			for(j=1;j<=w;j++)
				 {pixels[i][j].R=mapblur.RR[i][j];
				  pixels[i][j].G=mapblur.GG[i][j];
				  pixels[i][j].B=mapblur.BB[i][j];}
		}			 				
	for(i=h;i>=1;i--)
                {for(j=1;j<=w;j++)
                      {fwrite(&mapblur.BB[i][j],sizeof(unsigned char),1,outputblur);
		       fwrite(&mapblur.GG[i][j],sizeof(unsigned char),1,outputblur); 
		       fwrite(&mapblur.RR[i][j],sizeof(unsigned char),1,outputblur);}
                 fwrite("\0\0\0\0",w%4,1,outputblur);} 
	for(i=0;i<=h;i++) free(a[i]);
	for(i=0;i<=1502;i++) {free(mapblur.RR[i]);
			      free(mapblur.GG[i]);
			      free(mapblur.BB[i]);
			     }
	free(v);	
	return 0;
	}

	
	
	
