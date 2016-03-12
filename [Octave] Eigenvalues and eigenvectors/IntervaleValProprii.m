function w = IntervaleValProprii(d, s, m)
[liminf limsup]=LimiteValProprii(d,s);
if m>length(d) m=length(d);
end
r(1)=liminf;
r(m+2)=limsup;
for k=m:-1:1
	mij=(r(1)+r(k+2))/2;
	h=(r(k+2)-r(1))/2;
	numvp=NrValProprii(d,s,mij);
	while(numvp~=k)	
		numvp=NrValProprii(d,s,mij);
		h=h/2;
		if(numvp<k) mij=mij+h;
		else 
			if(numvp>k) mij=mij-h;
			end
		end
	end
	r(k+1)=mij;
end
for i=1:m+1
	w(i)=r(i);
end
end

