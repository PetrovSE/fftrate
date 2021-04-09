#include "foo_dsp_fftrate.h"


foo_dsp_fftrate::foo_dsp_fftrate( void ):
	m_convert( NULL )
{
}


foo_dsp_fftrate::foo_dsp_fftrate( const foo_dsp_fftrate_params &params ):
	m_convert( NULL )
{
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
	unsigned chann		= chunk->get_channels();
	unsigned map		= chunk->get_channel_config();
	unsigned rate		= chunk->get_sample_rate();
	t_size count		= chunk->get_sample_count();
	audio_sample *data	= chunk->get_data();

	return true;
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
