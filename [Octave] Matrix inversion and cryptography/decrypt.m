function [] = decrypt
fid1=fopen('input1B','r');
strlit = fgetl(fid1);
fid2=fopen('key1B','r');
v=fscanf(fid2,'%d');
%%la fel ca la matrixCipher, citim sirul de caractere si matricea-cheie, avand grija sa memoram in variabila n 
%%primul numar citit din fisierul key1B, adaugam padding de spatii la sfarsit si apoi reorganizam stringul intr-o matrice:
n=v(1); 
for i=1:mod(n-mod(length(strlit),n),n)
    strlit(1+length(strlit))=' ';
end
for i=1:length(v)-1 
    v(i)=v(i+1);
end
v(length(v))=[];
K=reshape(v,n,n);
K=transpose(K);
K=modinv(K,29); %functia de inversare modulo 29
%%transformare din litere in numere (strlit -> strnum)
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
l=length(strlit);
%%proces asemanator ca cel din matrixCipher prin utilizarea matricei aux si inmultirea matricei cheie cu fiecare dintre
%%liniile lui aux pentru a obtine cate n caractere descifrate odata
aux=reshape(strnum,n,l/n);
aux=transpose(aux);
for i=1:l/n
    p=aux(i,:);
    p=transpose(p);
    A(:,i)=K*p;
end
%%se realizeaza transformarea din litere in numere si afisarea:
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
fid3=fopen('output1B','w');
for i=1:length(O)
	fputs(fid3,O(i));
end
end

function b = modinv(a)
%%aceasta functie este de fapt algoritmul de inversare a unei matrice folosing Gauss-Jordan, modificat astfel incat
%%sa realizeze inversarea modulo 29
%%http://www.matrixlab-examples.com/matrix-inversion.html

r=length(a);
b = eye(r);

for j = 1 : r
    for i = j : r
        if a(i,j) ~= 0
            for k = 1 : r
                s = a(j,k); a(j,k) = a(i,k); a(i,k) = s;
                s = b(j,k); b(j,k) = b(i,k); b(i,k) = s;
            end
            t = invmod(a(j,j),29); %modificarea principala a codului este aici! mai multe detealii in README
            for k = 1 : r
                a(j,k) = mod(t * a(j,k),29); %am adaugat mod 29 la fiecare calcul pentru a usura outputul
                b(j,k) = mod(t * b(j,k),29); %procesul este optional deoarece se putea folosi functia de 
					     %mod pentru matricea de output, la sfarsit
            end
            for L = 1 : r
                if L ~= j
                    t = -a(L,j);
                    for k = 1 : r
                        a(L,k) = mod(a(L,k) + t * a(j,k),29);
                        b(L,k) = mod(b(L,k) + t * b(j,k),29);
                    end
                end
            end            
        end
        break
    end
end
end


function xInv = invmod(x,n)
%functia calculeaza inversul unui element din Zn (fata de inmultire, bineinteles)
%am pierdut sursa algoritmului, dar algoritmul e straight-forward oricum

if gcd(x,n) ~= 1
    error('x nu are inversa in Zn')
end

    [d, a, b]   = gcd(x,n); % ax+by = gcd(x,y), Bézout's identity
    xInv        = mod(a,n); 
end

