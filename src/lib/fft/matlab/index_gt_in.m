function [idx] = index_gt_in( n, m );
	
	len = n * m;
	idx = [];
	
	for i = 1 : n;
		idx = [ idx mod( ( i - 1 ) * m + ( 0 : n : len - 1 ), len ) + 1 ];
	end;
	