//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#include "swin.h"
#include "types.h"
#include "array.h"
#include "str.h"

#if !defined(IS_LINUX)

//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "conf.h"


BOOL test_parameters( CONST CONFPARAMS *param, BOOL b_out )
{
	printf( " Pass\n" );
	return TRUE;
}


INT enum_devices( CONST CARDNAME *p_card, CARDNAME *p_devs, INT n_dev, BOOL b_out  )
{
	INT n_add;
	
	n_dev = min( n_dev, 4 );
	
	for( n_add = 0 ; n_add < n_dev ; n_add ++ )
	{
		arrzero_unit( &p_devs[n_add] );
		p_devs[n_add].dev_out = n_add;

		snprintf
			(
				p_devs[n_add].desc,
				MAX_STRING,
				"%s,%d: %s",
				p_card->name,
				n_add,
				"Fake device"
			);
	}
	
	return n_add;
}


VOID update_mixer( VOID )
{
	printf( "Mixer updated.\n" );
}

#else

//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#include <alsa/asoundlib.h>
#include "conf.h"


//=============================================================================
// Functions
//-----------------------------------------------------------------------------
STATIC VOID print_result( INT res )
{
	if( res < 0 )
		printf( "Unsupported!\n" );
	else
		printf( "Ok.\n" );
}


BOOL test_parameters( CONST CONFPARAMS *param, BOOL b_out )
{
	PCARDNAME p_card = param->p_card;

	snd_pcm_t *h_pcm = NULL;
	snd_pcm_hw_params_t	*hw_params = NULL;

	CHAR name[MAX_STRING];
	BOOL ret = FALSE;

	//-----------------------------------------------------------------------------
	
	device_name( p_card, b_out, name, MAX_STRING );
	
	//-----------------------------------------------------------------------------
	
	while( 1 )
	{
		BOOL b_incorrect = FALSE;
		
		snd_pcm_uframes_t t_period, t_period_syn;
		snd_pcm_uframes_t t_buffer, t_buffer_syn;
		
		//-----------------------------------------------------------------------------
		
		printf( " Open device \"%s\" ... ", name );
		if( snd_pcm_open( &h_pcm, name, b_out ? SND_PCM_STREAM_PLAYBACK : SND_PCM_STREAM_CAPTURE, 0 ) < 0 )
			break;
			
		if( snd_pcm_hw_params_malloc( &hw_params ) < 0 )
			break;

		if( snd_pcm_hw_params_any( h_pcm, hw_params ) < 0 )
			break;
		
		print_result( 1 );
		
		//-----------------------------------------------------------------------------
		
		printf( " Set rate %d Hz ... ", param->rate );
		print_result( snd_pcm_hw_params_set_rate( h_pcm, hw_params, param->rate, 0 ) );
		
		//-----------------------------------------------------------------------------
		
		if( b_out )
		{
			printf( " Set channels %d ... ", param->chann );
			print_result( snd_pcm_hw_params_set_channels( h_pcm, hw_params, param->chann ) );
		}
		
		//-----------------------------------------------------------------------------
		
		printf( " Set format '%s' ... ", param->format.name );
		print_result( snd_pcm_hw_params_set_format( h_pcm, hw_params, param->format.val ) );
		
		//-----------------------------------------------------------------------------
		
		t_period = t_period_syn = param->period * param->period_mul;
		t_buffer = t_buffer_syn = calculate_buffer( param->rate, param->period ) * param->buff_mul;
		
		printf( " Set buffer size %d -> ", (int)t_buffer );
		if( snd_pcm_hw_params_set_buffer_size_near( h_pcm, hw_params, &t_buffer_syn ) < 0 )
			break;
			
		b_incorrect |= t_buffer != t_buffer_syn;
		printf( "%d\n", (int)t_buffer_syn );

		printf( " Set period size %d -> ", (int)t_period );
		if( snd_pcm_hw_params_set_period_size_near( h_pcm, hw_params, &t_period_syn, 0 ) < 0 )
			break;
			
		b_incorrect |= t_period != t_period_syn;
		printf( "%d\n", (int)t_period_syn );
		
		if( b_incorrect )
			printf( "Warning: some parameters were changed!\n" );
			
		//-----------------------------------------------------------------------------
		
		ret = TRUE;
		break;
	}
	
	//-----------------------------------------------------------------------------
	
	if( !ret )
		printf( "Failed!\n" );
		
	if( !invalid_ptr( hw_params ) )
		snd_pcm_hw_params_free( hw_params );
		
	if( !invalid_ptr( h_pcm ) )
		snd_pcm_close( h_pcm );
		
	return ret;
}


STATIC VOID zero_handler
(
	CONST CHAR	*file		ATTRIBUTE_UNUSED,
	INT			line		ATTRIBUTE_UNUSED,
	CONST CHAR	*function	ATTRIBUTE_UNUSED,
	INT			err			ATTRIBUTE_UNUSED,
	CONST CHAR	*fmt		ATTRIBUTE_UNUSED,
	...
)
{
}


INT enum_devices( CONST CARDNAME *p_card, CARDNAME *p_devs, INT n_dev, BOOL b_out  )
{
	snd_pcm_info_t	*pcm_info	= NULL;
	snd_ctl_t		*ctl		= NULL;
	INT n_add = 0;
	
	//-----------------------------------------------------------------------------
	
	loopinf
	{
		CHAR card[MAX_STRING];
		INT  pcm_device = -1;
		
		card_name( p_card, card, MAX_STRING );
		if( less_zero( snd_ctl_open( &ctl, card, 0 ) ) )
			break;
		
		snd_pcm_info_malloc( &pcm_info );
		if( invalid_ptr( pcm_info ) )
			break;
		
		//-----------------------------------------------------------------------------
		
		loopinf
		{
			if( less_zero( snd_ctl_pcm_next_device( ctl, &pcm_device ) ) || less_zero( pcm_device ) )
				break;
			
			snd_pcm_info_set_device( pcm_info, pcm_device );
			snd_pcm_info_set_subdevice( pcm_info, 0 );
			snd_pcm_info_set_stream( pcm_info, b_out ? SND_PCM_STREAM_PLAYBACK : SND_PCM_STREAM_CAPTURE );
			
			if( less_zero( snd_ctl_pcm_info( ctl, pcm_info ) ) )
				continue;
			
			if( n_add < n_dev )
			{
				arrzero_unit( &p_devs[n_add] );
				p_devs[n_add].dev_out = pcm_device;
				
				snprintf
					(
						p_devs[n_add].desc,
						MAX_STRING,
						"%s,%d: %s",
						p_card->name,
						pcm_device,
						snd_pcm_info_get_name( pcm_info )
					);
				
				n_add ++;
			}
		}
		
		break;
	}
	
	//-----------------------------------------------------------------------------
	
	if( !invalid_ptr( pcm_info ) )
		snd_pcm_info_free( pcm_info );
	
	if( !invalid_ptr( ctl ) )
		snd_ctl_close( ctl );
	
	return n_add;
}


VOID update_mixer( VOID )
{
	CONST INT direct[2] = { SND_PCM_STREAM_PLAYBACK, SND_PCM_STREAM_CAPTURE };
	CONST CHAR *devs[]  = 
	{
		"default",
		"capture",
		NULL,
	};
	
	INT n, k;

	snd_lib_error_handler_t err_handler = snd_lib_error;
	snd_lib_error_set_handler( &zero_handler );

	//-----------------------------------------------------------------------------
	
	for( n = 0 ; n < 2 ; n ++ )
	{
		printf( "Test: " );
		
		for( k = 0 ; !invalid_ptr( devs[k] ) ; k ++ )
		{
			snd_pcm_t *h_pcm = NULL;
			printf( "'%s' ... ", devs[k] );

			if( more_eq_zero( snd_pcm_open( &h_pcm, devs[k], direct[n], 0 ) ) && !invalid_ptr( h_pcm ) )
			{
				snd_pcm_close( h_pcm );
				printf( "Ok.\n" );
				break;
			}
		}

		if( invalid_ptr( devs[k] ) )
			printf( "Failed!\n" );
	}
	
	snd_lib_error_set_handler( err_handler );
}

#endif // IS_LINUX
