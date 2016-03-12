function [] = F3
fid=fopen('realmadrid.txt','r');
figure;
tline = fgetl(fid); %http://www.mathworks.com/matlabcentral/answers/71374-how-to-read-a-text-file-line-by-line
v=str2num(tline);
m=v(1); n=v(2); %citire m si n
tline=fgetl(fid);
while ischar(tline) %pentru fiecare set
	x=[]; % vectorul de valori x
	y=[]; % vectorul de valori y
	k=[]; % vectorul final de m valori x
	w=[]; % vectorul final de m valori y
	p=[]; % a-ul din formula Si(x)=a*(x-x0)^3+b*(x-x0)^2+c*(x-x0)+d
	r=[]; % b-ul din formula Si(x)=a*(x-x0)^3+b*(x-x0)^2+c*(x-x0)+d
	s=[]; % c-ul din formula Si(x)=a*(x-x0)^3+b*(x-x0)^2+c*(x-x0)+d
	t=[]; % d-ul din formula Si(x)=a*(x-x0)^3+b*(x-x0)^2+c*(x-x0)+d
	A=[]; % matricea inventata de mine
	b=[]; % coloana de solutii a sistemului Az=b
	z=[]; % coloana de rezultate (parametrii splineurilor)
	v=str2num(tline);
	len=length(v)/2;
	for i=1:len
		x(i)=v(i);
		y(i)=v(i+len);
	end;

	%% ORDONARE - vezi README
	while(1)
		changes=0;
		for i=1:length(x)-1
			for j=i+1:length(x)
				if(x(i)>x(j)) 
					aux=x(i); x(i)=x(j); x(j)=aux;
					aux=y(i); y(i)=y(j); y(j)=aux;
					changes=changes+1;
				end
			end
		end
		if changes==0 
			break;
		end
	end

	k=linspace(x(1),x(length(x)),m);
	
	%% FORMARE MATRICE A
	q=length(x);
	A=zeros(4*(q-1),4*(q-1));
	% primele n ecuatii
	for i=1:(q-1)
		A(i,4*(i-1)+4)=1;
	end
	% urmatoarele n ecuatii
	for i=1:(q-1)
		A(i+q-1,4*(i-1)+1)=(x(i+1)-x(i))^3;
		A(i+q-1,4*(i-1)+2)=(x(i+1)-x(i))^2;	
		A(i+q-1,4*(i-1)+3)=x(i+1)-x(i);
		A(i+q-1,4*(i-1)+4)=1;
	end
	for i=1:(q-2)
		% urmatoarele n-1 ecuatii
		A(i+2*q-2,4*(i-1)+1)=3*((x(i+1)-x(i))^2);
		A(i+2*q-2,4*(i-1)+2)=2*(x(i+1)-x(i));	
		A(i+2*q-2,4*(i-1)+3)=1;
		A(i+2*q-2,4*(i-1)+4)=0;
		A(i+2*q-2,4*(i-1)+5)=0;
		A(i+2*q-2,4*(i-1)+6)=0;
		A(i+2*q-2,4*(i-1)+7)=-1;
		A(i+2*q-2,4*(i-1)+8)=0;
		% urmatoarele n-1 ecuatii
		A(i+3*q-4,4*(i-1)+1)=6*(x(i+1)-x(i));
		A(i+3*q-4,4*(i-1)+2)=2;
		A(i+3*q-4,4*(i-1)+3)=0;
		A(i+3*q-4,4*(i-1)+4)=0;
		A(i+3*q-4,4*(i-1)+5)=0;
		A(i+3*q-4,4*(i-1)+6)=-2;
		A(i+3*q-4,4*(i-1)+7)=0;
		A(i+3*q-4,4*(i-1)+8)=0;
	end
	% ultimele doua ecuatii
	A(4*q-5,1)=0;
	A(4*q-5,2)=2;
	A(4*q-4,4*q-7)=6*(x(q)-x(q-1));
	A(4*q-4,4*q-6)=2;

	%% FORMARE COLOANA b - spline cubic NATURAL
	b=zeros(4*q-4,1);
	for i=1:q-1
		b(i,1)=y(i);
		b(i+q-1,1)=y(i+1);
	end

	%% REZOLVARE SISTEM A*z=b -> de rezolvat altfel

	z=invgauss(A)*b;

	for i=1:q-1
		p(i)=z(4*i-3,1);
		r(i)=z(4*i-2,1);
		s(i)=z(4*i-1,1);
		t(i)=z(4*i,1);
	end

	for i=1:length(k)
		for j=1:length(x)
			if(x(j)>k(i)) break; % repozitionare - vezi README
			end
		end
		w(i)=p(j-1)*((k(i)-x(j-1))^3)+r(j-1)*((k(i)-x(j-1))^2)+s(j-1)*(k(i)-x(j-1))+t(j-1);
	end
	
	hold on;
	plot(k,w);
	tline = fgetl(fid);
end
fclose(fid);
end
