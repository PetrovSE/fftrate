function [idx] = index_kt( n, m );
	
	len = n * m;
	id  = [];
	
	for i = 1 : m;
		id = [ id ( i : m : len ) ];
	end;
	
	idx( id ) = 1 : len;
	