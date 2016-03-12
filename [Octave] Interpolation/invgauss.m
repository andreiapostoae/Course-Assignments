function b = invgauss(a)
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