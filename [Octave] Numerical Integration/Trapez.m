function I = Trapez(x,y)

n=length(x);

I=0;

if(n==1) 
	I=0;
else
	for i=1:n-1	
		I=I+(1/2)*(x(i+1)-x(i))*(y(i+1)+y(i)); % formula arie trapez
	end
end

I=sprintf('%3.3f', abs(I)); % converteste aria la un string cu 3 zecimale pozitiv

end