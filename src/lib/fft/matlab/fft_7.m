function [y] = fft_7( x );

	t0 = x(2) + x(7);
	t1 = x(2) - x(7);
	t2 = x(3) + x(6);
	t3 = x(3) - x(6);
	t4 = x(5) + x(4);
	t5 = x(5) - x(4);
	t6 = t2 + t0;
	t7 = t5 + t3;
	
	a4 = t2 - t0;
	a2 = t0 - t4;
	a3 = t4 - t2;
	a7 = t5 - t3;
	a6 = t1 - t5;
	a8 = t3 - t1;
	a1 = t6 + t4;
	a5 = t7 + t1;
	a0 = x(1) + a1;
	
	Q  = 2 * pi / 7;
	B1 = 1/3 * ( cos( Q )     + cos( 2 * Q )     + cos( 3 * Q ) ) - 1;
	B2 = 1/3 * ( 2 * cos( Q ) - cos( 2 * Q )     - cos( 3 * Q ) );
	B3 = 1/3 * ( cos( Q )     - 2 * cos( 2 * Q ) + cos( 3 * Q ) );
	B4 = 1/3 * ( cos( Q )     + cos( 2 * Q )     - 2 * cos( 3 * Q ) );
	B5 = 1/3 * ( sin( Q )     + sin( 2 * Q )     - sin( 3 * Q ) );
	B6 = 1/3 * ( 2 * sin( Q ) - sin( 2 * Q )     + sin( 3 * Q ) );
	B7 = 1/3 * ( sin( Q )     - 2 * sin( 2 * Q ) - sin( 3 * Q ) );
	B8 = 1/3 * ( sin( Q )     + sin( 2 * Q )     + 2 * sin( 3 * Q ) );
	
	b = [ a1 a2 a3 a4 a5 a6 a7 a8 ] .* [ B1 B2 B3 B4 B5 B6 B7 B8 ];
	
	t0  =  a0   + b(1);
	t1  =  b(2) + b(3);
	t2  =  b(4) - b(3);
	t3  = -b(2) - b(4);
	t4  =  b(6) + b(7);
	t5  =  b(8) - b(7);
	t6  = -b(8) - b(6);
	
	t7  =  t0 + t1;
	t8  =  t0 + t2;
	t9  =  t0 + t3;
	t10 =  t4 + b(5);
	t11 =  t5 + b(5);
	t12 =  t6 + b(5);
	
	y = [ ...
		a0				...
		t7 - j * t10	...
		t9 - j * t12	...
		t8 + j * t11	...
		t8 - j * t11	...
		t9 + j * t12	...
		t7 + j * t10	...
	];
	