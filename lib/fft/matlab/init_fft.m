function [t_prof] = init_fft( t_prof, len );
	
	if mod( len, 2 ) == 0;
		n = 2;
	elseif mod( len, 3 ) == 0;
		n = 3;
	elseif mod( len, 5 ) == 0;
		n = 5;
	elseif mod( len, 7 ) == 0;
		n = 7;
	else;
		n = 1;
	end;
	
	m = len / n;
	
	t_prof = [ t_prof make_prof( n, m ) ];

	if n > 1;
		switch m;
		case 1;
		case 2;
		case 3;
		case 5;
		case 7;
		otherwise; t_prof = init_fft( t_prof, m );
		end;
	end;
	