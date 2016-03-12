function [] = matrixCipher
fid1=fopen('input1A','r');
strlit = fgetl(fid1); %citire string (in litere)
l=length(strlit);
fid2=fopen('key1A','r');
v=fscanf(fid2,'%d'); %citire valori intregi din key1A (pe toate)
n=v(1); %memorare prima valoare - n-ul
for i=1:mod(n-mod(l,n),n)
    strlit(1+length(strlit))=' '; %adaugare padding de spatii la sfarsitul stringului
end
%%transformarea stringului din litere in numere de la 0 la 29 (strlit -> strnum):
for i=1:length(strlit)
    if(double(strlit(i))==32) strnum(i)=0;
    else
        if(double(strlit(i))==39) strnum(i)=28;
        else
            if(double(strlit(i))==46) strnum(i)=27;
            else
                if(double(strlit(i))<91 && double(strlit(i))>64) strnum(i)=double(strlit(i))-64;
                else
                    strnum(i)=double(strlit(i))-96;
                end
            end    
        end
    end    
end
%%eliminarea primei valori (adica a n-ului din vectorul v):
for i=1:length(v)-1 
    v(i)=v(i+1);
end
v(length(v))=[];
%%rearanjarea elementelor matricei: (practic asa se citeste matricea, doar ca prima valoare citita, adica n-ul
%%incurca acest proces, asa ca n-am folosit dlmread, de exemplu)
l=length(strnum);
K=reshape(v,n,n);
%%aux va fi formata din l/n linii de n elemente a vectorului strnum, urmand ca cifrarea sa se realizeze prin inmultirea
%%matricei-cheie cu fiecare dintre liniile din aux, cifrandu-se astfel cate n elemente odata
aux=reshape(strnum,n,l/n);
aux=transpose(aux);
for i=1:l/n
    p=aux(i,:);
    p=transpose(p);
    A(:,i)=K'*p;
end
%%reorganizarea codului incifrat intr-un vector care urmeaza a fi transformat din numere in litere si simboluri
R=reshape(A,1,l); %https://www.mathworks.com/matlabcentral/newsreader/view_thread/294541
R=mod(R,29);
r=length(R);
for i=1:r
    if R(i)==0 O(i)=' ';
    else 
        if R(i)==27 O(i)='.';
        else if R(i)==28 O(i)=''''; %http://stackoverflow.com/questions/14984804/adding-apostrophe-in-a-string-matlab
            else
		switch(R(i))
			case 1
                        O(i)='a';
                    case 2
                        O(i)='b';
                    case 3
                        O(i)='c';
                    case 4
                        O(i)='d';
                    case 5
                        O(i)='e';
                    case 6
                        O(i)='f';
                    case 7
                        O(i)='g';
                    case 8
                        O(i)='h';
                    case 9
                        O(i)='i';
                    case 10
                        O(i)='j';
                    case 11
                        O(i)='k';
                    case 12
                        O(i)='l';
                    case 13
                        O(i)='m';
                    case 14
                        O(i)='n';
                    case 15
                        O(i)='o';
                    case 16
                        O(i)='p';
                    case 17
                        O(i)='q';
                    case 18
                        O(i)='r';
                    case 19
                        O(i)='s';
                    case 20
                        O(i)='t';
                    case 21
                        O(i)='u';
                    case 22
                        O(i)='v';
                    case 23
                        O(i)='w';
                    case 24
                        O(i)='x';
                    case 25
                        O(i)='y';
                    case 26
                        O(i)='z';
                    otherwise
                        disp('wtf');
		 end
            end
        end
    end
end
%%printare in fisier a vectorului final rezultat, O
fid3=fopen('output1A','w');
for i=1:length(O)
	fputs(fid3,O(i));
end
end

