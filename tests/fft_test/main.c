//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>
#include <math.h>

#include "../../lib/swin.h"
#include "../../lib/types.h"
#include "../../lib/array.h"
#include "../../lib/fft.h"
#include "../../lib/dct4.h"
#include "../../lib/mdct.h"
#include "../../lib/mathex.h"
#include "../../lib/clock.h"
#include "../../lib/cmdline.h"
#include "../../lib/profile.h"


#define PROF_EXTERN		"fft/proftodo.inc"
#include "../../lib/profdata.h"




//================================================================
//  Definitions
//----------------------------------------------------------------
#define N					64 //2 * 2 * 3 * 3 * 5 * 5

#define STEP				1
#define N_TESTS				15000L



//================================================================
//  Main
//----------------------------------------------------------------
INT main( VOID )
{
	INT i;

	PVOID p_inst1 = NULL;
	PVOID p_inst2 = NULL;
	BOOL b_radix2 = TRUE;

	PCOMPLEX p_in;
	PCOMPLEX p_out1;
	PCOMPLEX p_out2;

	PVOID p_mse;

	//---------------------------------------------------------------------------

#if 0 // DCT4 test
	DATA x[N];
	DATA y[N];
	HDCT4 h_dct4;

	srand( 1 );
	
	for( i = 0 ; i < N ; i ++ )
	{
		x[i] = (DATA)rand();
	}

	h_dct4 = dct4_init( N );
	dct4( h_dct4, x, y );
	dct4_deinit( h_dct4 );
#endif

	//---------------------------------------------------------------------------

#if 0 // MDCT test
	DATA x[N];
	DATA y[N / 2];
	HMDCT h_mdct;
	
	srand( 1 );
	
	for( i = 0 ; i < N ; i ++ )
	{
		x[i] = (DATA)rand();
	}
	
	h_mdct = mdct_init( N );
	mdct_forward( h_mdct, x, y );
	mdct_inverse( h_mdct, y, x );
	mdct_deinit( h_mdct );
#endif

	//---------------------------------------------------------------------------

	arralloc( p_in, N * STEP );
	arralloc( p_out1, N * STEP );
	arralloc( p_out2, N * STEP );

	p_mse = mse_open( MSE_FLAG_ERROR );
	
	//---------------------------------------------------------------------------

	srand( 1 );

	for( i = 0 ; i < N * STEP ; i ++ )
	{
		cmplx_make( p_in[i], (DATA)rand(), (DATA)rand() );
	}

	//---------------------------------------------------------------------------


//	p_inst1 = fft_init( N );
	p_inst1 = fft_splitradix_init( N );
	p_inst2 = fft_radix2_init( N );

	if( p_inst2 == NULL )
	{
		p_inst2	 = dft_init( N, N, FALSE );
		b_radix2 = FALSE;
	}

	//---------------------------------------------------------------------------

	profiler_add( ProfPointsFFT );
	profiler_reset();

	while( 1 )
	{
		INT ch;

		printf( "\n===========================\n" );
		printf( "Menu:\n" );
		printf( " 1 - precision test;\n" );
		printf( " 2 - speed test;\n" );
		printf( " 0 - exit.\n> " );

		unref_result( scanf( "%d", &ch ) );

		//---------------------------------------------------------------------------

		switch( ch )
		{
		case 1:
			arrcpy( p_out1, p_in, N * STEP );
			arrcpy( p_out2, p_in, N * STEP );

//			fft( p_inst1, p_out1, STEP );
			fft_splitradix( p_inst1, p_out1, STEP );
			b_radix2 ? fft_radix2( p_inst2, p_out2, STEP ) : dft( p_inst2, p_out2, STEP );

			mse_reset( p_mse );
			mse_complex( p_mse, p_out1, p_out2, NULL, N, STEP );
			printf( " FFT MSE = %e\n", mse_get_val( p_mse, MSE_VAL_ERROR ).data );

			ifft_preproc( p_out1, N, STEP );
//			fft( p_inst1, p_out1, STEP );
			fft_splitradix( p_inst1, p_out1, STEP );
			ifft_postproc( p_out1, N, STEP );

			mse_reset( p_mse );
			mse_complex( p_mse, p_in, p_out1, NULL, N, STEP );
			printf( "IFFT MSE = %e\n", mse_get_val( p_mse, MSE_VAL_ERROR ).data );

			ifft_preproc( p_out2, N, STEP );
			b_radix2 ? fft_radix2( p_inst2, p_out2, STEP ) : dft( p_inst2, p_out2, STEP );
			ifft_postproc( p_out2, N, STEP );

			mse_reset( p_mse );
			mse_complex( p_mse, p_in, p_out2, NULL, N, STEP );
			printf( "IFFT MSE = %e\n", mse_get_val( p_mse, MSE_VAL_ERROR ).data );
			continue;


		case 2:
			{
				INT i;
				CPUCLOCKS_INIT( counter );


				CPUCLOCKS_RESET( counter );
				CPUCLOCKS_START( counter );
				
				for( i = 0 ; i < N_TESTS ; i ++ )
				{
					arrcpy( p_out1, p_in, N * STEP );
					// 					fft( p_inst1, p_out1, STEP );
					fft_splitradix( p_inst1, p_out1, STEP );
				}
				
				CPUCLOCKS_STOP( counter );
				printf( "Fft: %g sec\n", CPUCLOCKS_GETTIME( counter ) );

				
				CPUCLOCKS_RESET( counter );
				CPUCLOCKS_START( counter );

				for( i = 0 ; i < N_TESTS ; i ++ )
				{
					arrcpy( p_out1, p_in, N * STEP );
// 					fft( p_inst1, p_out1, STEP );
					fft_splitradix( p_inst1, p_out1, STEP );
				}

				CPUCLOCKS_STOP( counter );
				printf( "Fft: %g sec\n", CPUCLOCKS_GETTIME( counter ) );


				CPUCLOCKS_RESET( counter );
				CPUCLOCKS_START( counter );
				
				for( i = 0 ; i < N_TESTS ; i ++ )
				{
					arrcpy( p_out2, p_in, N * STEP );
 					fft_radix2( p_inst2, p_out2, STEP );
				}
				
				CPUCLOCKS_STOP( counter );
				printf( "Fft: %g sec\n", CPUCLOCKS_GETTIME( counter ) );
			}
			continue;


		default:
			break;
		}

		break;
	}

	//---------------------------------------------------------------------------

	profiler_print_info( stderr, 0 );

	b_radix2 ? fft_radix2_deinit( p_inst2 ) : dft_deinit( p_inst2 );
//	fft_deinit( p_inst1 );
	fft_splitradix_deinit( p_inst1 );

	arrfree( p_in );
	arrfree( p_out1 );
	arrfree( p_out2 );

	mse_close( p_mse );

	return 0;
}
