function I = Simpson(x,y);
n=length(x);

I=0;

if(n==1) 
	I=0;
else
	for i=1:2:n-2
		h=(x(i+2)-x(i))/6; % (x(i+2)-x(i))/3 este h-ul din teorie, dar cum acesta trebuia impartit
						   % la 2 la sfarsit, am facut asta direct
		%% pentru cazurile urmatoare, urmariti README
		if(x(i+1)>=x(i) && x(i+2)>=x(i+1)) % cazul 1
			I=I-h*(y(i+2)+4*y(i+1)+y(i));
		else
			if(x(i+1)<=x(i) && x(i+2)<=x(i+1)) % cazul 2
				I=I-h*(y(i+2)+4*y(i+1)+y(i));
			else
				if(x(i+2)<=x(i+1) && x(i+1)>=x(i)) % cazul 3
					I=I-(y(i+1)+y(i))*(x(i+1)-x(i))*(1/2);
					I=I+(y(i+2)+y(i+1))*abs((x(i+2)-x(i+1)))*(1/2);
				else
					if(x(i+2)>=x(i+1) && x(i+1)<=x(i)) % cazul 4
						I=I+(y(i+1)+y(i))*abs((x(i+1)-x(i)))*(1/2);
						I=I-(y(i+2)+y(i+1))*(x(i+2)-x(i+1))*(1/2);
					end
				end
			end

		end
	end
end

I=sprintf('%3.3f', I); % conversteste aria la un string de 3 zecimale
end