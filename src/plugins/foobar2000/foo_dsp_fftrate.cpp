#include "foo_dsp_fftrate.h"


DECLARE_COMPONENT_VERSION
(
	"DSP FFT rate",
	"1.0.1",
	"FFT-based sample rate converter for foobar2000\n"
	"Written by Petrov Sergey\n\n"
	"e-mail: petrovse@mail.ru\n"
	"url: https://github.com/PetrovSE/fftrate"
);


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
		return foo_dsp_fftrate_params::g_get_guid();
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
		foo_dsp_fftrate_params().get_data( p_out );
		return true;
	}

	bool create_preset( dsp_preset &p_out, unsigned p_target_srate, float p_qualityscale )
	{
		foo_dsp_fftrate_params params;
		params.set_rate( p_target_srate );
		params.get_data( p_out );
		return true;
	}

	bool instantiate( service_ptr_t<dsp> &p_out, const dsp_preset &p_preset )
	{
		bool ret = false;

		if( p_preset.get_owner() == get_guid() )
		{
			foo_dsp_fftrate_params params;
			params.set_data( p_preset );
//			p_out = reinterpret_cast<dsp *>(new service_impl_t <dsp_rate>( params ) );
			ret = p_out.is_valid();
		}
		
		return ret;
	}

	bool show_config_popup( dsp_preset &p_data, HWND p_parent )
	{
		foo_dsp_fftrate_params params;
		
		if( params.set_data( p_data ) )
		{
			foo_dsp_fftrate_dialog dlg( params );
			
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
