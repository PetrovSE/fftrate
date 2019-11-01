function [idx] = index_gt_out( n, m );
	
	len = n * m;
	idx = [];
	
	[gcd, nn, mm] = extgcd( n, m );
	
	for i = 1 : n;
		idx = [ idx mod( ( i - 1 ) * mm * m + ( 0 : n : len - 1 ) * nn, len ) + 1 ];
	end;
