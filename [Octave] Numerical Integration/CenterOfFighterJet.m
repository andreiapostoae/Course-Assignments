function [] = CenterOfFighterJet(path)

%% determinare centroid

string_aux=strcat(path,'contur.in'); % citire din path/contur.in
fid1=fopen(string_aux,'r');

contur=fscanf(fid1,'%f');
contur=contur';

n=length(contur)/2; % pentru ca se citesc si x si y, lungimea este jumatate din total

for i=1:n
	x_contur(i)=contur(2*i-1); % elementele cu indice impar sunt valorile pt x
	y_contur(i)=contur(2*i); % elementele cu indice par sunt valorile pt y
end

[x_centroid y_centroid] = CentroidOfSurface(x_contur,y_contur); % aplicarea functiei de centroid
printf('Centroid of surface: x = %3.3f, y = %3.3f\n',x_centroid,y_centroid); % afisarea cu format


%% Aria cu trapeze, apoi cu Simpson


for i=1:7
	x=[]; y=[]; % reinitializare x si y deoarece dimensiunile se schimba la fiecare iteratie
	string_aux=strcat(path,'date',int2str(i-1),'.in'); % citire din path/date[k].in, unde [k]=1:7
	fid2=fopen(string_aux,'r');
	coordonate=fscanf(fid2,'%f');
	coordonate=coordonate';
	n=length(coordonate)/3; % pentru ca se citesc valori pentru x,y,z
 	for j=1:n
		x(j)=coordonate(3*j-2); % elementele cu indice de forma 3k-2 sunt valorile pt x
		y(j)=coordonate(3*j-1); % elementele cu indice de forma 3k-1 sunt valorile pt y
		z(i)=coordonate(3*j); % elementele cu indice de forma 3k sunt valorile pt z
	end

	[supr_centr_x(i) supr_centr_y(i)]=CentroidOfSurface(x,y); % se retin valorile centroizilor fiecarei sectiuni
												   % pentru uz ulterior	
	k=Trapez(x,y); % apelarea functiei Trapez
	l=Simpson(x,y); % apelarea funtiei Simpson
	A_trapeze(i)=str2num(k); % deoarece k este in format string, trebuie reformatat in float
	A_Simpson(i)=str2num(l); % analog
end

for i=1:7
	printf('%3.3f ',A_trapeze(i)); % afisare arie folosind metoda trapezelor
end
printf('\n');
for i=1:7
	printf('%3.3f ',A_Simpson(i)); % afisare arie folosind metoda Simpson 
end
printf('\n');


%% DETERMINARE MASA SI VOLUM

string_aux=strcat(path,'densitati.in');
fid3=fopen(string_aux,'r');
densitati=fscanf(fid3,'%d'); % se citesc valorile densitatilor medii dintre sectiuni

for i=1:6
	arie_medie(i)=(A_trapeze(i+1)+A_trapeze(i))/2; % aria medie dintre doua sectiuni
	volum(i)=arie_medie(i)*(z(i+1)-z(i)); % volum dintre sectiuni
	masa(i)=volum(i)*densitati(i); % masa dintre sectiuni
end

masa_totala=sum(masa);
volum_total=sum(volum);

printf('Volume of plane: %3.3f\n',volum_total); % afisare volum total
printf('Mass of plane: %3.3f\n',masa_totala); % afisare masa totala


%% CENTRU DE MASA

x_med=0;
for i=1:6
	x_med=x_med+((supr_centr_x(i+1)*A_trapeze(i+1)+supr_centr_x(i)*A_trapeze(i))/(A_trapeze(i+1)+A_trapeze(i)))*masa(i);
end
x_med=x_med/masa_totala; % calcul x_centru_de_masa - vezi README

y_med=0;
for i=1:6
	y_med=y_med+((supr_centr_y(i+1)*A_trapeze(i+1)+supr_centr_y(i)*A_trapeze(i))/(A_trapeze(i+1)+A_trapeze(i)))*masa(i);
end
y_med=y_med/masa_totala; % calcul y_centru_de_masa

z_med=0;
for i=1:6
	z_med=z_med+((z(i+1)+z(i))/2)*masa(i);
end
z_med=z_med/masa_totala; % calcul z_centru_de_masa

printf('Center of mass: %3.3f %3.3f %3.3f\n',x_med,y_med,z_med); % afisare cu format






