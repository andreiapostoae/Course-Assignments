function [x0 y0] = CentroidOfSurface(x,y)
n=length(x);
A=0;
x0=0; y0=0;

for i=1:n-1
	A=A+(1/2)*(x(i)*y(i+1)-x(i+1)*y(i)); % aria "signed" a poligonului - nu stiu echivalentul in romana
end

for i=1:n-1
	x0=x0+(1/(6*A))*(x(i)+x(i+1))*(x(i)*y(i+1)-x(i+1)*y(i));
	y0=y0+(1/(6*A))*(y(i)+y(i+1))*(x(i)*y(i+1)-x(i+1)*y(i));
end

end

