function []=strassen(input,output)
	%desi nu se specifica in enunt, pe forum am gasit ca inputul si outputul se dau ca parametri
	fid1=fopen(input,'r');
	%%se citeste n-ul si se elimina din vectorul de elemente:
	v=fscanf(fid1,'%f');
	N=v(1);
	for i=1:length(v)-1 
    v(i)=v(i+1);
	end
	v(length(v))=[];
	%%se formeaza matricea A din elementele citite
	A=reshape(v,sqrt(length(v)),sqrt(length(v)));
	A=transpose(A);
	B=strinv(A); %se inverseaza matricea A
	C=expsquare(B,N); %se ridica la putere rezultatul in timp logaritmic
	dlmwrite(output,C,' ') %se afiseaza matricea finala
end

function [D]=strmult(A,B)
	%http://www.mathworks.com/matlabcentral/fileexchange/2360-the-matrix-computation-toolbox/content/matrixcomp/strassen.m
	%%se adauga padding pana cand matricele au dimensiuni 2^n*2^n astfel incat matricele sa poata fi inmultite cu Strassen
	na=size(A);
	nb=size(B);
	npada=2^nextpow2(max(na));
	npadb=2^nextpow2(max(nb));
	if(npada>npadb) q=npada;
	else q=npadb;
	end
	if min(na)~=q
		A(q,q)=0;
	end
	if min(nb)~=q
		B(q,q)=0;
	end
	if max(na)<=32 %conform postarilor de pe forum, pentru dimensiuni mai mici sua egale cu 32, putem aplica inmultire directa
		C=A*B;
	else
	%%aici este algoritmul de multiplicare Strassen propriu-zis
   		m=npada/2;
   		i=1:m; 
   		j=m+1:npada;
   		P1=strmult(A(i,i)+A(j,j),B(i,i)+B(j,j));
   		P2=strmult(A(j,i)+A(j,j),B(i,i));
   		P3=strmult(A(i,i),B(i,j)-B(j,j));
   		P4=strmult(A(j,j),B(j,i)-B(i,i));
   		P5=strmult(A(i,i)+A(i,j),B(j,j));
   		P6=strmult(A(j,i)-A(i,i),B(i,i)+B(i,j));
   		P7=strmult(A(i,j)-A(j,j),B(j,i)+B(j,j));
    	C=[P1+P4-P5+P7 P3+P5; P2+P4 P1+P3-P2+P6 ];
	end
	D=C(1:na(1),1:nb(2)); %matricea finala va contine si elemente de padding, care nu trebuiesc intoarse
end

function [B]=strinv(A)
	%http://cs.curs.pub.ro/2014/pluginfile.php/12165/mod_resource/content/0/lab_mn_02.pdf
	n=length(A);
	if(n<=32) %conform forumului, pentru n<=32 putem folosi alt algoritm pentru a salva timp
		B=invgauss(A); %am implementat inversa folosind Gauss-Jordan
	else
	%%aici este algoritmul de inversare prin partitionare propriu zis, luat din laboratorul 2:
		if mod(n,2)==1
			i=(n+1)/2;
		else
			i=n/2;
		end
		M=A(1:i,1:i);
		N=A(i+1:n,i+1:n);
		while (det(M)==0 || det(N)==0)
			i=i+1;
			M=A(1:i,1:i);
			N=A(i+1:n,i+1:n);
		end
		Q=A(i+1:n,1:i);
		P=A(1:i,i+1:n);
		X1=strinv(M-strmult(strmult(P,strinv(N)),Q));
		X4=strinv(N-strmult(strmult(Q,strinv(M)),P));
		X2=(-1)*strmult(strmult(strinv(N),Q),X1);
		X3=(-1)*strmult(strmult(strinv(M),P),X4);
		B=[X1 X3; X2 X4];
	end
end

function [B]=expsquare(A,n)
	%http://en.wikipedia.org/wiki/Exponentiation_by_squaring
	%am folosit aceasta functie pentru a executa ridicare la putere in timp logaritmic, iar inmultirile
	%sunt facute, conform cerintei, cu Strassen
	if (n==0) 
		B=eye(length(A));
	else 
		if(n==1) 
			B=A;
		else 
			if(mod(n,2)==0) 
				B=expsquare(strmult(A,A),n/2); %daca n este par, B=A^(n/2) - functie recursiva
			else 
				B=strmult(A,expsquare(strmult(A,A),(n-1)/2)); %daca n este impar, B=A*A^((n-1)/2) - functie recursiva
			end
		end
	end
end


function b = invgauss(a)
%aici am implementat singur algoritmul de inversare cu Gauss-Jordan
%desi il puteam lua pe cel de pe net, am preferat sa nu fac asta deoarece imi incetinea enorm programul
n=length(a);
aux=eye(n);
%%bordare a matricei cu eye(n) la dreapta
for i=1:n
	for j=1:n
		a(i,j+n)=aux(i,j);
	end
end
c=rref(a); %rref=reduced row echelon form, adica algoritmul de eliminare Gaussiana implentat in octave
%%alegerea valorilor din dreapta si intoarcerea lor
for i=1:n
	for j=1:n
		b(i,j)=c(i,j+n);
	end
end
end
