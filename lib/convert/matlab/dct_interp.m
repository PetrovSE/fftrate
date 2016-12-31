function [y] = dct_interp( x, fr1, fr2 );

	[fr1, fr2] = get_lengths( fr1, fr2, 4 );
	
	hfr1 = fr1 / 2;
	hfr2 = fr2 / 2;
	
	lenx = length( x );
	nfrm = fix( lenx / hfr1 );
	
	%------------------------------------------------------------------
	
	[win1, win2] = get_windows( fr1, fr2, 1 );
	win2 = win2 * sqrt( fr2 / fr1 );
	
	hist1 = zeros( 1, hfr1 );
	hist2 = zeros( 1, hfr2 );
	
	y = zeros( 1, nfrm * hfr2 );
	
	%------------------------------------------------------------------

	for n = 1 : nfrm;
		pos = ( n - 1 ) * hfr1;
		
		frame = x( pos + 1 : pos + hfr1 );
		spec  = odct( [ hist1 frame ] .* win1, 1 );
		hist1 = frame;

		if hfr1 > hfr2;
			spec = spec( 1 : hfr2 );
		else;
			spec = [ spec zeros( 1, hfr2 - hfr1 ) ];
		end;
		
		pos   = ( n - 1 ) * hfr2;
		frame = odct( spec, -1 ) .* win2;
		
		y( pos + 1 : pos + hfr2 ) = frame( 1 : hfr2 ) + hist2;
		hist2 = frame( hfr2 + 1 : end );
	end;
	
	y = y( hfr2 + 1 : end );
	