function [err, dif] = mserr( fn1, fn2 );

	[x1, Fr1, nBits] = wavread( fn1 );
	[x2, Fr2, nBits] = wavread( fn2 );
	
	[Len1, Ch1] = size( x1 );
	[Len2, Ch2] = size( x2 );

	if Fr1 ~= Fr2 || Ch1 ~= Ch2;
		err = Inf;
		return;
	end;

	len = min( Len1, Len2 );
	err = [];
	dif = [];
	
	for ch = 1 : Ch1;
		er = ...
			sum( ( x1( 1 : len , ch ) - x2( 1 : len , ch ) ) .^ 2 ) / ...
			( sum( x1( 1 : len , ch ) .^ 2 ) + eps );
		df = max( abs( x1( 1 : len , ch ) - x2( 1 : len , ch ) ) );
		
		err = [ err er ];
		dif = [ dif df ];
	end;
	
