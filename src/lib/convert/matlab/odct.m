function [y] = odct( x, flag );
%% Overlapped block DCT transform 
%% flag==1 means forward, -1 means inverse;

	switch flag;
		case 1;
			n = length( x ) / 2;
			y( 1 : n / 2 )		= -x( ( 3 * n / 2 + 1 ) : 2 * n ) - x( 3 * n / 2 : -1 : n + 1 );
			y( n / 2 + 1 : n )	= x( 1 : n / 2 ) - x( n : -1 : n / 2 + 1 );
			y = dct4( y );
			
		case -1;
			n = length( x ); 
			x = dct4( x );
			y = [ ...
				x( n / 2 + 1 : n )			...
				-x( n : -1 : n / 2 + 1 )	...
				-x( n / 2 : - 1 : 1 )		...
				-x( 1 : n / 2 )				...
				];   
	end;  
