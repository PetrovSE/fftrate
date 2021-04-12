#include "foo_dsp_fftrate.h"


static const t_foo_dsp_fftrate_desc g_hTrans[] =
{
	{ _T( "FFT" ),		CONV_TRANSFORM_FFT	},
	{ _T( "DCT" ),		CONV_TRANSFORM_DCT	},
	{ NULL,				0	}
};

static const t_foo_dsp_fftrate_desc g_hWind[] =
{
	{ _T( "Sin" ),		CONV_WINDOW_SIN		},
	{ _T( "Vorbis" ),	CONV_WINDOW_VORBIS	},
	{ NULL,				0	}
};

static const t_foo_dsp_fftrate_desc g_hRates[] =
{
	{ _T( "By pass" ),	0		},
	{ _T( "6000" ),		6000	},
	{ _T( "8000" ),		8000	},
	{ _T( "11025" ),	11025	},
	{ _T( "12000" ),	12000	},
	{ _T( "16000" ),	16000	},
	{ _T( "22050" ),	22050	},
	{ _T( "24000" ),	24000	},
	{ _T( "32000" ),	32000	},
	{ _T( "44100" ),	44100	},
	{ _T( "48000" ),	48000	},
	{ _T( "64000" ),	64000	},
	{ _T( "88200" ),	88200	},
	{ _T( "96000" ),	96000	},
	{ _T( "128000" ),	128000	},
	{ _T( "176400" ),	176400	},
	{ _T( "192000" ),	192000	},
	{ _T( "256000" ),	256000	},
	{ _T( "352800" ),	352800	},
	{ _T( "384000" ),	384000	},
	{ NULL,				0		}
};


foo_dsp_fftrate_dialog::foo_dsp_fftrate_dialog( foo_dsp_fftrate_params &p_params ):
	m_params( p_params )
{
}


BOOL foo_dsp_fftrate_dialog::OnInitDialog( CWindow wndFocus, LPARAM lInitParam )
{
	combo_upload( IDC_COMBO_TRANS,	g_hTrans,	m_params.trans() );
	combo_upload( IDC_COMBO_WIND,	g_hWind,	m_params.wind()  );
	combo_upload( IDC_COMBO_RATE,	g_hRates,	m_params.rate()  );

	return FALSE;
}


void foo_dsp_fftrate_dialog::OnCommand( UINT uNotifyCode, int nID, CWindow wndCtl )
{
	switch( nID )
	{
	case IDOK:
		m_params.set_trans(	combo_download(	IDC_COMBO_TRANS, g_hTrans ) );
		m_params.set_wind(	combo_download(	IDC_COMBO_WIND,  g_hWind  ) );
		m_params.set_rate(	combo_download(	IDC_COMBO_RATE,  g_hRates ) );
		EndDialog( IDOK );
		break;
			
	case IDCANCEL:
		EndDialog( 0 );
		break;
	}
}


void foo_dsp_fftrate_dialog::combo_upload( int id, const t_foo_dsp_fftrate_desc *desc, t_int32 curr )
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


t_int32 foo_dsp_fftrate_dialog::combo_download( int id, const t_foo_dsp_fftrate_desc *desc )
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
};
