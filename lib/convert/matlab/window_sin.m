function [win] = window_sin( n );

	win = sin( pi * ( 0.5 + ( 0 : n - 1 ) ) / n );
	