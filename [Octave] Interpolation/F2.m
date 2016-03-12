function [] = F2
fid=fopen('barcelona.txt','r');
figure;
tline = fgetl(fid);
v=str2num(tline);
m=v(1); n=v(2); % citire m si n
tline=fgetl(fid);
while ischar(tline) % pentru fiecare set de valori
	%% reinitializare vectori
	x=[]; % vectorul de coordonate x
	y=[]; % vectorul de coordonate y
	a=[]; % vector auxiliar pentru a-ul dreptelor (panta)
	b=[]; % vector auxiliar pentru b-ul dreptelor
	w=[]; % vector final de m coordonate x
	k=[]; % vector final de m coordonate y
	v=str2num(tline);
	len=length(v)/2;
	for i=1:len
		x(i)=v(i);
		y(i)=v(i+len);
	end;
	for i=1:length(x)-1
		a(i)=(y(i+1)-y(i))/(x(i+1)-x(i));
		b(i)=y(i)-a(i)*x(i);
	end
	k=linspace(x(1),x(length(x)),m);
	for i=1:length(k)
		for j=1:length(x)
			if(x(j)>k(i)) break; % aici se realizeaza pozitionarea - vezi README
			end
		end
		w(i)=a(j-1)*k(i)+b(j-1);
	end
	hold on;
	plot(k,w);
	tline = fgetl(fid);
end
fclose(fid);
end