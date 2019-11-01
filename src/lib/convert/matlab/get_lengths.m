function [fr1, fr2] = get_lengths( fr1, fr2, dv );

	delay = round( fr1 * 32 / 1000 );

	gcd = euclid( fr1, fr2 );
	fr1 = fr1 / gcd;
	fr2 = fr2 / gcd;
	
	while mod( fr1, dv ) ~= 0 ||  mod( fr2, dv ) ~= 0 || fr1 < delay;
		fr1 = fr1 * 2;
		fr2 = fr2 * 2;
	end;
