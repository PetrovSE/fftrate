function [T] = make_dft( len );

	T = zeros( len, len );
	m = 0 : len - 1;
	
	for n = 0 : len - 1;
		T( n + 1, : ) = exp( -2 * pi * j * n * m / len );
	end;
