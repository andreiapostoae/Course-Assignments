function [] = F1
fid=fopen('dodel.txt','r');
figure;
tline = fgetl(fid);
v=str2num(tline);
m=v(1); n=v(2); % citire m si n
tline=fgetl(fid);
while ischar(tline) % pentru fiecare set de puncte 
	%% reinitializarea vectori necesari
	x=[]; % vectorul de coordonate x
	y=[]; % vectorul de coordonate y
	k=[]; % vectorul final de m coordonate x
	w=[]; % vectorul final de m coordonate y
	v=str2num(tline); % transforma string in numar
	len=length(v)/2;
	for i=1:len
		x(i)=v(i); % determinarea coordonatelor de pe Ox
		y(i)=v(i+len); % deteterminarea coordonatelor corespunzatoare x-ilor de pe Oy
	end;
	k=linspace(x(1),x(length(x)),m); % impartirea intervalului in m valori
	for i=1:length(k)
		w(i)=Neville(x,y,k(i)); % functia Neville calculeaza valoarea f(x) in fiecare punct din k
	end
	hold on;
	plot(k,w);
	tline = fgetl(fid);
end
fclose(fid);
end
