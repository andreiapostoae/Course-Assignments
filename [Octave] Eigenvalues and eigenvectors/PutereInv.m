function [valp vecp] = PutereInv(d, s, h, y, maxIter, eps)
y_old=y;
z=Thomas(s,d-h,s,y_old);
z=transpose(z);
y_new=z/norm(z);
lambda=inmultire1(transpose(y_new),s,d)*y_new;
iter=1;
V2=lambda*transpose(y_new);
V1=inmultire2(y_new,s,d);
R=V1-V2;
while(iter<=maxIter && norm(R)>eps)
	h=lambda;
	y_old=y_new;
	z=Thomas(s,d-h,s,y_old);
	z=transpose(z);
	y_new=z/norm(z);
	lambda=inmultire1(transpose(y_new),s,d)*y_new;
	iter=iter+1;
	V2=lambda*transpose(y_new);
	V1=inmultire2(y_new,s,d);
	R=V1-V2;
end
valp=lambda;
vecp=y_new;
end




function x = Thomas(a,b,c,d)
	n = length(d);
	
	% transform a astfel incat sa aiba n elemente, cu primul element 0
 	a = [0 a];
 
	% Operariile la limita;
	c(1) = c(1) / b(1); 
	d(1) = d(1) / b(1);   
 	
 	% calculul coeficientilor pe caz general.
	for i = 2:n-1
    	temp = b(i) - a(i) * c(i-1);
    	c(i) = c(i) / temp;
    	d(i) = (d(i) - a(i) * d(i-1))/temp;
	end
	d(n) = (d(n) - a(n) * d(n-1))/(b(n) - a(n) * c(n-1));
 
	% Substitutia inapoi pentru rezolvarea sistemului de ecuatii
	x(n) = d(n);
	for i = n-1:-1:1
    	x(i) = d(i) - c(i) * x(i + 1);
	end
end

function a = inmultire1(k,s,d) %vector*matrice
n=length(d);
a(1)=k(1)*d(1)+k(2)*s(1);
a(n)=k(n-1)*s(n-1)+k(n)*d(n);
for i=2:n-1
	a(i)=k(i)*d(i)+s(i-1)*k(i-1)+s(i)*k(i+1);
end
end

function a = inmultire2(k,s,d) %matrice*vector
n=length(d);
a(1)=d(1)*k(1)+s(1)*k(2);
a(n)=s(n-1)*k(n-1)+d(n)*k(n);
for i=2:n-1
	a(i)=s(i-1)*k(i-1)+d(i)*k(i)+s(i)*k(i+1);
end
end
