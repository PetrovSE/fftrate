clear;

%fname = 'd:\Sp_Fil32\Austr\DIRES3_3.WAV';
%fname = 'd:\Sp_Files\Ann8.WAV';
%fname = 'd:\Sp_Fil48\DireStraits_s_ref_0.wav';
%fname = 'd:\Sp_Fil48\msix2_s_ref_0.wav';
%fname = 'd:\Sp_Fil32\Austr\NETW1A.WAV';
fname = 'd:\Sp_Fil32\Austr\Track6_orig.WAV';
%fname = 'd:\Sp_Fil32\ROSENB1.WAV';
%fname = 'd:\Sp_Fil44\Alaris\Beethoven.wav';
%fname = 'd:\Sp_Fil44\SF_1.WAV';
%fname = 'd:\Sp_Fil44\Samsung samples\eu_ref_0.wav';
%fname = 'd:\Sp_Fil44\Samsung samples\sm01_s_ref_0.wav';
%fname = 'd:\Sp_Fil48\vivaldi_s_ref_0.wav';
%fname = 'd:\Sp_Fil44\New\PLENTY1.WAV';
%fname = 'd:\Sp_Fil44\New\PORGY1.WAV';
%fname = 'd:\Sp_Fil44\Test\gspi35_1[1].wav';
%fname = 'd:\Sp_Fil44\Test\quar48_1[1].wav';
%fname = 'd:\Sp_Fil44\Test\vioo10_2[1].wav';
%fname = 'd:\Sp_Fil44\Let.wav';

fout  = 'syn.wav';

[x, Fr, nBits] = wavread( fname );
[Len, Chann] = size( x );

Frame = round( 512 * Fr / 48000 );

NormLevel = 0.9;
MinLevel  = 1 / 100;
PeakSlope = Fr * 60;

Level = NormLevel;
Step  = Level / PeakSlope;
Coef  = 1;

for pos = 1 : Frame : Len - Frame;
	idx  = pos : pos + Frame - 1;
	mval = eps;
	
	for ch = 1 : Chann;
		mval = max( mval, max( abs( x( idx, ch ) ) ) );
	end;

	if Level < mval;
		Level = mval;
		Step  = mval / PeakSlope;
	else;
		if mval > MinLevel;
			Level = Level - Frame * Step;
			Level = max( Level, mval );
		end;
	end;

	Level = max( Level, MinLevel );
	step  = ( ( NormLevel / Level ) - Coef ) / Frame;
	
	upd  = Coef + step * ( 1 : Frame );
	Coef = upd( end );
	
	for ch = 1 : Chann;
		x( idx, ch ) = x( idx, ch ) .* upd';
%		x( idx, ch ) = upd' / ( 20 + 0.1 );
%		x( idx, ch ) = Level;
	end;
end;

wavwrite( x, Fr, nBits, fout );
