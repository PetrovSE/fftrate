#include <foobar2000/helpers/foobar2000+atl.h>
#include "resource.h"


DECLARE_COMPONENT_VERSION
(
	"DSP FFT rate",
	"1.0.1",
	"FFT-based sample rate converter for foobar2000\n"
	"Written by Petrov Sergey\n\n"
	"e-mail: petrovse@mail.ru\n"
	"url: https://github.com/PetrovSE/fftrate"
);


struct t_foo_dsp_fftrate_value
{
	const TCHAR	*name;
	t_int32		val;
};


static const t_foo_dsp_fftrate_value g_hRates[] =
{
	{ _T( "By pass" ),	0		},
	{ _T( "8000" ),		8000	},
	{ _T( "11025" ),	11025	},
	{ _T( "16000" ),	16000	},
	{ _T( "22050" ),	22050	},
	{ _T( "32000" ),	32000	},
	{ _T( "44100" ),	44100	},
	{ _T( "48000" ),	48000	},
	{ _T( "64000" ),	64000	},
	{ _T( "88200" ),	88200	},
	{ _T( "96000" ),	96000	},
	{ NULL,				0		}
};

static const t_foo_dsp_fftrate_value g_hBits[] =
{
	{ _T( "By pass" ),	0	},
	{ _T( "8" ),		8	},
	{ _T( "16" ),		16	},
	{ _T( "24" ),		24	},
	{ _T( "32" ),		32	},
	{ NULL,				0	}
};

static const t_foo_dsp_fftrate_value g_hChann[] =
{
	{ _T( "By pass" ),	0	},
	{ _T( "Mono" ),		1	},
	{ _T( "Stereo" ),	2	},
	{ NULL,				0	}
};

static const t_foo_dsp_fftrate_value g_hTrans[] =
{
	{ _T( "FFT" ),		1	},
	{ _T( "DCT" ),		2	},
	{ NULL,				0	}
};

static const t_foo_dsp_fftrate_value g_hWind[] =
{
	{ _T( "Sin" ),		1	},
	{ _T( "Vorbis" ),	2	},
	{ NULL,				0	}
};


class t_foo_dsp_fftrate_params
{
public:
	t_foo_dsp_fftrate_params( void ):
		m_rate( 0 ),
		m_bits( 0 ),
		m_chann( 0 ),
		m_trans( 1 ),
		m_wind( 2 )
	{
	}

	// The GUID that identifies this DSP and its configuration.
	static const GUID &g_get_guid( void )
	{
		static const GUID guid = { 0x806d2aed, 0x290a, 0x41a7, { 0x9c, 0x2f, 0x7f, 0xb7, 0xca, 0xec, 0x90, 0xbe } };
		return guid;
	}

	// Read data from a preset.
	bool set_data( const dsp_preset &p_data )
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

	bool get_data( dsp_preset &p_data )
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


class dialog_foo_dsp_fftrate: public CDialogImpl <dialog_foo_dsp_fftrate>
{
public:
	enum
	{
		IDD = IDD_CONFIG
	};
 
	BEGIN_MSG_MAP_EX( dialog_foo_dsp_fftrate )
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_COMMAND( OnCommand )
	END_MSG_MAP()


	dialog_foo_dsp_fftrate( t_foo_dsp_fftrate_params &p_params ):
		m_params( p_params )
	{
	}


protected:
	BOOL OnInitDialog( CWindow wndFocus, LPARAM lInitParam )
	{
		combo_upload( IDC_COMBO_RATE,	g_hRates,	m_params.rate()  );
		combo_upload( IDC_COMBO_BITS,	g_hBits,	m_params.bits()  );
		combo_upload( IDC_COMBO_CHANN,	g_hChann,	m_params.chann() );
		combo_upload( IDC_COMBO_TRANS,	g_hTrans,	m_params.trans() );
		combo_upload( IDC_COMBO_WIND,	g_hWind,	m_params.wind()  );

		return FALSE;
	}

	void OnCommand( UINT uNotifyCode, int nID, CWindow wndCtl )
	{
		switch( nID )
		{
		case IDOK:
			m_params.set_rate(	combo_download(	IDC_COMBO_RATE,  g_hRates ) );
			m_params.set_bits(	combo_download(	IDC_COMBO_BITS,  g_hBits  ) );
			m_params.set_chann(	combo_download(	IDC_COMBO_CHANN, g_hChann ) );
			m_params.set_trans(	combo_download(	IDC_COMBO_TRANS, g_hTrans ) );
			m_params.set_wind(	combo_download(	IDC_COMBO_WIND,  g_hWind  ) );
			EndDialog( IDOK );
			break;
			
		case IDCANCEL:
			EndDialog( 0 );
			break;
		}
	}


private:
	t_foo_dsp_fftrate_params &m_params;

	void combo_upload( int id, const t_foo_dsp_fftrate_value *desc, t_int32 curr )
	{
		CComboBox combo( GetDlgItem( id ) );
		int select = 0;

		for( int n = 0 ; ; n ++ )
		{
			if( desc[n].name == NULL )
				break;

			combo.AddString( desc[n].name );

			if( desc[n].val == curr )
				select = n;
		}

		combo.SetCurSel( select );
	}

	t_int32 combo_download( int id, const t_foo_dsp_fftrate_value *desc )
	{
		CString text;
		CComboBox combo( GetDlgItem( id ) );

		combo.GetWindowText( text );

		for( int n = 0 ; ; n ++ )
		{
			if( desc[n].name == NULL )
				break;

			if( text.Compare( desc[n].name ) == 0 )
				return desc[n].val;
		}

		return 0;
	}
};


class foo_dsp_fftrate: public resampler_entry
{
public:
	foo_dsp_fftrate( void )
	{
	}

	void get_name( pfc::string_base &p_out )
	{
		p_out = "FFT-based sample rate converter";
	}

	GUID get_guid( void )
	{
		return t_foo_dsp_fftrate_params::g_get_guid();
	}

	bool have_config_popup( void )
	{
		return true;
	}

	float get_priority( void )
	{
		return 0;
	}

	bool is_conversion_supported( unsigned int src_srate, unsigned int dst_srate )
	{
		return false;
	}

	bool get_default_preset( dsp_preset &p_out )
	{
		t_foo_dsp_fftrate_params().get_data( p_out );
		return true;
	}

	bool create_preset( dsp_preset &p_out, unsigned p_target_srate, float p_qualityscale )
	{
		t_foo_dsp_fftrate_params params;
		params.set_rate( p_target_srate );
		params.get_data( p_out );
		return true;
	}

	bool instantiate( service_ptr_t<dsp> &p_out, const dsp_preset &p_preset )
	{
		bool ret = false;

		if( p_preset.get_owner() == get_guid() )
		{
			t_foo_dsp_fftrate_params params;
			params.set_data( p_preset );
//			p_out = reinterpret_cast<dsp *>(new service_impl_t <dsp_rate>( params ) );
			ret = p_out.is_valid();
		}
		
		return ret;
	}

	bool show_config_popup( dsp_preset &p_data, HWND p_parent )
	{
		t_foo_dsp_fftrate_params params;
		
		if( params.set_data( p_data ) )
		{
			dialog_foo_dsp_fftrate dlg( params );
			
			if( dlg.DoModal( p_parent ) == IDOK )
			{
				params.get_data( p_data );
				return true;
			}
		}
		
		return false;
	}
};


static service_factory_t <foo_dsp_fftrate> foo_foo_dsp_fftrate;
