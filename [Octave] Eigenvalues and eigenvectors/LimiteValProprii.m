function [limita_inf, limita_sup] = LimiteValProprii(d, s)
n=length(d);
for i=1:n
	t(i)=0;
	for j=1:n
		if j==i+1
			t(i)=t(i)+abs(s(i));
		else
			if i==j+1
				t(i)=t(i)+abs(s(j));
			end
		end
	end
end

for i=1:n
	v1(i)=d(i)-t(i);
end
limita_inf=min(v1);

for i=1:n
	v2(i)=d(i)+t(i);
end
limita_sup=max(v2);

end
