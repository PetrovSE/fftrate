function [y] = rotate( x, rt, n, m );

	len = n * m;
	y   = zeros( 1, len );
	
	for i = 1 : n;
		idx = ( i - 1 ) * m + 1 : i * m;
		
		if i == 1;
			y( idx ) = x( idx );
		else;
			y( idx( 1 ) )       = x( idx( 1 ) );
			y( idx( 2 : end ) ) = x( idx( 2 : end ) ) .* rt( 1 : m - 1 );
			rt = rt( m : end );
		end;
	end;
