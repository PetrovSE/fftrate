//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdlib.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../mmsys.h"
#include "../str.h"
#include "../mathex.h"
#include "../fpux86.h"
#include "../profile.h"

#include "convertlib.h"

#define PROF_SLAVE	"convert/proftodo.inc"
#include "../profdata.h"


//================================================================
//  Buffer functions
//----------------------------------------------------------------
STATIC VOID buffer_shift( PSAMPLESTATE p_sample, DWORD size )
{
	INT chn;

	size = min( size, p_sample->rem );
	p_sample->rem -= size;

	for( chn = 0 ; chn < p_sample->n_of_chann ; chn ++ )
	{
		arrcpy
			(
				&p_sample->pp_buffers[chn][0],
				&p_sample->pp_buffers[chn][size],
				p_sample->rem
			);
	}
}


STATIC VOID buffer_clear( PSAMPLESTATE p_sample, DWORD start, DWORD size )
{
	INT chn;

	start = min( start, p_sample->packet_len );
	size  = min( size, p_sample->packet_len - start );

	for( chn = 0 ; chn < p_sample->n_of_chann ; chn ++ )
		arrzero( &p_sample->pp_buffers[chn][start], size );
}


STATIC VOID buffers_merge( PSAMPLESTATE p_sample_dst, PSAMPLESTATE p_sample_src, DWORD size )
{
	INT chn;
	
	size = min( size, p_sample_dst->packet_len - p_sample_dst->rem );
	size = min( size, p_sample_src->rem );

	for( chn = 0 ; chn < p_sample_dst->n_of_chann ; chn ++ )
	{
		arrcpy
			(
				&p_sample_dst->pp_buffers[chn][p_sample_dst->rem],
				&p_sample_src->pp_buffers[chn][0],
				size
			);
	}

	p_sample_dst->rem += size;
}


//================================================================
//  Window functions
//----------------------------------------------------------------
STATIC VOID window_rect( DATA *p_window, INT len, DATA coef )
{
	INT i;

	coef *= (DATA)sqrt( 0.5 );

	for( i = 0 ; i < len ; i ++ )
		p_window[i] = coef;
}


STATIC VOID window_sin( DATA *p_window, INT len, DATA coef )
{
	INT i;

	for( i = 0 ; i < len ; i ++ )
		p_window[i] = coef * (DATA)sin( M_PI * ( 0.5 + (DATA)i ) / (DATA)len );
}


STATIC VOID window_vorbis( DATA *p_window, INT len, DATA coef )
{
	INT i, len2 = len / 2;

	for( i = 0 ; i < len2 ; i ++ )
	{
		DATA arg = (DATA)sin( ( 0.5 * M_PI * (DATA)i + 0.5 ) / (DATA)len2 );

		p_window[len - i - 1] = p_window[i] = coef * (DATA)sin( 0.5 * M_PI * arg * arg );
	}
}


//================================================================
//  Resampling functions
//----------------------------------------------------------------
STATIC VOID resampler_fft
(
	PRESAMPLER	p_resampler_in,
	PRESAMPLER	p_resampler_out,
	PTRANSFORM	p_transform,

	DATA		*p_data_in,
	DATA		*p_hist_in,

	DATA		*p_data_out,
	DATA		*p_hist_out
)
{
	COMPLEX *p_spectrum = p_transform->p_spectrum;
	DATA *p_window;
	INT hist_len, i, j;

	//---------------------------------------------------------------------------

	BP_START( RATE_ANAL );

	hist_len = p_resampler_in->hist_len;
	p_window = p_resampler_in->p_window;

	for( i = 0, j = hist_len ; i < hist_len ; i ++, j ++ )
	{
		cmplx_make( p_spectrum[i], p_hist_in[i], p_data_in[i] );
		cmplx_make( p_spectrum[j], p_data_in[i], p_data_in[j] );

		cmplx_mul_real( p_spectrum[i], p_spectrum[i], p_window[i] );
		cmplx_mul_real( p_spectrum[j], p_spectrum[j], p_window[j] );
	}

	arrcpy( p_hist_in, &p_data_in[hist_len], hist_len );

	BP_STOP( RATE_ANAL );
	
	//---------------------------------------------------------------------------

	BP_START( RATE_FFT_IN );
	fft( p_resampler_in->h_fft_inst, p_spectrum, 1 );
	BP_STOP( RATE_FFT_IN );

	//---------------------------------------------------------------------------

	BP_START( RATE_EXPAND );
	
	if( p_resampler_in->frame_len > p_resampler_out->frame_len )
	{
		arrcpy
			(
				&p_spectrum[p_resampler_out->hist_len + 1],
				&p_spectrum[p_resampler_in->frame_len - p_resampler_out->hist_len + 1],
				p_resampler_out->hist_len - 1
			);
	}
	else
	{
		arrmove
			(
				&p_spectrum[p_resampler_out->frame_len - p_resampler_in->hist_len],
				&p_spectrum[p_resampler_in->hist_len],
				p_resampler_in->hist_len
			);

		arrzero
			(
				&p_spectrum[p_resampler_in->hist_len + 1],
				p_resampler_out->frame_len - p_resampler_in->frame_len - 1
			);
	}

	BP_STOP( RATE_EXPAND );
	
	//---------------------------------------------------------------------------

	BP_START( RATE_FFT_PREP );
	ifft_preproc( p_spectrum, p_resampler_out->frame_len, 1 );
	BP_STOP( RATE_FFT_PREP );

	BP_START( RATE_FFT_OUT );
	fft( p_resampler_out->h_fft_inst, p_spectrum, 1 );
	BP_STOP( RATE_FFT_OUT );
	
	BP_START( RATE_FFT_POST );
	ifft_postproc( p_spectrum, p_resampler_out->frame_len, 1 );
	BP_STOP( RATE_FFT_POST );

	//---------------------------------------------------------------------------

	BP_START( RATE_SYN );
	
	hist_len = p_resampler_out->hist_len;
	p_window = p_resampler_out->p_window;

	for( i = 0, j = hist_len ; i < hist_len ; i ++, j ++ )
	{
		cmplx_mul_real( p_spectrum[i], p_spectrum[i], p_window[i] );
		cmplx_mul_real( p_spectrum[j], p_spectrum[j], p_window[j] );

		p_data_out[i] = p_hist_out[i] + p_spectrum[i].re;
		p_data_out[j] = p_spectrum[j].re + p_spectrum[i].im;
		p_hist_out[i] = p_spectrum[j].im;
	}

	BP_STOP( RATE_SYN );
}


STATIC VOID resampler_dct
(
	PRESAMPLER	p_resampler_in,
	PRESAMPLER	p_resampler_out,
	PTRANSFORM	p_transform,
	
	DATA		*p_data_in,
	DATA		*p_hist_in,
	
	DATA		*p_data_out,
	DATA		*p_hist_out
)
{
	DATA *p_frame	= p_transform->p_frame;
	DATA *p_spec	= p_transform->p_spec;
	DATA *p_window;
	INT hist_len, i, j;
	
	//---------------------------------------------------------------------------

	BP_START( RATE_ANAL );

	hist_len = p_resampler_in->hist_len;
	p_window = p_resampler_in->p_window;

	for( i = 0, j = hist_len ; i < hist_len ; i ++, j ++ )
	{
		p_frame[i]			= ( *p_hist_in ) * p_window[i];
		p_frame[j]			= ( *p_data_in ) * p_window[j];
		( *p_hist_in ++ )	= ( *p_data_in ++ );
	}

	BP_STOP( RATE_ANAL );

	//---------------------------------------------------------------------------

	BP_START( RATE_DCT_IN );
	mdct_forward( p_resampler_in->h_mdct_inst, p_frame, p_spec );
	BP_STOP( RATE_DCT_IN );

	BP_START( RATE_DCT_OUT );
	mdct_inverse( p_resampler_out->h_mdct_inst, p_spec, p_frame );
	BP_STOP( RATE_DCT_OUT );
	
	//---------------------------------------------------------------------------

	BP_START( RATE_SYN );

	hist_len = p_resampler_out->hist_len;
	p_window = p_resampler_out->p_window;
	
	for( i = 0, j = hist_len ; i < hist_len ; i ++, j ++ )
	{
		( *p_data_out ++ ) = p_frame[i] * p_window[i] + ( *p_hist_out );
		( *p_hist_out ++ ) = p_frame[j] * p_window[j];
	}

	BP_STOP( RATE_SYN );
}


//================================================================
//  Transform init & deinit functions
//----------------------------------------------------------------
STATIC BOOL transform_init( PTRANSFORM p_transform, DWORD transform_id, DWORD window_id, BOOL b_resample )
{
	if( !b_resample )
		transform_id = CONV_TRANSFORM_NONE;

	p_transform->transform_id	= transform_id;
	p_transform->window_id		= window_id;

	//----------------------------------------------------------------

	switch( window_id )
	{
	case CONV_WINDOW_NONE:
		p_transform->window_type = "Rectangle";
		break;
		
	case CONV_WINDOW_SIN:
		p_transform->window_type = "Hanning";
		break;
		
	case CONV_WINDOW_VORBIS:
		p_transform->window_type = "Vorbis";
		break;
		
	default:
		p_transform->window_type = NULL;
	}

	//----------------------------------------------------------------

	switch( transform_id )
	{
	case CONV_TRANSFORM_NONE:
		p_transform->transform_type	= "None";
		p_transform->window_type	= "Planar";

		p_transform->window_id		= CONV_WINDOW_NONE;
		p_transform->p_resampler	= NULL;
		p_transform->tail			= 2;
		break;

	case CONV_TRANSFORM_FFT:
		p_transform->transform_type	= "FFT";
		p_transform->p_resampler	= resampler_fft;
		p_transform->tail			= 2;
		break;
		
	case CONV_TRANSFORM_DCT:
		p_transform->transform_type	= "DCT";
		p_transform->p_resampler	= resampler_dct;
		p_transform->tail			= 4;
		break;

	default:
		p_transform->transform_type	= NULL;
	}

	//----------------------------------------------------------------

	return
		!invalid_ptr( p_transform->transform_type )
		&&
		!invalid_ptr( p_transform->window_type );
}


STATIC BOOL transform_alloc( PTRANSFORM p_transform, INT spec_len )
{
	p_transform->p_spectrum		= NULL;
	p_transform->p_spec			= NULL;
	p_transform->p_frame		= NULL;

	//----------------------------------------------------------------

	switch( p_transform->transform_id )
	{
	case CONV_TRANSFORM_NONE:
		return TRUE;
		

	case CONV_TRANSFORM_FFT:
		arralloc( p_transform->p_spectrum, spec_len );
		return arrcheck( p_transform->p_spectrum );
		

	case CONV_TRANSFORM_DCT:
		arralloc( p_transform->p_spec, spec_len );
		arralloc( p_transform->p_frame, spec_len * 2 );

		if( !arrcheck( p_transform->p_spec ) || !arrcheck( p_transform->p_frame ) )
			return FALSE;

		arrzero( p_transform->p_spec, spec_len );
		return TRUE;
	}
		
	//----------------------------------------------------------------

	return FALSE;
}


STATIC VOID transform_free( PTRANSFORM p_transform )
{
	arrfree( p_transform->p_spectrum );
	arrfree( p_transform->p_spec );
	arrfree( p_transform->p_frame );
}


//================================================================
//  Resampler init & deinit functions
//----------------------------------------------------------------
STATIC BOOL resampler_init
(
	PTRANSFORM	p_transform,
	PRESAMPLER	p_resampler,

	INT			frame_len,
	DATA		coef
)
{
	if( is_odd( frame_len ) )
		return FALSE;
	
	//---------------------------------------------------------------------------
	
	p_resampler->frame_len = frame_len;
	
	switch( p_transform->transform_id )
	{
	case CONV_TRANSFORM_NONE:
		p_resampler->packet_len		= frame_len / 2;
		p_resampler->spectrum_len	= 0;
		p_resampler->hist_len		= 0;
		return TRUE;
		
		
	case CONV_TRANSFORM_FFT:
		p_resampler->packet_len		= frame_len;
		p_resampler->spectrum_len	= frame_len;
		p_resampler->hist_len		= frame_len / 2;
		
		p_resampler->h_fft_inst = fft_init( frame_len );
		if( invalid_ptr( p_resampler->h_fft_inst ) )
		{
			return FALSE;
		}
		
		break;
		
		
	case CONV_TRANSFORM_DCT:
		p_resampler->packet_len		= frame_len / 2;
		p_resampler->spectrum_len	= frame_len / 2;
		p_resampler->hist_len		= frame_len / 2;
		
		coef = (DATA)sqrt( coef );
		
		p_resampler->h_mdct_inst = mdct_init( frame_len );
		if( invalid_ptr( p_resampler->h_mdct_inst ) )
			return FALSE;
		
		break;
	}
	
	//---------------------------------------------------------------------------
	
	arralloc( p_resampler->p_window, frame_len );
	if( !arrcheck( p_resampler->p_window ) )
		return FALSE;
	
	//---------------------------------------------------------------------------
	
	switch( p_transform->window_id )
	{
	case CONV_WINDOW_NONE:
		window_rect( p_resampler->p_window, frame_len, coef );
		break;
		
	case CONV_WINDOW_SIN:
		window_sin( p_resampler->p_window, frame_len, coef );
		break;
		
	case CONV_WINDOW_VORBIS:
		window_vorbis( p_resampler->p_window, frame_len, coef );
		break;
		
	default:
		return FALSE;
	}
	
	//---------------------------------------------------------------------------
	
	return TRUE;
}


STATIC VOID resampler_deinit( PRESAMPLER p_resampler )
{
	if( !invalid_ptr( p_resampler->h_fft_inst ) )
	{
		fft_deinit( p_resampler->h_fft_inst );
		p_resampler->h_fft_inst = NULL;
	}
	
	if( !invalid_ptr( p_resampler->h_mdct_inst ) )
	{
		mdct_deinit( p_resampler->h_mdct_inst );
		p_resampler->h_mdct_inst = NULL;
	}
	
	arrfree( p_resampler->p_window );
}


BOOL resampling_init( PCONVINST h_inst, INT freq_in, INT freq_out, DWORD transform_id, DWORD window_id )
{
	INT delay	= ( freq_in * CONV_FRAME_DURATION ) / 1000;
	INT gcd		= euclid( freq_in, freq_out );
	INT tail;

	//---------------------------------------------------------------------------

	if( !more_zero( freq_in ) || !more_zero( freq_out ) )
		return FALSE;

	//---------------------------------------------------------------------------

	h_inst->resampler_in.rate	= freq_in;
	h_inst->resampler_out.rate	= freq_out;

	//---------------------------------------------------------------------------

	if( !transform_init( &h_inst->transform, transform_id, window_id, freq_in != freq_out ) )
		return FALSE;

	//---------------------------------------------------------------------------
	
	freq_in		/= gcd;
	freq_out	/= gcd;
	tail		 = h_inst->transform.tail;

	while
	(
		( freq_in % tail )
		||
		( freq_out % tail )
		||
		freq_in < delay
	)
	{
		freq_in  *= 2;
		freq_out *= 2;
	}

	//---------------------------------------------------------------------------

	if
	(
		!resampler_init( &h_inst->transform, &h_inst->resampler_in, freq_in, D( 1.0 ) )
		||
		!resampler_init( &h_inst->transform, &h_inst->resampler_out, freq_out, (DATA)freq_out / (DATA)freq_in )
	)
	{
		return FALSE;
	}

	//---------------------------------------------------------------------------

	return transform_alloc
		(
			&h_inst->transform,
			max( h_inst->resampler_in.spectrum_len, h_inst->resampler_out.spectrum_len )
		);
}


BOOL resampling_alloc( PCONVINST h_inst, PSAMPLESTATE p_sample )
{
	INT chn;

	//---------------------------------------------------------------------------

	for( chn = 0 ; chn < p_sample->n_of_chann ; chn ++ )
	{
		arralloc( h_inst->pp_hist_in[chn], h_inst->resampler_in.hist_len );
		arralloc( h_inst->pp_hist_out[chn], h_inst->resampler_out.hist_len );

		if( !arrcheck( h_inst->pp_hist_in[chn] ) || !arrcheck( h_inst->pp_hist_out[chn] ) )
			return FALSE;
	}

	//---------------------------------------------------------------------------

	return TRUE;
}


VOID resampling_deinit( PCONVINST h_inst, PSAMPLESTATE p_sample )
{
	INT chn;

	for( chn = 0 ; chn < p_sample->n_of_chann ; chn ++ )
	{
		arrfree( h_inst->pp_hist_in[chn] );
		arrfree( h_inst->pp_hist_out[chn] );
	}

	transform_free( &h_inst->transform );

	//---------------------------------------------------------------------------

	resampler_deinit( &h_inst->resampler_in );
	resampler_deinit( &h_inst->resampler_out );
}


VOID resampling_reset( PCONVINST h_inst, PSAMPLESTATE p_sample )
{
	INT chn;

	for( chn = 0 ; chn < p_sample->n_of_chann ; chn ++ )
	{
		if( !invalid_ptr( h_inst->pp_hist_in[chn] ) )
			arrzero( h_inst->pp_hist_in[chn], h_inst->resampler_in.hist_len );

		if( !invalid_ptr( h_inst->pp_hist_out[chn] ) )
			arrzero( h_inst->pp_hist_out[chn], h_inst->resampler_out.hist_len );
	}
}


VOID resampling_process( PCONVINST h_inst, PSAMPLESTATE p_sample_in, PSAMPLESTATE p_sample_out )
{
	INT chn;

	//---------------------------------------------------------------------------

	if
	(
		p_sample_in->rem < p_sample_in->packet_len
		||
		p_sample_out->rem > 0
	)
	{
		return;
	}

	//---------------------------------------------------------------------------

	for( chn = 0 ; chn < p_sample_in->n_of_chann ; chn ++ )
	{
		h_inst->transform.p_resampler
			(
				&h_inst->resampler_in,
				&h_inst->resampler_out,
				&h_inst->transform,

				p_sample_in->pp_buffers[chn],
				h_inst->pp_hist_in[chn],

				p_sample_out->pp_buffers[chn],
				h_inst->pp_hist_out[chn]
			);
	}

	//---------------------------------------------------------------------------

	p_sample_in->rem	-= p_sample_in->packet_len;
	p_sample_out->rem	+= p_sample_out->packet_len;
}


//================================================================
//  Channel relation functions
//----------------------------------------------------------------
STATIC DWORD chann_relations_all_values( CONST SAMPLESTATE *p_sample, BOOL b_dir )
{
	DWORD val = 0x0;
	INT n;

	for( n = 0 ; n < p_sample->n_of_chann ; n ++ )
	{
		CONST CHANNELDESC *p_desc = mmsys_channel_desc( p_sample->chann_mask, p_sample->p_chann_order, n );

		if( invalid_ptr( p_desc ) )
			break;

		if( b_dir )
			val |= p_desc->dwDirection;
		else
			val |= p_desc->dwPosition;
	}

	return val;
}


STATIC DATA chann_relations_weight_by_direction( CONST CHANNELDESC *p_desc )
{
	switch( p_desc->dwDirection )
	{
	case SPDIR_CENTER:
	case SPDIR_LFE:
		return D( 0.707945784384138 ); // -3 dB
	}

	return D( 1.0 ); // 0 dB
}


STATIC DATA chann_relations_weight_by_position( CONST CHANNELDESC *p_desc )
{
	switch( p_desc->dwPosition )
	{
	case SPPOS_FRONT:
		return D( 1.0 ); // 0 dB

	case SPPOS_LFE:
		return D( 0.100000000000000 ); // -20 dB

	case SPPOS_BACK:
	case SPPOS_FRONT_CENTER:
	case SPPOS_SIDE:
		return D( 0.707945784384138 ); // -3 dB
		
	case SPPOS_TOP_CENTER:
	case SPPOS_TOP_FRONT:
	case SPPOS_TOP_BACK:
		break;
	}

	return D( 0.562341325190349 ); // -5 dB
}


STATIC DATA chann_relations_weight
(
	CONST CHANNELDESC *p_desc1,
	CONST CHANNELDESC *p_desc2,
	pfn_chann_relations_weight fn_chann_relations_weight
)
{
	DATA val1 = fn_chann_relations_weight( p_desc1 );
	DATA val2 = fn_chann_relations_weight( p_desc2 );

	return min( val1, val2 ) / max( val1, val2 );
}


STATIC BOOL chann_relations_coef
(
	CONST CHANNELDESC	*p_desc_alien,
	CONST CHANNELDESC	*p_desc_self,
	CONST SAMPLESTATE	*p_sample_self,
	DATA				*p_val,
	BOOL				b_new_chann
)
{
	//---------------------------------------------------------------------------
	// This channel is exist
	if( flag_check_mask( p_sample_self->chann_mask, p_desc_alien->dwMask ) )
	{
		return FALSE;
	}

	//---------------------------------------------------------------------------
	// Do not make alien direction
	if
	(
		p_desc_self->dwDirection != p_desc_alien->dwDirection
		&&
		(
			flag_check_mask( p_sample_self->direction, p_desc_alien->dwDirection )
			||
			b_new_chann
		)
	)
	{
		return FALSE;
	}

	//---------------------------------------------------------------------------
	// Check neighbour position
	{
		INT n;

		for( n = 0 ; n < 2 ; n ++ )
		{
			DWORD pos = p_desc_alien->dwPosition;

			for( ; non_zero( pos ) ; pos = is_zero( n ) ? pos << 1 : pos >> 1 )
			{
				if( pos == SPPOS_LFE )
					continue;
				
				//---------------------------------------------------------------------------

				if( flag_check_mask( p_sample_self->position, pos ) )
				{
					if( pos == p_desc_self->dwPosition )
					{
						n = 2;
						break;
					}

					return FALSE;
				}
			}
		}
	}
	
	//---------------------------------------------------------------------------
	// Coeff calculation
	*p_val =
		chann_relations_weight( p_desc_alien, p_desc_self, chann_relations_weight_by_position )
		*
		chann_relations_weight( p_desc_alien, p_desc_self, chann_relations_weight_by_direction );

	return TRUE;
}


//================================================================
//  Channel transform functions
//----------------------------------------------------------------
BOOL transform_matrix_make( PCONVINST h_inst )
{
	PSAMPLESTATE p_sample_in  = &h_inst->sample_in;
	PSAMPLESTATE p_sample_out = &h_inst->sample_out;

	INT chn_in, chn_out;

	//---------------------------------------------------------------------------

	for( chn_out = 0 ; chn_out < p_sample_out->n_of_chann ; chn_out ++ )
	{
		PCHANNELMIXER p_mixer = &h_inst->p_chann_mixers[chn_out];
		CONST CHANNELDESC *p_desc_out =
			mmsys_channel_desc
				(
					p_sample_out->chann_mask,
					p_sample_out->p_chann_order,
					chn_out
				);

		if( invalid_ptr( p_desc_out ) )
			return FALSE;

		//---------------------------------------------------------------------------

		p_mixer->channels = 0;

		for( chn_in = 0 ; chn_in < p_sample_in->n_of_chann ; chn_in ++ )
		{
			DATA val = D( 0.0 );
			CONST CHANNELDESC *p_desc_in =
				mmsys_channel_desc
					(
						p_sample_in->chann_mask, 
						p_sample_in->p_chann_order,
						chn_in
					);

			if( invalid_ptr( p_desc_in ) )
				return FALSE;

			//---------------------------------------------------------------------------

			loopinf
			{
				//---------------------------------------------------------------------------
				// The same channel
				if( p_desc_out->dwMask == p_desc_in->dwMask )
				{
					val = D( 1.0 );
					break;
				}

				//---------------------------------------------------------------------------
				// The disappear channel
				if( chann_relations_coef( p_desc_in, p_desc_out, p_sample_out, &val, FALSE ) )
					break;

				//---------------------------------------------------------------------------
				// The new channel
				if( chann_relations_coef( p_desc_out, p_desc_in, p_sample_in, &val, TRUE ) )
					break;

				//---------------------------------------------------------------------------

				break;
			}

			//---------------------------------------------------------------------------

			if( CHANN_IS_USED( val ) )
			{
				p_mixer->relation[p_mixer->channels].idx = chn_in;
				p_mixer->relation[p_mixer->channels].val = min( val, D( 1.0 ) );

				p_mixer->channels ++;
			}
		}

		//---------------------------------------------------------------------------

		if( h_inst->chann_norm )
		{
			DATA sum = D( 0.0 );

			for( chn_in = 0 ; chn_in < p_mixer->channels ; chn_in ++ )
				sum += p_mixer->relation[chn_in].val;
			
			if( sum > D( 1.0 ) )
			{
				for( chn_in = 0 ; chn_in < p_mixer->channels ; chn_in ++ )
					p_mixer->relation[chn_in].val /= sum;
			}
		}
	}

	//---------------------------------------------------------------------------

	return TRUE;
}


VOID transform_matrix_export( PCONVINST h_inst, PCHANNELMIXMATRIX p_transform_matrix )
{
	PSAMPLESTATE p_sample_in  = &h_inst->sample_in;
	PSAMPLESTATE p_sample_out = &h_inst->sample_out;

	INT chn_in, chn_out;

	//----------------------------------------------------------------

	p_transform_matrix->dwChannMaskIn	= p_sample_in->chann_mask;
	p_transform_matrix->dwChannMaskOut	= p_sample_out->chann_mask;

	p_transform_matrix->pOrderIn		= p_sample_in->p_chann_order;
	p_transform_matrix->pOrderOut		= p_sample_out->p_chann_order;

	p_transform_matrix->bEmptyMatrix	= TRUE;

	//----------------------------------------------------------------

	for( chn_out = 0 ; chn_out < p_sample_out->n_of_chann ; chn_out ++ )
	{
		CONST CHANNELMIXER *p_mixer = &h_inst->p_chann_mixers[chn_out];

		for( chn_in = 0 ; chn_in < p_sample_in->n_of_chann ; chn_in ++ )
			p_transform_matrix->ppMixer[chn_in][chn_out] = D( 0.0 );

		for( chn_in = 0 ; chn_in < p_mixer->channels ; chn_in ++ )
		{
			DATA val = p_mixer->relation[chn_in].val;

			p_transform_matrix->ppMixer[p_mixer->relation[chn_in].idx][chn_out] = val;

			if( CHANN_IS_USED( val ) )
				p_transform_matrix->bEmptyMatrix = FALSE;
		}
	}
}


VOID transform_matrix_processing
(
	PCONVINST		h_inst,
	PSAMPLESTATE	p_sample_in,
	PSAMPLESTATE	p_sample_out
)
{
	DWORD rem = min( p_sample_in->rem, p_sample_out->packet_len - p_sample_out->rem );
	INT chn_in, chn_out;

	//---------------------------------------------------------------------------

	switch( h_inst->mixing_mode )
	{
	case MIX_MODE_BYPASS:
		buffers_merge( p_sample_out, p_sample_in, rem );
		break;


	case MIX_MODE_MIXING:
		buffer_clear( p_sample_out, p_sample_out->rem, rem );

		for( chn_out = 0 ; chn_out < p_sample_out->n_of_chann ; chn_out ++ )
		{
			CONST CHANNELMIXER *p_mixer = &h_inst->p_chann_mixers[chn_out];

			for( chn_in = 0 ; chn_in < p_mixer->channels ; chn_in ++ )
			{
				DATA  val = p_mixer->relation[chn_in].val;
				DWORD pos;

				DATA *p_in   = p_sample_in->pp_buffers[p_mixer->relation[chn_in].idx];
				DATA *p_out  = &p_sample_out->pp_buffers[chn_out][p_sample_out->rem];

				for( pos = 0 ; pos < rem ; pos ++ )
					( *p_out ++ ) += val * ( *p_in ++ );
			}
		}

		p_sample_out->rem += rem;
		break;
	}

	//---------------------------------------------------------------------------

	buffer_shift( p_sample_in, rem );
}


//================================================================
//  Samples init & deinit functions
//----------------------------------------------------------------
BOOL samples_alloc
(
	PSAMPLESTATE	p_sample,
	DWORD			unit_type,
	INT				n_chann,
	DWORD			chann_mask,
	CONST CHAR		*p_chann_order,
	INT				packet_len,
	BOOL			b_need_peaks
)
{
	INT chn;

	switch( unit_type )
	{
	case UNIT_TYPE_INTERNAL:
	case UNIT_TYPE_FIXED_8:
	case UNIT_TYPE_FIXED_16:
	case UNIT_TYPE_FIXED_24:
	case UNIT_TYPE_FIXED_32:
	case UNIT_TYPE_FLOAT:
	case UNIT_TYPE_DOUBLE:
		break;

	default:
		return FALSE;
	}

	//---------------------------------------------------------------------------

	if( is_zero( chann_mask ) )
		chann_mask = mmsys_make_chmask( n_chann );

	if( n_chann != binweight( chann_mask ) || is_zero( n_chann ) || n_chann > N_OF_MAX_CHANNELS )
		return FALSE;

	//---------------------------------------------------------------------------

	p_sample->unit_type		= unit_type;
	p_sample->n_of_chann	= n_chann;
	p_sample->chann_mask	= chann_mask;

	p_sample->direction		= chann_relations_all_values( p_sample, TRUE );
	p_sample->position		= chann_relations_all_values( p_sample, FALSE );

	p_sample->p_chann_order = mmsys_channel_order_make( p_chann_order );

	//---------------------------------------------------------------------------

	p_sample->unit_size		= get_unit_size( p_sample->unit_type );
	p_sample->sample_size	= p_sample->unit_size * p_sample->n_of_chann;
	p_sample->packet_len	= packet_len;

	samples_set_skipped( p_sample, 0 );

	//---------------------------------------------------------------------------

	if( b_need_peaks )
	{
		arralloc_unit( p_sample->p_peaks );
		if( !arrcheck( p_sample->p_peaks ) )
			return FALSE;
	}

	//---------------------------------------------------------------------------

	for( chn = 0 ; chn < n_chann ; chn ++ )
	{
		arralloc( p_sample->pp_buffers[chn], p_sample->packet_len );
		if( !arrcheck( p_sample->pp_buffers[chn] ) )
		{
			chn = 0;
			break;
		}
	}
	
	return more_zero( chn );
}


VOID samples_free( PSAMPLESTATE p_sample )
{
	INT chn;

	for( chn = 0 ; chn < p_sample->n_of_chann ; chn ++ )
		arrfree( p_sample->pp_buffers[chn] );
	
	arrfree( p_sample->p_peaks );

	p_sample->p_chann_order = mmsys_channel_order_free( p_sample->p_chann_order );
}


VOID samples_reset( PSAMPLESTATE p_sample )
{
	PSAMPLEPEAKS p_peaks = p_sample->p_peaks;
	
	if( !invalid_ptr( p_peaks ) )
	{
		p_peaks->dwChannMask	= p_sample->chann_mask;
		p_peaks->pOrder			= p_sample->p_chann_order;
		
		arrzero( p_peaks->pValues, N_OF_MAX_CHANNELS );
	}

	//----------------------------------------------------------------

	p_sample->rem		= 0;
	p_sample->skip_len	= p_sample->hist_len;
}


VOID samples_set_skipped( PSAMPLESTATE p_sample, DWORD hist_len )
{
	p_sample->hist_len	= hist_len;
}


//================================================================
//  Load & Save functions
//----------------------------------------------------------------
STATIC VOID samples_get_peaks( PSAMPLESTATE p_sample, INT from, INT to )
{
	PSAMPLEPEAKS p_peaks = p_sample->p_peaks;
	INT chn;

	//---------------------------------------------------------------------------

	if( from == to || invalid_ptr( p_peaks ) )
		return;

	//---------------------------------------------------------------------------

	for( chn = 0 ; chn < p_sample->n_of_chann ; chn ++ )
	{
		DATA max_val = D( 0.0 );
		DATA *p_data = p_sample->pp_buffers[chn];
		INT pos;

		for( pos = from ; pos < to ; pos ++ )
		{
			DATA val = (DATA)fabs( p_data[pos] );
			max_val  = max( max_val, val );
		}

		p_peaks->pValues[chn] = max_val * NORM_FLOAT_OUT;
	}
}


DWORD samples_load
(
	PSAMPLESTATE	p_sample,
	CONST BYTE		*p_buff,
	DWORD	buff_size_in_bytes,
	BOOL	b_need_peaks
)
{
	DWORD skipped		= p_sample->rem;
	DWORD buff_size		= buff_size_in_bytes / p_sample->sample_size;
	DWORD load_size		= min( buff_size, p_sample->packet_len - skipped );
	DWORD stop_pos		= skipped + load_size;

	INT  step = p_sample->n_of_chann;
	INT  chn;

	//---------------------------------------------------------------------------

	switch( p_sample->unit_type )
	{
	case UNIT_TYPE_FIXED_8:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			PBYTE p_data  = (PBYTE)p_buff + chn;
			DATA *p_smpls = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < stop_pos ; pos ++ )
			{
				( *p_smpls ++ ) = (DATA)mul_power2( (LONG)( *p_data ) + ADD_SMP_8, SHIFT_SMP_8 );
				p_data += step;
			}
		}
		break;


	case UNIT_TYPE_FIXED_16:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			SHORT *p_data = (SHORT *)p_buff + chn;
			DATA *p_smpls = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < stop_pos ; pos ++ )
			{
				( *p_smpls ++ ) = (DATA)mul_power2( (LONG)( *p_data ), SHIFT_SMP_16 );
				p_data += step;
			}
		}
		break;


	case UNIT_TYPE_FIXED_24:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			PBYTE p_data  = (PBYTE)p_buff + 3 * chn;
			DATA *p_smpls = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < stop_pos ; pos ++ )
			{
				( *p_smpls ++ ) =
					(DATA)mul_power2
						(
							(LONG)make_dword4( p_data[0], p_data[1], p_data[2], 0x0 ),
							SHIFT_SMP_24
						);

				p_data += 3 * step;
			}
		}
		break;


	case UNIT_TYPE_FIXED_32:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			LONG *p_data  = (LONG *)p_buff + chn;
			DATA *p_smpls = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < stop_pos ; pos ++ )
			{
				( *p_smpls ++ ) = (DATA)( *p_data );
				p_data += step;
			}
		}
		break;


	case UNIT_TYPE_FLOAT:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			FLOAT *p_data = (FLOAT *)p_buff + chn;
			DATA *p_smpls = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < stop_pos ; pos ++ )
			{
				( *p_smpls ++ ) = (DATA)( *p_data ) * NORM_FLOAT_IN;
				p_data += step;
			}
		}
		break;


	case UNIT_TYPE_DOUBLE:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			DOUBLE *p_data = (DOUBLE *)p_buff + chn;
			DATA *p_smpls = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < stop_pos ; pos ++ )
			{
				( *p_smpls ++ ) = (DATA)( *p_data ) * NORM_FLOAT_IN;
				p_data += step;
			}
		}
		break;
	}

	//---------------------------------------------------------------------------

	if( b_need_peaks )
		samples_get_peaks( p_sample, skipped, stop_pos );

	p_sample->rem = stop_pos;

	//---------------------------------------------------------------------------

	return load_size * p_sample->sample_size;
}


DWORD samples_save
(
	PSAMPLESTATE	p_sample,
	PBYTE	p_buff,
	DWORD	buff_size_in_bytes,
	BOOL 	b_need_peaks
)
{
	DWORD buff_size	= buff_size_in_bytes / p_sample->sample_size;
	DWORD save_size	= min( buff_size, p_sample->rem );
	DWORD skipped	= min( save_size, p_sample->skip_len );

	INT step = p_sample->n_of_chann;
	INT chn;

	DWORD fpu_flag = fpux86_get_rounding_mode();

	//---------------------------------------------------------------------------

	fpux86_set_rounding_mode( FPU_ROUND_DOWN );

	switch( p_sample->unit_type )
	{
	case UNIT_TYPE_FIXED_8:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			PBYTE p_data  = (PBYTE)p_buff + chn;
			DATA *p_smpls = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < save_size ; pos ++ )
			{
				LONG lval   = fpux86_clip_long( ( *p_smpls ++ ) + ROUND_SMP_8 );
				( *p_data )	= (BYTE)( div_power2( lval, SHIFT_SMP_8 ) - ADD_SMP_8 );
				p_data += step;
			}
		}
		break;


	case UNIT_TYPE_FIXED_16:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			SHORT *p_data = (SHORT *)p_buff + chn;
			DATA *p_smpls = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < save_size ; pos ++ )
			{
				LONG lval   = fpux86_clip_long( ( *p_smpls ++ ) + ROUND_SMP_16 );
				( *p_data )	= (SHORT)div_power2( lval, SHIFT_SMP_16 );
				p_data += step;
			}
		}
		break;


	case UNIT_TYPE_FIXED_24:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			PBYTE p_data  = (PBYTE)p_buff + 3 * chn;
			DATA *p_smpls = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < save_size ; pos ++ )
			{
				LONG lval = fpux86_clip_long( ( *p_smpls ++ ) + ROUND_SMP_24 );

				lval >>= 8; ( *p_data ++ ) = get_low_byte( lval );
				lval >>= 8; ( *p_data ++ ) = get_low_byte( lval );
				lval >>= 8; ( *p_data ++ ) = get_low_byte( lval );

				p_data += 3 * ( step - 1 );
			}
		}
		break;


	case UNIT_TYPE_FIXED_32:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			LONG *p_data  = (LONG *)p_buff + chn;
			DATA *p_smpls = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < save_size ; pos ++ )
			{
				( *p_data )	= fpux86_clip_long( ( *p_smpls ++ ) + ROUND_SMP_32 );
				p_data += step;
			}
		}
		break;


	case UNIT_TYPE_FLOAT:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			FLOAT *p_data = (FLOAT *)p_buff + chn;
			DATA *p_smpls = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < save_size ; pos ++ )
			{
				( *p_data )	= (FLOAT)( ( *p_smpls ++ ) * NORM_FLOAT_OUT );
				p_data += step;
			}
		}
		break;


	case UNIT_TYPE_DOUBLE:
		for( chn = 0 ; chn < step ; chn ++ )
		{
			DOUBLE *p_data = (DOUBLE *)p_buff + chn;
			DATA *p_smpls  = &p_sample->pp_buffers[chn][skipped];
			DWORD pos;

			for( pos = skipped ; pos < save_size ; pos ++ )
			{
				( *p_data )	= (DOUBLE)( ( *p_smpls ++ ) * NORM_FLOAT_OUT );
				p_data += step;
			}
		}
		break;
	}

	//---------------------------------------------------------------------------

	if( b_need_peaks )
		samples_get_peaks( p_sample, skipped, save_size );

	buffer_shift( p_sample, save_size );
	fpux86_set_rounding_mode( fpu_flag );

	//---------------------------------------------------------------------------

	p_sample->skip_len -= skipped;

	return ( save_size - skipped ) * p_sample->sample_size;
}
