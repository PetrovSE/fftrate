function [y] = fft_5( x );

	t0 = x(2) + x(5);
	t1 = x(3) + x(4);
	
	a4 = x(4) - x(3);
	a5 = x(2) - x(5);
	a1 = t0 + t1;
	a2 = t0 - t1;
	a3 = a4 + a5;
	a0 = x(1) + a1;
	
	Q  = 2 * pi / 5;
	B1 = 0.5 * ( cos( Q ) + cos( 2 * Q ) ) - 1.0;
	B2 = 0.5 * ( cos( Q ) - cos( 2 * Q ) );
	B3 = sin( Q );
	B4 = sin( Q ) + sin( 2 * Q );
	B5 = sin( 2 * Q ) - sin( Q );
	
	b = [ a1 a2 a3 a4 a5 ] .* [ B1 B2 B3 B4 B5 ];
	
	t0 = a0   + b(1);
	t1 = b(3) - b(4);
	t2 = b(3) + b(5);
	
	t3 = t0 + b(2);
	t4 = t0 - b(2);
	
	y = [ ...
		a0			...
		t3 - j * t1 ...
		t4 - j * t2 ...
		t4 + j * t2 ...
		t3 + j * t1 ...
	];
	