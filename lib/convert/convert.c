//================================================================
//  Includes
//----------------------------------------------------------------
#include <string.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../str.h"
#include "../profile.h"

#include "../convert.h"
#include "convertlib.h"


#define PROF_MAIN	"convert/proftodo.inc"
#include "../profdata.h"


//================================================================
//  Get module information
//----------------------------------------------------------------
LIBINFO_FUNCTION
(
	convert,
	"PCM Converter",
	"Sampling converter & channels mixer for Wave PCM data",
	"Copyright (c) 2007-11 PetrovSE",
	"2.0.9.8"
);


//================================================================
//  Open module
//----------------------------------------------------------------
HCONVERT convert_open
(
	CONST WAVEFORMATEX	*p_src_format,
	CONST WAVEFORMATEX	*p_dst_format,
	CONST CHAR			*types,
	...
)
{
	PCONVINST h_inst;
	
	DWORD transform_id	= CONV_TRANSFORM_DCT;
	DWORD window_id		= CONV_WINDOW_VORBIS;

	BOOL  b_sync		= TRUE;
	BOOL  b_norm		= TRUE;

	CONST CHAR *order_in	= SPORDER_DEFAULT;
	CONST CHAR *order_out	= SPORDER_DEFAULT;

	//---------------------------------------------------------------------------

	parse_arg_start( types, "dt dw bs bn pi po" )
		&transform_id,
		&window_id,
		&b_sync,
		&b_norm, 
		&order_in,
		&order_out
	parse_arg_stop

	//---------------------------------------------------------------------------

	arralloc_unit( h_inst );

	while( arrcheck( h_inst ) )
	{
		PSAMPLESTATE p_sample_in  = &h_inst->sample_in;
		PSAMPLESTATE p_sample_out = &h_inst->sample_out;

		arrzero_unit( h_inst );

		//---------------------------------------------------------------------------

		if
		( 
			invalid_ptr( p_src_format )
			||
			invalid_ptr( p_dst_format )
		)
		{
			break;
		}

		//--------------------------------------------------------------------------

		if
		(
			!resampling_init
				(
					h_inst,
					mmsys_format_info( p_src_format, 'r' ),
					mmsys_format_info( p_dst_format, 'r' ),
					transform_id,
					window_id
				)
		)
		{
			break;
		}

		//--------------------------------------------------------------------------

		if
		(
			!samples_alloc
				(
					p_sample_in,
					mmsys_format_unit_type( p_src_format ),
					mmsys_format_info( p_src_format, 'c' ),
					mmsys_format_info( p_src_format, 'm' ),
					order_in,
					h_inst->resampler_in.packet_len,
					TRUE
				)
			||
			!samples_alloc
				(
					p_sample_out,
					mmsys_format_unit_type( p_dst_format ),
					mmsys_format_info( p_dst_format, 'c' ),
					mmsys_format_info( p_dst_format, 'm' ),
					order_out,
					h_inst->resampler_out.packet_len,
					TRUE
				)
		)
		{
			break;
		}

		//---------------------------------------------------------------------------
		
		h_inst->chann_norm		= b_norm;
		h_inst->mixing_mode		= MIX_MODE_BYPASS;
		h_inst->resampling_mode	= RES_MODE_NONE;
		
		h_inst->p_sample_src	= p_sample_in;
		h_inst->p_sample_dst	= p_sample_out;

		//--------------------------------------------------------------------------

		if
		( 
			p_sample_in->chann_mask != p_sample_out->chann_mask
			||
			!mmsys_channel_order_is_same
				(
					p_sample_in->p_chann_order,
					p_sample_out->p_chann_order,
					p_sample_in->chann_mask
				)
		)
		{
			h_inst->mixing_mode = MIX_MODE_MIXING;
		}

		if( !transform_matrix_make( h_inst ) )
		{
			if( h_inst->mixing_mode == MIX_MODE_MIXING )
				break;

			arrzero( h_inst->p_chann_mixers, N_OF_MAX_CHANNELS );
		}
		
		//--------------------------------------------------------------------------

		if( h_inst->resampler_in.packet_len != h_inst->resampler_out.packet_len )
		{
			BOOL res = FALSE;

			//---------------------------------------------------------------------------

			switch( h_inst->mixing_mode )
			{
			case MIX_MODE_BYPASS:
				h_inst->p_sample_src	= p_sample_out;
				h_inst->p_sample_work	= p_sample_out;
				h_inst->resampling_mode	= RES_MODE_PRE;
				res = TRUE;
				break;


			case MIX_MODE_MIXING:
				h_inst->resampling_mode = 
					( p_sample_in->n_of_chann > p_sample_out->n_of_chann )
					?
					RES_MODE_POST
					:
					RES_MODE_PRE;

				//---------------------------------------------------------------------------

				arralloc_unit( h_inst->p_sample_work );

				if( !arrcheck( h_inst->p_sample_work ) )
					break;

				arrzero_unit( h_inst->p_sample_work );

				//---------------------------------------------------------------------------

				switch( h_inst->resampling_mode )
				{
				case RES_MODE_PRE:
					h_inst->p_sample_src = h_inst->p_sample_work;

					res = samples_alloc
						(
							h_inst->p_sample_src,
							UNIT_TYPE_INTERNAL,
							p_sample_in->n_of_chann,
							p_sample_in->chann_mask,
							order_in,
							p_sample_out->packet_len,
							FALSE
						);
					break;


				case RES_MODE_POST:
					h_inst->p_sample_dst = h_inst->p_sample_work;

					res = samples_alloc
						(
							h_inst->p_sample_dst,
							UNIT_TYPE_INTERNAL,
							p_sample_out->n_of_chann,
							p_sample_out->chann_mask,
							order_out,
							p_sample_in->packet_len,
							FALSE
						);
					break;
				}

				break;
			}

			//---------------------------------------------------------------------------

			if( !res )
				break;

			if( !resampling_alloc( h_inst, h_inst->p_sample_work ) )
				break;

			if( b_sync )
				samples_set_skipped( p_sample_out, h_inst->resampler_out.hist_len );
		}

		//---------------------------------------------------------------------------

		return (HCONVERT)h_inst;
	}

	//---------------------------------------------------------------------------

	return convert_close( (HCONVERT)h_inst );
}


//================================================================
//  Close module
//----------------------------------------------------------------
HCONVERT convert_close( HCONVERT p_inst )
{
	PCONVINST h_inst = (PCONVINST)p_inst;

	if( arrcheck( h_inst ) )
	{
		PSAMPLESTATE p_sample_in	= &h_inst->sample_in;
		PSAMPLESTATE p_sample_out	= &h_inst->sample_out;
		PSAMPLESTATE p_sample_work	= h_inst->p_sample_work;

		//----------------------------------------------------------------

		if( arrcheck( p_sample_work ) )
		{
			resampling_deinit( h_inst, p_sample_work );
		
			//----------------------------------------------------------------

			if( p_sample_work != p_sample_out )
			{
				samples_free( p_sample_work );
				arrfree( p_sample_work );
			}
		}

		//----------------------------------------------------------------

		samples_free( p_sample_in );
		samples_free( p_sample_out );
	}

	//----------------------------------------------------------------

	arrfree( h_inst );

	return NULL;
}


//================================================================
//  Reset module
//----------------------------------------------------------------
BOOL convert_reset( HCONVERT p_inst )
{
	PCONVINST h_inst = (PCONVINST)p_inst;

	if( arrcheck( h_inst ) )
	{
		PSAMPLESTATE p_sample_in	= &h_inst->sample_in;
		PSAMPLESTATE p_sample_out	= &h_inst->sample_out;
		PSAMPLESTATE p_sample_work	= h_inst->p_sample_work;

		//----------------------------------------------------------------

		if( arrcheck( p_sample_work ) )
		{
			resampling_reset( h_inst, p_sample_work );

			//----------------------------------------------------------------

			if( p_sample_work != p_sample_out )
				samples_reset( p_sample_work );
		}

		//----------------------------------------------------------------

		samples_reset( p_sample_in );
		samples_reset( p_sample_out );

		convert_stat_mode( (HCONVERT)h_inst, CONV_STAT_ALL, FALSE );
		return TRUE;
	}

	return FALSE;
}


//================================================================
//  Processing
//----------------------------------------------------------------
BOOL convert_processing
(
	HCONVERT	p_inst,
	CONST BYTE	*p_src,
	PDWORD		p_src_size,
	PBYTE		p_dst,
	PDWORD		p_dst_size
)
{
	PCONVINST h_inst = (PCONVINST)p_inst;

	DWORD bytes_src = 0;
	DWORD bytes_dst = 0;

	//---------------------------------------------------------------------------

	if
	(
		arrcheck( h_inst )
		&&
		!invalid_ptr( p_src )
		&&
		!invalid_ptr( p_dst )
		&&
		!invalid_ptr( p_src_size )
		&&
		!invalid_ptr( p_dst_size )
	)
	{
		PSAMPLESTATE p_sample_in	= &h_inst->sample_in;
		PSAMPLESTATE p_sample_out	= &h_inst->sample_out;

		PSAMPLESTATE p_sample_src	= h_inst->p_sample_src;
		PSAMPLESTATE p_sample_dst	= h_inst->p_sample_dst;
		
		BOOL b_need_peaks = flag_check_mask( h_inst->statistic_mode, CONV_STAT_GETPEAKS );

		//---------------------------------------------------------------------------

		loopinf
		{
			DWORD bytes_load;
			DWORD bytes_save;

			//---------------------------------------------------------------------------

			BP_START( CONV_LOAD );

			bytes_load =
				samples_load
					(
						p_sample_in,
						&p_src[bytes_src],
						*p_src_size - bytes_src,
						b_need_peaks
					);

			BP_STOP( CONV_LOAD );

			//---------------------------------------------------------------------------

			if( p_sample_in != p_sample_src )
			{
				BP_START( CONV_RATE_PRE );
				resampling_process( h_inst, p_sample_in, p_sample_src );
				BP_STOP( CONV_RATE_PRE );
			}

			if( p_sample_src != p_sample_dst )
			{
				BP_START( CONV_MAPPING );
				transform_matrix_processing( h_inst, p_sample_src, p_sample_dst );
				BP_STOP( CONV_MAPPING );
			}

			if( p_sample_dst != p_sample_out )
			{
				BP_START( CONV_RATE_POST );
				resampling_process( h_inst, p_sample_dst, p_sample_out );
				BP_STOP( CONV_RATE_POST );
			}

			//---------------------------------------------------------------------------

			BP_START( CONV_SAVE );

			bytes_save =
				samples_save
					(
						p_sample_out,
						&p_dst[bytes_dst],
						*p_dst_size - bytes_dst,
						b_need_peaks
					);

			BP_STOP( CONV_SAVE );

			//---------------------------------------------------------------------------

			bytes_src += bytes_load;
			bytes_dst += bytes_save;

			if( is_zero( bytes_load ) && is_zero( bytes_save ) )
				break;
		}
	}

	//---------------------------------------------------------------------------

	if( !invalid_ptr( p_src_size ) )
		*p_src_size = bytes_src;
	
	if( !invalid_ptr( p_dst_size ) )
		*p_dst_size = bytes_dst;
	
	return non_zero( bytes_src ) || non_zero( bytes_dst );
}


//================================================================
//  Set statistic mode
//----------------------------------------------------------------
BOOL convert_stat_mode( HCONVERT p_inst, DWORD mode, BOOL b_set )
{
	PCONVINST h_inst = (PCONVINST)p_inst;

	if( arrcheck( h_inst ) )
	{
		if( b_set )
			flag_set( h_inst->statistic_mode, mode );
		else
			flag_reset( h_inst->statistic_mode, mode );
		
		return TRUE;
	}

	return FALSE;
}


//================================================================
//  Get statistic
//----------------------------------------------------------------
BOOL convert_get_stat( HCONVERT p_inst, PCONVSTAT p_stat )
{
	PCONVINST h_inst = (PCONVINST)p_inst;

	//---------------------------------------------------------------------------

	if( arrcheck( h_inst ) && arrcheck( p_stat ) )
	{
		arrzero_unit( p_stat );

		//---------------------------------------------------------------------------

		if( flag_check_mask( h_inst->statistic_mode, CONV_STAT_GETMATRIX ) )
		{
			transform_matrix_export( h_inst, &p_stat->TransformMatrix );
			flag_set( p_stat->dwModeDone, CONV_STAT_GETMATRIX );
		}

		//---------------------------------------------------------------------------
		
		if
		(
			flag_check_mask( h_inst->statistic_mode, CONV_STAT_GETPEAKS )
			&&
			!invalid_ptr( h_inst->sample_in.p_peaks )
			&&
			!invalid_ptr( h_inst->sample_out.p_peaks )
		)
		{
			arrcpy_unit( &p_stat->InputPeaks, h_inst->sample_in.p_peaks );
			arrcpy_unit( &p_stat->OutputPeaks, h_inst->sample_out.p_peaks );
			
			flag_set( p_stat->dwModeDone, CONV_STAT_GETPEAKS );
		}

		//---------------------------------------------------------------------------
		
		if( flag_check_mask( h_inst->statistic_mode, CONV_STAT_GETINFO ) )
		{
			PCONVNFO p_info = &p_stat->Info;

			p_info->szTransformName	= h_inst->transform.transform_type;
			p_info->szWindowName	= h_inst->transform.window_type;

			p_info->nWindowLenIn	= h_inst->resampler_in.frame_len;
			p_info->nWindowLenOut	= h_inst->resampler_out.frame_len;

			p_info->bSync			=
				more_zero( h_inst->sample_out.hist_len )
				||
				( h_inst->resampler_in.frame_len == h_inst->resampler_out.frame_len );

			p_info->nDelay			=
				(INT)
				(
					D( 1000.0 )
					*
					D( h_inst->resampler_out.packet_len )
					/
					D( h_inst->resampler_out.rate )
					+
					D( 0.5 )
				);

			flag_set( p_stat->dwModeDone, CONV_STAT_GETINFO );
		}
		
		//---------------------------------------------------------------------------

		return TRUE;
	}

	return FALSE;
}
