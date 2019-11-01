function [gcd, x, y] = extgcd( a, b );

	if a == 0 || b == 0;
		gcd = 0;
		x   = 0;
		y   = 0;
		return;
	end;

	
	x2 = 1;
	x1 = 0;
	y2 = 0;
	y1 = 1;
	
	while b > 0;
		q = fix( a / b );
		r = a  - q * b;
		x = x2 - q * x1;
		y = y2 - q * y1;
		a = b;
		b = r;
		
		x2 = x1;
		x1 = x;
		y2 = y1;
		y1 = y;
	end;
	
	gcd = a;
	x   = x2;
	y   = y2;
