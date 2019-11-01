clear;

%fname = 'd:\sp_fil44\pulse.wav';
%fname = 'd:\sp_fil48\modul.wav';
%fname = 'd:\sp_fil44\mitki_6.wav';
%fname = 'D:\Sp_Fil24bit\noise_8.wav';
fname = 'test.wav';
%fname = 'syn.wav';

fout = 'syn.wav';
%fout = 'syn1.wav';

[x, Fr1, nBits] = wavread( fname );

%Fr2 = Fr1;
%Fr2 = 48000;
Fr2 = 44100;
%Fr2 = 11025;
%Fr2 = 8000;

n_chann = size( x, 2 );

for chn = 1 : n_chann;
%	y( :, chn ) = fft_interp( x( :, chn )', Fr1, Fr2 );
	y( :, chn ) = dct_interp( x( :, chn )', Fr1, Fr2 );
end;

wavwrite( y, Fr2, nBits, fout );

%[ err, dif ] = mserr( 'syn_fft.wav', 'syn.wav' )
