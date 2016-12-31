function [y] = fft_3( x );

	a1 = x(2) + x(3);
	a2 = x(2) - x(3);
	a0 = x(1) + a1;
	
	Q  = 2 * pi / 3;
	B1 = cos( Q ) - 1;
	B2 = sin( Q );
	
	b = [ a1 a2 ] .* [ B1 B2 ];
	t = a0 + b(1);
	
	y = [ ...
		a0				...
		t - j * b(2)	...
		t + j * b(2)	...
	];
	