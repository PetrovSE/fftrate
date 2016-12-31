function [gcd] = euclid( n, m );

	gcd = min( n, m );
	n   = max( n, m );
	
	while 1;
		tmp = rem( n, gcd );
		if tmp == 0;
			break;
		end;
		
		n   = gcd;
		gcd = tmp;
	end;
	