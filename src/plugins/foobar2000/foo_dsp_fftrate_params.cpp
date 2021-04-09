#include "foo_dsp_fftrate.h"


const GUID &foo_dsp_fftrate_params::g_get_guid( void )
{
	static const GUID guid = { 0x806d2aed, 0x290a, 0x41a7, { 0x9c, 0x2f, 0x7f, 0xb7, 0xca, 0xec, 0x90, 0xbe } };
	return guid;
}


bool foo_dsp_fftrate_params::set_data( const dsp_preset &p_data )
{
	if( p_data.get_owner() != g_get_guid() )
		return false;

	const t_int32 *raw_data = static_cast <const t_int32 *>( p_data.get_data() );

	t_size dataLen = p_data.get_data_size() / sizeof(t_int32);
	dataLen = min( dataLen, m_paramCount );

	for( t_size n = 0 ; n < dataLen ; n ++ )
	{
		m_config[n] = raw_data[n];
		byte_order::order_le_to_native_t( m_config[n] );
	}

	return true;
}


bool foo_dsp_fftrate_params::get_data( dsp_preset &p_data )
{
	t_int32 temp[m_paramCount];
		
	p_data.set_owner( g_get_guid() );

	for( t_size n = 0 ; n < m_paramCount ; n ++ )
	{
		temp[n] = m_config[n];
		byte_order::order_native_to_le_t( temp[n] );
	}
		
	p_data.set_data( temp, m_paramCount * sizeof(t_int32) );
	return true;
}
