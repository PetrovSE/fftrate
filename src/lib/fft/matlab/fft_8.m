function [y] = fft_8( x );

	t0 = x(1) + x(5);
	a3 = x(1) - x(5);
	t1 = x(2) + x(6);
	t2 = x(2) - x(6);
	t3 = x(3) + x(7);
	a6 = x(3) - x(7);
	t4 = x(4) + x(8);
	t5 = x(4) - x(8);
	
	t6 = t0 + t3;
	a2 = t0 - t3;
	t7 = t1 + t4;
	a5 = t1 - t4;
	a4 = t2 - t5;
	a7 = t2 + t5;
	a0 = t6 + t7;
	a1 = t6 - t7;
	
	
	Q  = 2 * pi / 8;
	B1 = cos( Q );
	B2 = sin( Q );
	
	a4 = a4 * B1;
	a7 = a7 * B2;
	
	t0 = a3 + a4;
	t1 = a3 - a4;
	t2 = a6 + a7;
	t3 = a6 - a7;

	y = [			...
		a0			...
		t0 - j * t2 ...
		a2 - j * a5 ...
		t1 + j * t3 ...
		a1			...
		t1 - j * t3 ...
		a2 + j * a5 ...
		t0 + j * t2 ...
	];
	