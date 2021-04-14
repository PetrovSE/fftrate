#include "foo_dsp_fftrate.h"

#define SELF "[foo_dsp_fftrate]"


foo_dsp_fftrate::foo_dsp_fftrate( const foo_dsp_fftrate_params &params ):
	m_convert( NULL ),
	m_rate( 0 ),
	m_chann( 0 ),
	m_map( 0 )
{
	m_transRate		= params.rate();
	m_transChann	= 0;

	m_trans	= params.trans();
	m_wind	= params.wind();
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
	flush();
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

	if( m_convert == NULL )
		return true;

	DWORD total = chunk->get_sample_count() * chann * sizeof(audio_sample);
	CONST BYTE *data = reinterpret_cast <CONST BYTE *>( chunk->get_data() );

	while( 1 )
	{
		DWORD src_len = total;
		DWORD dst_len = m_bufferSize;

		convert_processing
			(	
				m_convert,
				data,
				&src_len,
				m_buffer,
				&dst_len	
			);

		if( src_len == 0 && dst_len == 0 )
			break;

		if( src_len )
		{
			data  += src_len;
			total -= src_len;
		}

		if( dst_len )
		{
			t_size samples   = dst_len / sizeof(audio_sample);
			audio_chunk *out = insert_chunk( samples );

			out->set_data
				(
					reinterpret_cast <audio_sample *>( m_buffer ),
					samples / m_outChann,
					m_outChann,
					m_outRate
				);
		}
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

		if( m_transRate )
			format_dst = mmsys_format_pcm_change( format_dst, "r", m_transRate );

		if( m_transChann )
			format_dst = mmsys_format_pcm_change( format_dst, "c", m_transChann );

		if( format_dst == NULL )
			break;

		mmsys_format_complete( format_dst );

		m_convert = convert_open( format_src, format_dst, "tw", m_trans, m_wind );
		if( m_convert == NULL )
			break;

		convert_reset( m_convert );
		break;
	}

	m_outRate	= m_transRate  ? m_transRate  : p_rate;
	m_outChann	= m_transChann ? m_transChann : p_chann;

	mmsys_format_free( format_src );
	mmsys_format_free( format_dst );
}
