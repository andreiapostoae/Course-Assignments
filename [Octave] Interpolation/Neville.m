function [w] = Neville(x,y,val)
n=length(x);
P=zeros(n,n);
for i=1:n
	P(i,i)=y(i);
end
k=n-1;
i=1;
while(k~=0)
	for j=1:n-i
		P(j,j+i)=((val-x(j+i))*P(j,j+i-1)-(val-x(j))*P(j+1,j+i))/(x(j)-x(j+i));
	end
	k=k-1;
	i=i+1;
end
w=P(1,n);
end
