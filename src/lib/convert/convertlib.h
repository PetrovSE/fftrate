#ifndef _CONVERTLIB_H_
#define _CONVERTLIB_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"
#include "../fft.h"
#include "../mdct.h"
#include "../complex.h"
#include "../convert.h"


//================================================================
//  Definitions
//----------------------------------------------------------------
#define SHIFT_SMP_24				( CONV_INTERNAL_BITS - 24 )
#define SHIFT_SMP_16				( CONV_INTERNAL_BITS - 16 )
#define SHIFT_SMP_8					( CONV_INTERNAL_BITS - 8 )

#define ROUND_SMP_32				D( 0.5 )
#define ROUND_SMP_24				D( one_fpp( SHIFT_SMP_24 - 1 ) )
#define ROUND_SMP_16				D( one_fpp( SHIFT_SMP_16 - 1 ) )
#define ROUND_SMP_8					D( one_fpp( SHIFT_SMP_8  - 1 ) )

#define NORM_FLOAT_IN				(DATA)dword_fpp( CONV_INTERNAL_BITS - 1 )
#define NORM_FLOAT_OUT				( (DATA)1.0 / NORM_FLOAT_IN )

#define ADD_SMP_8					( -128 )

#define CONV_INTERNAL_BITS			32
#define CONV_FRAME_DURATION			64			// in ms

enum
{
	MIX_MODE_BYPASS	= 0,
	MIX_MODE_MIXING
};

enum
{
	RES_MODE_NONE	= 0,
	RES_MODE_PRE,
	RES_MODE_POST
};

//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct 
{
	DWORD		packet_len;
	DWORD		skip_len;
	DWORD		hist_len;

	INT			sample_size;

	DWORD		rem;
	DATA		*pp_buffers[N_OF_MAX_CHANNELS];

	DWORD		unit_type;
	INT			unit_size;

	INT			n_of_chann;
	DWORD		chann_mask;

	DWORD		direction;
	DWORD		position;

	PCHANNELORDER	p_chann_order;
	PSAMPLEPEAKS	p_peaks;

} SAMPLESTATE, *PSAMPLESTATE;


typedef struct
{
	DWORD		rate;

	INT			packet_len;
	INT			spectrum_len;
	INT			frame_len;
	INT			hist_len;

	HFFT		h_fft_inst;
	HMDCT		h_mdct_inst;

	DATA		*p_window;

} RESAMPLER, *PRESAMPLER;


typedef struct 
{
	INT		channels;
	
	struct
	{
		INT		idx;
		DATA	val;
		
	} relation[N_OF_MAX_CHANNELS];
	
} CHANNELMIXER, *PCHANNELMIXER;


typedef struct TTRANSFORM TTRANSFORM;

typedef VOID (*pfn_resampler)( PRESAMPLER, PRESAMPLER, TTRANSFORM *, DATA *, DATA *, DATA *, DATA * );
typedef DATA (*pfn_chann_relations_weight)( CONST CHANNELDESC * );


typedef struct TTRANSFORM
{
	DWORD			transform_id;
	DWORD			window_id;

	CONST CHAR		*transform_type;
	CONST CHAR		*window_type;

	pfn_resampler	p_resampler;
	INT				tail;

	COMPLEX			*p_spectrum;
	DATA			*p_spec;
	DATA			*p_frame;

} TRANSFORM, *PTRANSFORM;


typedef struct 
{
	SAMPLESTATE		sample_in;
	SAMPLESTATE		sample_out;

	PSAMPLESTATE	p_sample_src;
	PSAMPLESTATE	p_sample_dst;
	PSAMPLESTATE	p_sample_work;

	DATA			*pp_hist_in[N_OF_MAX_CHANNELS];
	DATA			*pp_hist_out[N_OF_MAX_CHANNELS];

	RESAMPLER		resampler_in;
	RESAMPLER		resampler_out;
	TRANSFORM		transform;

	INT				mixing_mode;
	INT				resampling_mode;
	DWORD			statistic_mode;

	BOOL			chann_norm;
	CHANNELMIXER	p_chann_mixers[N_OF_MAX_CHANNELS];

} CONVINST, *PCONVINST;


//================================================================
//  Function prototypes
//----------------------------------------------------------------
BOOL resampling_init( PCONVINST h_inst, INT freq_in, INT freq_out, DWORD transform_id, DWORD window_id );
BOOL resampling_alloc( PCONVINST h_inst, PSAMPLESTATE p_sample );
VOID resampling_deinit( PCONVINST h_inst, PSAMPLESTATE p_sample );
VOID resampling_reset( PCONVINST h_inst, PSAMPLESTATE p_sample );
VOID resampling_process( PCONVINST h_inst, PSAMPLESTATE p_sample_in, PSAMPLESTATE p_sample_out );

BOOL transform_matrix_make( PCONVINST h_inst );
VOID transform_matrix_export( PCONVINST h_inst, PCHANNELMIXMATRIX p_transform_matrix );
VOID transform_matrix_processing( PCONVINST h_inst, PSAMPLESTATE p_sample_in, PSAMPLESTATE p_sample_out );

BOOL samples_alloc
(
	PSAMPLESTATE	p_sample,
	DWORD			unit_type,
	INT				n_chann,
	DWORD			chann_mask,
	CONST CHAR		*p_chann_order,
	INT				packet_len,
	BOOL			b_need_peaks
);

VOID samples_free( PSAMPLESTATE p_sample );
VOID samples_reset( PSAMPLESTATE p_sample );
VOID samples_set_skipped( PSAMPLESTATE p_sample, DWORD hist_len );

DWORD samples_load( PSAMPLESTATE p_sample, CONST BYTE *p_buff, DWORD buff_size_in_bytes, BOOL b_need_peaks );
DWORD samples_save( PSAMPLESTATE p_sample, PBYTE p_buff, DWORD buff_size_in_bytes, BOOL b_need_peaks );

#endif // _CONVERTLIB_H_
