clear;


n = 9; %3 * 3 * 5 * 7 * 13;
x = randn( 1, n ) + i * randn( 1, n );

y = fft_9( x );
z = fft( x );
e = y - z;

[ y' z' abs( e )' ]
sum( abs( e ) )
