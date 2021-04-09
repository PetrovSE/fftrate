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
