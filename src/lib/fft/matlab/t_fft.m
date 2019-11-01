function [y] = t_fft( t_prof, x );

	prof = t_prof( 1 );
	
	len = prof.len;
	n   = prof.n;
	m   = prof.m;

	if len ~= length( x );
		error( 'Invalid length!' );
		y = [];
		return;
	end;

	x = x( prof.IndexIn );
	
	if n > 1;
		switch n;
			case 2; fn_fft = @fft_2;
			case 3; fn_fft = @fft_3;
			case 5; fn_fft = @fft_5;
			case 7; fn_fft = @fft_7;
		end;

		for i = 1 : m;
			idx = i : m : len;
			x( idx ) = fn_fft( x( idx ) );
		end;
	end;
	

	if ~prof.bGT;
		x = rotate( x, prof.Rotate, n, m );
	end;

	
	if m > 1;
		switch m;
			case 2;		fn_fft = @fft_2;
			case 3;		fn_fft = @fft_3;
			case 5;		fn_fft = @fft_5;
			case 7;		fn_fft = @fft_7;
			otherwise;	fn_fft = 0;
		end;

		for i = 1 : n;
			idx = ( i - 1 ) * m + 1 : i * m;

			if isa( fn_fft, 'function_handle' );
				x( idx ) = fn_fft( x( idx ) );
			else;
				if n == 1;
					x( idx ) = dft( x( idx ), prof.Dft_T );
				else;
					x( idx ) = t_fft( t_prof( 2 : end ), x( idx ) );
				end;
			end;
		end;
	end;

	y( prof.IndexOut ) = x;
	