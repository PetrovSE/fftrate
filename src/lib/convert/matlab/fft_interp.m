function [y] = fft_interp( x, fr1, fr2 );

	[fr1, fr2] = get_lengths( fr1, fr2, 2 );

	hfr1 = fr1 / 2;
	hfr2 = fr2 / 2;
	
	lenx = length( x );
	nfrm = fix( lenx / fr1 );
	
	%------------------------------------------------------------------
	
	[win1, win2] = get_windows( fr1, fr2, 1 );
	win2 = win2 * ( fr2 / fr1 );
	
	hist1 = zeros( 1, hfr1 );
	hist2 = zeros( 1, hfr2 );
	
	y = zeros( 1, nfrm * fr2 );
	
	%------------------------------------------------------------------

	for n = 1 : nfrm;
		pos_1 = ( n - 1 ) * fr1;
		pos_2 = pos_1 + hfr1;
		
		frame1 = x( pos_1 + 1 : pos_1 + hfr1 );
		frame2 = x( pos_2 + 1 : pos_2 + hfr1 );
		frame  = [ hist1 frame1 ] + i * [ frame1 frame2 ];
		hist1  = frame2;
		
		spec = fft( frame .* win1 );
		[idx1, idx2] = make_index( fr1, hfr1, fr2, hfr2 );

		res = zeros( 1, fr2 );
		res( idx2 ) = spec( idx1 );

		frame  = ifft( res ) .* win2;
		frame1 = real( frame );
		frame2 = imag( frame );
		
		frame1( 1 : hfr2 ) = frame1( 1 : hfr2 ) + hist2;
		frame2( 1 : hfr2 ) = frame2( 1 : hfr2 ) + frame1( hfr2 + 1 : fr2 );
		hist2 = frame2( hfr2 + 1 : fr2 );
		
		pos_1 = ( n - 1 ) * fr2;
		pos_2 = pos_1 + hfr2;
		
		y( pos_1 + 1 : pos_1 + hfr2 ) = frame1( 1 : hfr2 );
		y( pos_2 + 1 : pos_2 + hfr2 ) = frame2( 1 : hfr2 );
	end;
	
	y = y( hfr2 + 1 : end );
	