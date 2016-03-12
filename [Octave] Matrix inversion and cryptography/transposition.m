function [] = transposition
fid1=fopen('input1C','r');
fid3=fopen('output1C','w');
w=fscanf(fid1,'%d');
n=w(2); %n
k=w(1); %k-ul initial
strlit=fgetl(fid1);
g=length(strlit);
l=length(strlit);
rest=mod(l,1000);
mii=(l-rest)/1000; %variabila mii reprezinta numarul miilor de caractere din fisier (cu alte cuvinte iteratiile k-ului)
for i=1:mod(n-mod(l,n),n) %bordare
    strlit(1+length(strlit))=' ';
end
%%transformarea sirului de caractere in sir de numere (strlit->strnum)
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
l=length(strnum);
t=0;
%%transformarea propriu-zisa are loc aici - procedeul este descris in detaliu in README
for i=1:mii
	A=zeros(n+1,n+1);
	M=generate(n,k+i-1); %matricea cheie - vezi subprogram
	%%vectorul u retine cate 1000 de caractere la fiecare iteratie a i-ului
	for j=1:1000
		u(j)=strnum(j+(i-1)*1000);
	end
	sss=1000;
	%%bordare pentru fiecare mie:
	while mod(sss,n)~=0
		u(sss+1)=0;
		sss++;
	end
	p=reshape(u,n,length(u)/n);
	p=transpose(p);
	for j=1:l/n
    		p(j,n+1)=1;
	end
	%cifrare asemanatoare ca la matrixCipher, dar cu matricea cheie determinata de mine
	for j=1:length(u)/n
		q=p(j,:);
		U=(M*q')';
		U=mod(U,29);
		for v=1:n+1
			A(v,j)=U(v);
		end
	end
	A=transpose(A);
	A(:,n+1)=[];
	A=transpose(A);
	R=reshape(A,1,numel(A));
	R=transpose(R);
	%%selectare a doar primelor 1000 de caractere (in afara de cele bordate) care sunt trecute in vectorul final de caractere
	for j=1:1000
		final(t+j)=R(j);
	end
	t=t+1000;	
end
for i=1:1000
	u(i)=0;
end
for i=1:rest
	u(i)=strnum(i+1000*mii);
end
%%lucrul pe ultima grupa de caractere (numar mai mic ca 1000) se afla aici:
M=generate(n,k+mii);
l=length(u);
p=reshape(u,n,length(u)/n);
p=transpose(p);
for j=1:l/n
    	p(j,n+1)=1;
end
for j=1:length(u)/n
	q=p(j,:);
	U=(M*q')';
	U=mod(U,29);
	for v=1:n+1
		A(v,j)=U(v);
	end
end
A=transpose(A);
A(:,n+1)=[];
A=transpose(A);
R=reshape(A,1,numel(A));
%%adaugarea ultimului grup de caractere la finalul vectrului final
for j=1:rest
	final(t+j)=R(j);
end
%%transformare inapoi din numere in litere (strnum -> strlit)
for i=1:g
    if final(i)==0 O(i)=' ';
    else 
        if final(i)==27 O(i)='.';
        else if final(i)==28 O(i)=''''; %http://stackoverflow.com/questions/14984804/adding-apostrophe-in-a-string-matlab
            else
		switch(final(i))
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
%%afisare in fisier
for i=1:length(O)
	fputs(fid3,O(i));
end
end

function [M] = generate(n,k) %M=matricea cheie - detalii in README
for i=1:n+1
	for j=1:n+1
		M(i,j)=0;
	end
end
for i=1:n+1
	M(i,i)=1;
end
for i=1:n
	M(i,n+1)=k;
end
end

