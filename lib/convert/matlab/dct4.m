function [y] = dct4( x );
% Computation through l/2-FFT

	n = length( x );
	y = zeros( 1, n );
	
	K1 = exp( -i * pi / n * ( [ 0 : ( n / 2 - 1 ) ] + 1/4 ) ) * sqrt( 2 / n );
	K2 = exp( -i * pi / n * [ 0 : n / 2 - 1 ] );
	
	C( 1 : n / 2 ) = ( x( 1 : 2 : ( n - 1 ) ) + i * x( n : -2 : 2 ) ) .* K1;
	D = fft( C ) .* K2;
	
	y( 1 : 2 : n - 1 )	=  real( D );
	y( n : - 2 : 2 )	= -imag( D );
