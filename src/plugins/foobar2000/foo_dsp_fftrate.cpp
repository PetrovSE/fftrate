#include "foo_dsp_fftrate.h"


foo_dsp_fftrate::foo_dsp_fftrate( const foo_dsp_fftrate_params &params ):
	m_convert( NULL ),
	m_out( NULL ),
	m_rate( 0 ),
	m_chann( 0 ),
	m_map( 0 )
{
	m_rateOut	= params.rate();
	m_trans		= params.trans();
	m_wind		= params.wind();
}


foo_dsp_fftrate::~foo_dsp_fftrate( void )
{
	close();
}


void foo_dsp_fftrate::on_endoftrack( abort_callback &p_abort )
{
	flush();
}


void foo_dsp_fftrate::on_endofplayback( abort_callback &p_abort )
{
}


bool foo_dsp_fftrate::on_chunk( audio_chunk *chunk, abort_callback &p_abort )
{
	unsigned rate	= chunk->get_sample_rate();
	unsigned chann	= chunk->get_channels();
	unsigned map	= chunk->get_channel_config();

	if( rate != m_rate || chann != m_chann || map != m_map )
	{
		close();
		open( rate, chann, map );
	}

	if( m_convert )
	{
		audio_sample *data	= chunk->get_data();
		t_size count = chunk->get_sample_count();
		t_size total = count * chann * sizeof(audio_sample);

		audio_chunk *out = insert_chunk( count );

		free( m_out );
		m_out = (audio_sample *)malloc( total );
		memcpy( m_out, data, total );

		out->set_data( m_out, count, chann, rate, map );
	}

	return false;
}


void foo_dsp_fftrate::flush( void )
{
	if( m_convert )
		convert_reset( m_convert );
}


double foo_dsp_fftrate::get_latency( void )
{
	return 0.0;
}


bool foo_dsp_fftrate::need_track_change_mark( void )
{
	return false;
}


void foo_dsp_fftrate::close( void )
{
	m_convert = convert_close( m_convert );

	if( m_out )
	{
		free( m_out );
		m_out = NULL;
	}
}


void foo_dsp_fftrate::open( int p_rate, int p_chann, int p_map )
{
	WAVEFORMATEX *format_src = NULL;
	WAVEFORMATEX *format_dst = NULL;

	m_rate	= p_rate;
	m_chann	= p_chann;
	m_map	= p_map;

	while( 1 )
	{
		format_src = mmsys_format_pcm_fill
			(
				WAVE_FORMAT_IEEE_FLOAT,
				p_chann,
				p_rate,
				sizeof(audio_sample) * 8,
				0x0
			);

		if( format_src == NULL )
			break;

		format_dst = mmsys_format_dublicate( format_src );

		if( m_rateOut )
			format_dst = mmsys_format_pcm_change( format_dst, "r", m_rateOut );

		if( format_dst == NULL )
			break;

		mmsys_format_complete( format_dst );

		m_convert = convert_open( format_src, format_dst, "tw", m_trans, m_wind );
		break;
	}

	mmsys_format_free( format_src );
	mmsys_format_free( format_dst );
}
