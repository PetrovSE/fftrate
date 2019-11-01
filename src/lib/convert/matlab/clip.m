function x = clip( x, range );

	x = min( x, range );
	x = max( x, -range );
	