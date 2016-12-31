function [y] = t_ifft( t_prof, x );

	y = conj( t_fft( t_prof, conj( x ) ) ) / length( x );
	