function [win] = window_vorbis( n );

	k   = fix( n / 2 );
	win = sin( pi / 2 * ( sin( ( pi / 2 * ( 0 : k - 1 ) + 0.5 ) / k ) .^ 2 ) );
	win = [ win fliplr( win ) ];
