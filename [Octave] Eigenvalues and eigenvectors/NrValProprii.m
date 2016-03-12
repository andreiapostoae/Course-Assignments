function numvp = NrValProprii(d, s, val_lambda)
P=ValoriPolinoame(d,s,val_lambda);
n=length(d);
changes=0;
for i=1:n
	if P(i)*P(i+1)<0 changes=changes+1;
	else
		if P(i)==0 
			changes=changes+1;
			if P(i-1)*P(i+1)>0 changes=changes+1;
			end
		end
	end
end
numvp=changes;
end;
