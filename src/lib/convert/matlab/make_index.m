function [idx1, idx2] = make_index( n1, m1, n2, m2 );

	if n1 < n2;
		idx1 = [ ( 0 : m1 ) ( n1 - m1 + 1 : n1 - 1 ) ];
		idx2 = [ ( 0 : m1 ) ( n2 - m1 + 1 : n2 - 1 ) ];
	else;
		idx1 = [ ( 0 : m2 ) ( n1 - m2 + 1 : n1 - 1 ) ];
		idx2 = [ ( 0 : m2 ) ( n2 - m2 + 1 : n2 - 1 ) ];
	end;

	idx1 = idx1 + 1;
	idx2 = idx2 + 1;
	