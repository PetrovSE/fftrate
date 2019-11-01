function [y] = fft_9( x );

	t1  = x(2) + x(9);
	t2  = x(3) + x(8);
	t3  = x(4) + x(7);
	t4  = x(5) + x(6);
	t5  = x(2) - x(9);
	t6  = x(3) - x(8);
	t7  = x(4) - x(7);
	t8  = x(5) - x(6);

	t9  = t1 + t2 + t4;
	t10 = t5 - t6 + t8;
	t11 = x(1) + t3;
	
	a6  = t5 + t6;
	a7  = t6 + t8;
	a8  = t5 - t8;
	a9  = t2 - t1;
	a10 = t4 - t2;
	a11 = t4 - t1;

	
	Q = 2 * pi / 9;
	
	B1 = -1/2;
	B2 = sin( 3 * Q );
	B3 = sin( 4 * Q );
	B4 = sin( 2 * Q );
	B5 = sin( Q );
	B6 = 1/3 * (     cos( Q ) +     cos( 2 * Q ) - 2 * cos( 4 * Q ) );
	B7 = 1/3 * (    -cos( Q ) + 2 * cos( 2 * Q ) -     cos( 4 * Q ) );
	B8 = 1/3 * ( 2 * cos( Q ) -     cos( 2 * Q ) -     cos( 4 * Q ) );
	
	
	a2  = B1 * t3;
	a3  = B2 * t10;
	a4  = B1 * t9;
	a5  = B2 * t7;
	a6  = B3 * a6;
	a7  = B4 * a7;
	a8  = B5 * a8;
	a9  = B6 * a9;
	a10 = B7 * a10;
	a11 = B8 * a11;

	a1  = t9 + t11;
	a2  = a2 + x(1);
	a4  = a4 + t11;

	t6  = a9  + a10;
	t7  = a10 + a11;
	t8  = t7  - t6;
	t9  = a7  + a8;
	t10 = a6  + a8;
	t11 = t10 - t9;
	
	t0 = a2  - t7;
	t1 = a2  + t8;
	t2 = a2  + t6;
	t3 = a5  + t9;
	t4 = t10 - a5;
	t5 = a5 + t11;

	y = [ ...
		a1					...
		t0 - j * t3			...
		t1 - j * t4			...
		a4 - j * a3			...
		t2 - j * t5			...
		t2 + j * t5			...
		a4 + j * a3			...
		t1 + j * t4			...
		t0 + j * t3			...
		];
	