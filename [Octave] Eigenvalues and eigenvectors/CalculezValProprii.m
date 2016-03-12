function vp = CalculezValProprii(d,s,m,eps)
if(m>length(d)) m=length(d);
end
n=length(d);
w=IntervaleValProprii(d,s,m);
if (m==length(d)) j=m-1;
end
if (m<length(d)) j=m;
end
for i=1:j
	stanga=w(i);
	dreapta=w(i+1);
	medie=(stanga+dreapta)*0.5;
	valmedie=ValoriPolinoame(d,s,medie);
	valfin=valmedie(n+1);
	while abs(valfin)>eps
		capat=ValoriPolinoame(d,s,stanga);
		if(capat(n+1)*valfin>0) stanga=medie;
		else
			dreapta=medie;
		end
		medie=(stanga+dreapta)*0.5;
		valmedie=ValoriPolinoame(d,s,medie);
		valfin=valmedie(n+1);
	end
	vp(i)=medie;
end
if (m==length(d)) 
	stanga=w(n);
	dreapta=w(n+1);
	medie=(stanga+dreapta)*0.5;
	valmedie=ValoriPolinoame(d,s,medie);
	valfin=valmedie(n+1);
	while abs(valfin)>eps
		capat=ValoriPolinoame(d,s,stanga);
		if(capat(n+1)*valfin>0) stanga=medie;
		else
			dreapta=medie;
		end
		medie=(stanga+dreapta)*0.5;
		valmedie=ValoriPolinoame(d,s,medie);
		valfin=valmedie(n+1);
	end
	vp(length(vp)+1)=medie;
end
end
