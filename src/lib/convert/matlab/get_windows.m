function [win1, win2] = get_windows( len1, len2, flag );

	switch flag;
		case 0;
			win1 = window_sin( len1 );
			win2 = window_sin( len2 );
			
		case 1;
			win1 = window_vorbis( len1 );
			win2 = window_vorbis( len2 );
	end;
