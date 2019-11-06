#include <stdio.h>
#include <stdarg.h>

#include <alsa/asoundlib.h>
#include <alsa/pcm_rate.h>

#include "swin.h"
#include "types.h"
#include "array.h"
#include "convert.h"
#include "mathex.h"
#include "mmsys.h"
#include "waveio.h"
#include "cmdline.h"
#include "inifile.h"

//--------------------------------------------------------------------------------------------

typedef struct
{
	WORD	format_tag;
	int		rate;
	int		format;

	BYTE	*data;
	BYTE	fill;

	int		sample_size;
	int		unit_size;

	int		buffer_size;
	int		period_len;

	int		rem;
	int		dummy;

} conv_data;


typedef struct
{
	void		*conv_inst;

	conv_data	src;
	conv_data	dst;

	int			channels;
	BOOL		started;
	BOOL		zummer;

#ifdef DUMP
	HWAVEIO		f_in;
	HWAVEIO		f_out;
#endif

} conv_state;


//--------------------------------------------------------------------------------------------

#define SELFNAME		"fftrate"
#define TITLE			"Converter: " SELFNAME
#define CONF			"/etc/" SELFNAME ".conf"

#define OUTF			stderr
#define TMPD			"/tmp"

//--------------------------------------------------------------------------------------------

#define channel_processing( _name, _arg )	\
static void channel_##_name					\
(											\
	const snd_pcm_channel_area_t	*area,	\
	int		offset,							\
	int		dummy,							\
											\
	BYTE	*inter,							\
	int		step,							\
											\
	int		size,							\
	int		len								\
)											\
{											\
	int n, k;								\
	int dist	= area->step / 8;			\
	BYTE *data	=							\
		(BYTE *)area->addr					\
		+									\
		( area->first / 8 )					\
		+									\
		dist * offset;						\
											\
	for( n = 0 ; n < size ; n ++ )			\
	{										\
		BYTE *p_src = data  ++;				\
		BYTE *p_dst = inter ++;				\
											\
		for( k = 0 ; k < len ; k ++ )		\
		{									\
			_arg;							\
			p_src += dist;					\
			p_dst += step;					\
		}									\
	}										\
											\
	for( n = 0 ; n < dummy ; n ++ )			\
	{										\
		BYTE *p_src = data ++;				\
											\
		for( k = 0 ; k < len ; k ++ )		\
		{									\
			*p_src = 0;						\
			p_src += dist;					\
		}									\
	}										\
}

channel_processing( interleaving,   *p_dst = *p_src )
channel_processing( deinterleaving, *p_src = *p_dst )

//--------------------------------------------------------------------------------------------

#ifdef DUMP
static HWAVEIO wave_file_open( int id, const char *postfix, const WAVEFORMATEX *fmt )
{
	char name[MAX_STRING];
	HWAVEIO h_file;

	//--------------------------------------------------------------------------------------------

	sprintf( name, "%s/%s_%d_%s.wav", TMPD, SELFNAME, id, postfix );
	h_file = waveio_open( name, WAVE_IO_WRITE );

	if( h_file == NULL )
		return NULL;

	//--------------------------------------------------------------------------------------------

	waveio_store_chunk( h_file, FCC_FMT );
	waveio_write( h_file, (PBYTE)fmt, mmsys_format_info( fmt, 's' ) );
	waveio_store_chunk( h_file, FCC_DATA );

	return h_file;
}


static void wave_file_close( HWAVEIO h_file )
{
	if( h_file )
		waveio_close( h_file );
}
#endif

//--------------------------------------------------------------------------------------------

static int data_setup( conv_data *data, snd_pcm_rate_side_info_t *info, int n_channels )
{
	const char *fmt_name = NULL;

	arrzero_unit( data );

	if( info->period_size == 0 || info->period_size > info->rate )
		return -1;

	//--------------------------------------------------------------------------------------------

	switch( info->format )
	{
	case SND_PCM_FORMAT_U8:
		data->format_tag	= WAVE_FORMAT_PCM;
		data->sample_size	= 1;
		data->dummy			= 0;
		data->fill			= 0x80;
		fmt_name			= "u8";
		break;

	case SND_PCM_FORMAT_S16_LE:
		data->format_tag	= WAVE_FORMAT_PCM;
		data->sample_size	= 2;
		data->dummy			= 0;
		data->fill			= 0x00;
		fmt_name			= "s16_le";
		break;

	case SND_PCM_FORMAT_S24_3LE:
		data->format_tag	= WAVE_FORMAT_PCM;
		data->sample_size	= 3;
		data->dummy			= 0;
		data->fill			= 0x00;
		fmt_name			= "s24_3le";
		break;

	case SND_PCM_FORMAT_S24_LE:
		data->format_tag	= WAVE_FORMAT_PCM;
		data->sample_size	= 3;
		data->dummy			= 1;
		data->fill			= 0x00;
		fmt_name			= "s24_le";
		break;

	case SND_PCM_FORMAT_S32_LE:
		data->format_tag	= WAVE_FORMAT_PCM;
		data->sample_size	= 4;
		data->dummy			= 0;
		data->fill			= 0x00;
		fmt_name			= "s32_le";
		break;

	case SND_PCM_FORMAT_FLOAT_LE:
	case SND_PCM_FORMAT_FLOAT64_LE:
	default:
		fmt_name			= "unsupported";
		break;
	}

	//--------------------------------------------------------------------------------------------

	if( data->sample_size )
	{
		data->unit_size		= data->sample_size * n_channels;
		data->period_len	= info->period_size;
		data->buffer_size	= 2 * data->period_len * data->unit_size;

		data->rem			= 0;
		data->rate			= info->rate;
		data->format		= info->format;

		arralloc( data->data, data->buffer_size );
	}

	//--------------------------------------------------------------------------------------------

	fprintf
		(
			OUTF,
			"%d Hz, %d ch, '%s' (0x%x): dummy = %d, period = %d\n",
			data->rate,
			n_channels,
			fmt_name,
			info->format,
			data->dummy,
			data->period_len
		);

	//--------------------------------------------------------------------------------------------

	return ( data->data == NULL ) ? -1 : 0;
}

//--------------------------------------------------------------------------------------------

static snd_pcm_uframes_t pcm_rate_input_frames( void *obj, snd_pcm_uframes_t frames )
{
	conv_state *inst = obj;

	dbg_printf( "[pcm_rate_input_frames]: %d ->", (int)frames );
	frames = (snd_pcm_uframes_t)( ( (double)frames * (double)inst->src.period_len ) / (double)inst->dst.period_len );

	dbg_printf( " %d\n", (int)frames );
	return frames;
}


static snd_pcm_uframes_t pcm_rate_output_frames( void *obj, snd_pcm_uframes_t frames )
{
	conv_state *inst = obj;

	dbg_printf( "[pcm_rate_output_frames]: %d ->", (int)frames );
	frames = (snd_pcm_uframes_t)( ( (double)frames * (double)inst->dst.period_len ) / (double)inst->src.period_len );

	dbg_printf( " %d\n", (int)frames );
	return frames;
}


static void pcm_rate_free( void *obj )
{
	conv_state *inst = obj;

	dbg_printf( "[pcm_rate_free]\n" );

#ifdef DUMP
	wave_file_close( inst->f_in );
	wave_file_close( inst->f_out );
#endif

	if( inst->conv_inst )
		convert_close( inst->conv_inst );

	if( inst->src.data )
		arrfree( inst->src.data );

	if( inst->dst.data )
		arrfree( inst->dst.data );

	arrzero_unit( inst );
}


static void pcm_rate_reset( void *obj )
{
	conv_state *inst = obj;

	dbg_printf( "[pcm_rate_reset]\n" );

	if( inst->conv_inst )
		convert_reset( inst->conv_inst );

	inst->src.rem = 0;
	inst->dst.rem = 0;

	inst->started = FALSE;
}


static int pcm_rate_init( void *obj, snd_pcm_rate_info_t *info )
{
	conv_state *inst = obj;
	BOOL ok = FALSE;

	WAVEFORMATEX *p_in_format  = NULL;
	WAVEFORMATEX *p_out_format = NULL;

	DWORD trans_type	= CONV_TRANSFORM_DCT;
	DWORD wind_type		= CONV_WINDOW_VORBIS;

	//--------------------------------------------------------------------------------------------

	dbg_printf( "[pcm_rate_init]\n" );
	pcm_rate_free( obj );

	//--------------------------------------------------------------------------------------------

	{
		CHAR trans_name[MAX_STRING]	= "";
		CHAR wind_name[MAX_STRING]	= "";
		BOOL zummer = FALSE;

		INI_BEGIN( init_value )
			SET_STR(	"transform",	trans_name,	MAX_STRING	),
			SET_STR(	"window",		wind_name,	MAX_STRING	),
			SET_BOOL(	"zummer",		zummer,		INI_BOOL_STYLE_ONOFF ),
		INI_END

		FILE *fp_conf = inifile_open( CONF, FILE_OPEN_READ );

		dbg_printf( " load config... " );

		if( fp_conf )
		{
			dbg_printf( "Ok.\n" );

			inifile_load( fp_conf, init_value );
			fp_conf = inifile_close( fp_conf );

			dbg_printf( " transform name : '%s'\n", trans_name );
			dbg_printf( " window name    : '%s'\n", wind_name );


			if( strcasecmp( trans_name, "dct" ) == 0 || strlen( trans_name ) == 0 )
				trans_type = CONV_TRANSFORM_DCT;

			if( strcasecmp( trans_name, "fft" ) == 0 )
				trans_type = CONV_TRANSFORM_FFT;


			if( strcasecmp( wind_name, "vorbis" ) == 0 || strlen( wind_name ) == 0 )
				wind_type = CONV_WINDOW_VORBIS;

			if( strcasecmp( wind_name, "hanning" ) == 0 )
				wind_type = CONV_WINDOW_SIN;

			if( strcasecmp( wind_name, "rect" ) == 0 )
				wind_type = CONV_WINDOW_NONE;

			inst->zummer = zummer;
		}
		else
			dbg_printf( "Failed!\n" );
	}

	//--------------------------------------------------------------------------------------------

	while( 1 )
	{
		int in_rate  = info->in.rate;
		int out_rate = info->out.rate;

		if( in_rate == 0 || out_rate == 0 )
			break;

		//--------------------------------------------------------------------------------------------

		inst->channels = info->channels;

		fprintf( OUTF, "Input:  " );
		if( data_setup( &inst->src, &info->in, info->channels ) < 0 )
			break;

		fprintf( OUTF, "Output: " );
		if( data_setup( &inst->dst, &info->out, info->channels ) < 0 )
			break;

		//--------------------------------------------------------------------------------------------

		if( ( in_rate * info->out.period_size ) != ( out_rate * info->in.period_size ) )
		{
			double l_in  = log2( (double)in_rate  / (double)info->in.period_size );
			double l_out = log2( (double)out_rate / (double)info->out.period_size );
			int   fpp   = (int)( 0.5 * ( l_in + l_out ) );

			fpp = max( fpp, 0 );
			in_rate	 = info->in.period_size  << fpp;
			out_rate = info->out.period_size << fpp;
		}

		//--------------------------------------------------------------------------------------------

		p_in_format = mmsys_format_pcm_fill
			(
				inst->src.format_tag,
				info->channels,
				in_rate,
				inst->src.sample_size * 8,
				0x0
			);

		if( p_in_format == NULL )
			break;

		//--------------------------------------------------------------------------------------------

		p_out_format = mmsys_format_pcm_fill
			(
				inst->dst.format_tag,
				info->channels,
				out_rate,
				inst->dst.sample_size * 8,
				0x0
			);

		if( p_out_format == NULL )
			break;

		//--------------------------------------------------------------------------------------------

	#ifdef DUMP
		{
			int num = time( NULL );

			inst->f_in  = wave_file_open( num, "in", p_in_format );
			inst->f_out = wave_file_open( num, "out", p_out_format );
		}
	#endif

		//--------------------------------------------------------------------------------------------

		inst->conv_inst = convert_open( p_in_format, p_out_format, "tws", trans_type, wind_type, FALSE );
		if( inst->conv_inst == NULL )
			break;

		//--------------------------------------------------------------------------------------------

		pcm_rate_reset( obj );

		//--------------------------------------------------------------------------------------------

		fprintf( OUTF, "Rates:  %d --> %d", in_rate, out_rate );

		{
			CONVSTAT conv_info;
			
			double c1 = (double)info->out.rate / (double)info->in.rate;
			double c2 = (double)out_rate / (double)in_rate;

			convert_stat_mode( inst->conv_inst, CONV_STAT_GETINFO, TRUE );
			convert_get_stat( inst->conv_inst, &conv_info );

			fprintf( OUTF, " (J: %.2f%%", 100.0 * fabs( c1 - c2 ) / c1 );

			if( flag_check_mask( conv_info.dwModeDone, CONV_STAT_GETINFO ) )
				fprintf( OUTF, ", T: %s, W: %s", conv_info.Info.szTransformName, conv_info.Info.szWindowName );

			if( inst->zummer )
				fprintf( OUTF, ", Z" );

			fprintf( OUTF, ")" );
		}

		fprintf( OUTF, "\nOk.\n\n" );
		ok = TRUE;

		break;
	}

	//--------------------------------------------------------------------------------------------

	mmsys_format_free( p_in_format );
	mmsys_format_free( p_out_format );

	if( ok )
		return 0;

	fprintf( OUTF, "Failed!\n\n" );
	pcm_rate_free( obj );

	return -EINVAL;
}


static int pcm_rate_adjust_pitch( void *obj, snd_pcm_rate_info_t *info )
{
	conv_state *inst = obj;
	conv_data  *src  = &inst->src;
	conv_data  *dst  = &inst->dst;

	dbg_printf( "[pcm_rate_adjust_pitch]:\n" );
	dbg_printf( " channels: %d -> %d\n", inst->channels, info->channels );

	dbg_printf( " src.rate: %d -> %d\n", src->rate, info->in.rate );
	dbg_printf( " src.fmt : %04x -> %04x\n", src->format, info->in.format );

	dbg_printf( " dst.rate: %d -> %d\n", dst->rate, info->out.rate );
	dbg_printf( " dst.fmt : %04x -> %04x\n", dst->format, info->out.format );

	if
	(
		inst->channels != info->channels
		||
		src->rate != info->in.rate || src->format != info->in.format
		||
		dst->rate != info->out.rate || dst->format != info->out.format
	)
	{
		return pcm_rate_init( obj, info );
	}

	return 0;
}


static void pcm_zummer( const snd_pcm_channel_area_t *area, int offset, int sample_size, BYTE fill )
{
	BYTE *data = (BYTE *)area->addr + ( area->first + offset * area->step ) / 8;
	int n;

	//--------------------------------------------------------------------------------------------

	dbg_printf( "[pcm_zummer]:\n" );
	dbg_printf( " first : %d bit\n", (int)area->first );
	dbg_printf( " step  : %d bit\n", (int)area->step );
	dbg_printf( " offset: %d\n", offset );

	//--------------------------------------------------------------------------------------------

	for( n = 1 ; n < sample_size ; n ++ )
		( *data ++ ) = 0xff;
	*data = 0x7f + fill;
}


static void pcm_rate_convert
(
	void							*obj,
	const snd_pcm_channel_area_t	*dst_areas,
	snd_pcm_uframes_t				dst_offset,
	unsigned int					dst_frames,
	const snd_pcm_channel_area_t	*src_areas,
	snd_pcm_uframes_t				src_offset,
	unsigned int					src_frames
)
{
	conv_state *inst = obj;
	conv_data  *src  = &inst->src;
	conv_data  *dst  = &inst->dst;
	BOOL zummer = inst->zummer;

	//--------------------------------------------------------------------------------------------

	dbg_printf( "[pcm_rate_convert]:\n" );
	dbg_printf( " offset: %d -> %d\n", (int)src_offset, (int)dst_offset );
	dbg_printf( " frames: %d -> %d\n", (int)src_frames, (int)dst_frames );

	src_frames *= src->unit_size;
	dst_frames *= dst->unit_size;

	dbg_printf( " bytes : %d -> %d\n", (int)src_frames, (int)dst_frames );

	//--------------------------------------------------------------------------------------------

	while( 1 )
	{
		int working = FALSE;
		int frames, ch;

		//--------------------------------------------------------------------------------------------

		frames = min( src->buffer_size - src->rem, src_frames ) / src->unit_size;

		for( ch = 0 ; ch < inst->channels ; ch ++ )
		{
			channel_interleaving
				(
					&src_areas[ch],
					src_offset,
					0,
					&src->data[src->rem + ch * src->sample_size],
					src->unit_size,
					src->sample_size,
					frames
				);
		}

		src_offset	+= frames;
		frames		*= src->unit_size;

		//--------------------------------------------------------------------------------------------

	#ifdef DUMP
		if( inst->f_in )
			waveio_write( inst->f_in, &src->data[src->rem], frames );
	#endif

		src_frames	-= frames;
		src->rem	+= frames;
		working		|= ( frames != 0 );

		dbg_printf( " loaded = %d, buff = %d, left = %d\n", frames, src->rem, src_frames );

		//--------------------------------------------------------------------------------------------

		{
			DWORD in_size  = src->rem;
			DWORD out_size = dst->buffer_size - dst->rem;

			dbg_printf( " (%d, %d) ->", (int)in_size, (int)out_size );

			convert_processing
				(
					inst->conv_inst,
					src->data,
					&in_size,
					&dst->data[dst->rem],
					&out_size
				);

			dbg_printf( " (%d, %d)", (int)in_size, (int)out_size );

			//--------------------------------------------------------------------------------------------

			src->rem -= in_size;
			dst->rem += out_size;
			working  |= ( in_size != 0 ) | ( out_size != 0 );
			arrcpy( src->data, &src->data[in_size], src->rem );

			//--------------------------------------------------------------------------------------------

			if( !inst->started )
			{
				if( out_size )
				{
					dbg_printf( " [started]" );
					inst->started = TRUE;
				}

				if( dst->rem < dst->buffer_size )
				{
					int upd_size = dst->buffer_size - dst->rem;

					dbg_printf( " [fake: %d + %d]", (int)dst->rem, upd_size );

					arrmove( &dst->data[upd_size], dst->data, dst->rem );
					memset( dst->data, dst->fill, upd_size );

					out_size += upd_size;
					dst->rem += upd_size;
				}
			}

			//--------------------------------------------------------------------------------------------

			dbg_printf( " -> done = %d\n", dst->rem );
		}

		//--------------------------------------------------------------------------------------------

		frames = min( dst->rem, dst_frames ) / dst->unit_size;

		for( ch = 0 ; ch < inst->channels ; ch ++ )
		{
			channel_deinterleaving
				(
					&dst_areas[ch],
					dst_offset,
					dst->dummy,
					&dst->data[ch * dst->sample_size],
					dst->unit_size,
					dst->sample_size,
					frames
				);

			if( zummer )
				pcm_zummer( &dst_areas[ch], dst_offset, dst->sample_size, dst->fill );
		}

		dst_offset	+= frames;
		frames		*= dst->unit_size;
		zummer		 = FALSE;

		//--------------------------------------------------------------------------------------------

	#ifdef DUMP
		if( inst->f_out )
			waveio_write( inst->f_out, dst->data, frames );
	#endif

		dst_frames	-= frames;
		dst->rem	-= frames;
		working		|= ( frames != 0 );
		arrcpy( dst->data, &dst->data[frames], dst->rem );

		dbg_printf( " saved: %d, buff = %d, left: %d\n", frames, dst->rem, dst_frames );
		
		//--------------------------------------------------------------------------------------------

		if( !working )
		{
			dbg_printf( " break\n" );
			break;
		}
	}

	//--------------------------------------------------------------------------------------------

	if( src_frames )
		fprintf( OUTF, "WARNING(%s): Input buffer is overflown: %d\n", SELFNAME, src_frames );

	if( dst_frames )
		fprintf( OUTF, "WARNING(%s): Output buffer is underflown: %d\n", SELFNAME, dst_frames );
}


static void pcm_rate_close( void *obj )
{
	dbg_printf( "[pcm_rate_close]\n" );
	pcm_rate_free( obj );
	arrfree( obj );
}


#if SND_PCM_RATE_PLUGIN_VERSION >= 0x010002
static int pcm_rate_rates
(
	void			*obj,
	unsigned int	*rate_min,
	unsigned int	*rate_max
)
{
	dbg_printf( "[pcm_rate_rates]\n" );
	*rate_min = *rate_max = 0; // both unlimited
	return 0;
}


static void pcm_rate_dump( void *obj, snd_output_t *out )
{
	dbg_printf( "[pcm_rate_dump]\n" );
	snd_output_printf( out, TITLE "\n" );
}
#endif


static const snd_pcm_rate_ops_t pcm_rate_ops =
{
	.close			= pcm_rate_close,
	.init			= pcm_rate_init,
	.free			= pcm_rate_free,
	.reset			= pcm_rate_reset,
	.adjust_pitch	= pcm_rate_adjust_pitch,
	.convert		= pcm_rate_convert,
	.convert_s16	= NULL,
	.input_frames	= pcm_rate_input_frames,
	.output_frames	= pcm_rate_output_frames,

#if SND_PCM_RATE_PLUGIN_VERSION >= 0x010002
	.version				= SND_PCM_RATE_PLUGIN_VERSION,
	.get_supported_rates	= pcm_rate_rates,
	.dump					= pcm_rate_dump,
#endif
};


int SND_PCM_RATE_PLUGIN_ENTRY( fftrate )
(
	unsigned int 		version,
	void				**objp,
	snd_pcm_rate_ops_t	*ops
)
{
	conv_state *inst;

	dbg_printf( "[pcm_rate_open]\n" );

#if SND_PCM_RATE_PLUGIN_VERSION < 0x010002
	if( version != SND_PCM_RATE_PLUGIN_VERSION )
	{
		fprintf( OUTF, "Invalid rate plugin version %x!\n", version );
		return -EINVAL;
	}
#endif

	arralloc_unit( inst );
	if( inst  == NULL )
		return -ENOMEM;

	arrzero_unit( inst );
	*objp = inst;

#if SND_PCM_RATE_PLUGIN_VERSION >= 0x010002
	if( version == 0x010001 )
		arrcpy_bytes( ops, &pcm_rate_ops, sizeof(snd_pcm_rate_old_ops_t) )
	else
#endif
		*ops = pcm_rate_ops;

	return 0;
}
