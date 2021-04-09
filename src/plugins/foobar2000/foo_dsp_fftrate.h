#ifndef _FOO_DSP_FFT_RATE_H_
#define _FOO_DSP_FFT_RATE_H_

#include <foobar2000/helpers/foobar2000+atl.h>
#include "swin.h"
#include "convert.h"
#include "resource.h"


struct t_foo_dsp_fftrate_desc
{
	const TCHAR	*name;
	t_int32		val;
};


class foo_dsp_fftrate_params
{
public:
	foo_dsp_fftrate_params( void ):
		m_rate( 0 ),
		m_bits( 0 ),
		m_chann( 0 ),
		m_trans( 1 ),
		m_wind( 2 )
	{
	}

	enum
	{
		BITS_FLAG_INT		= 0,
		BITS_FLAG_FLOAT
	};

	// The GUID that identifies this DSP and its configuration.
	static const GUID &g_get_guid( void );

	// Read data from a preset.
	bool set_data( const dsp_preset &p_data );
	bool get_data( dsp_preset &p_data );

	void set_rate( t_int32 p_rate )
	{
		m_rate = p_rate;
	}

	void set_bits( t_int32 p_bits )
	{
		m_bits = p_bits;
	}

	void set_chann( t_int32 p_chann )
	{
		m_chann = p_chann;
	}

	void set_trans( t_int32 p_trans )
	{
		m_trans = p_trans;
	}

	void set_wind( t_int32 p_wind )
	{
		m_wind = p_wind;
	}

	t_int32 rate( void ) const
	{
		return m_rate;
	}

	t_int32 bits( void ) const
	{
		return m_bits;
	}

	t_int32 chann( void ) const
	{
		return m_chann;
	}

	t_int32 trans( void ) const
	{
		return m_trans;
	}

	t_int32 wind( void ) const
	{
		return m_wind;
	}

private:
	static const t_size	m_paramCount = 5;

	union
	{
		t_int32	m_config[1];

		struct 
		{
			t_int32	m_rate;
			t_int32	m_bits;
			t_int32	m_chann;
			t_int32	m_trans;
			t_int32	m_wind;
		};
	};
};


class foo_dsp_fftrate_dialog: public CDialogImpl <foo_dsp_fftrate_dialog>
{
public:
	enum
	{
		IDD = IDD_CONFIG
	};
 
	BEGIN_MSG_MAP_EX( foo_dsp_fftrate_dialog )
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_COMMAND( OnCommand )
	END_MSG_MAP()

	foo_dsp_fftrate_dialog( foo_dsp_fftrate_params &p_params );


protected:
	BOOL OnInitDialog( CWindow wndFocus, LPARAM lInitParam );
	void OnCommand( UINT uNotifyCode, int nID, CWindow wndCtl );

private:
	foo_dsp_fftrate_params &m_params;

	void combo_upload( int id, const t_foo_dsp_fftrate_desc *desc, t_int32 curr );
	t_int32 combo_download( int id, const t_foo_dsp_fftrate_desc *desc );
};


class foo_dsp_fftrate: public dsp_impl_base_t<dsp_v2>
{
public:
	foo_dsp_fftrate( void );
	foo_dsp_fftrate( const foo_dsp_fftrate_params &params );
	~foo_dsp_fftrate( void );

	void flush( void );
	double get_latency( void );
	bool need_track_change_mark( void );

protected:
	void on_endoftrack( abort_callback &p_abort );
	void on_endofplayback( abort_callback &p_abort );
	bool on_chunk( audio_chunk *chunk, abort_callback &p_abort );

private:
	HCONVERT	m_convert;

	void close( void );
};

#endif //_FOO_DSP_FFT_RATE_H_
