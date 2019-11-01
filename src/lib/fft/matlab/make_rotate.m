function [rt] = make_rotate( n, m );

	len = n * m;
	w   = exp( -2 * pi * j / len );
	rt  = [];
	
	for i = 2 : n;
		rt = [ rt w .^ ( ( 1 : m - 1 ) * ( i - 1 ) ) ];
	end;
