#ifndef _IMAGE_H_
#define _IMAGE_H_


//================================================================
//  Definitions
//----------------------------------------------------------------
#define MAX_IMAGE_LAYERS			3


#define CH_STATUS_MAKE( _e, _n )	make_dword( _e, _n )

#define CH_STATUS_FLAG_OK			0x1
#define CH_STATUS_FLAG_FAILED		0x0

#define CH_STATUS_IS_OK( _s )		( get_hi_word( _s ) == CH_STATUS_FLAG_OK )
#define CH_STATUS_IS_FAILED( _s )	( get_hi_word( _s ) == CH_STATUS_FLAG_FAILED )
#define CH_STATUS_VAL( _s )			get_low_word( _s )



#define FCC_SAME					MAKEFOURCC( 0xff, 0xff, 0xff, 0xff )	// The same fourcc

#define FCC_RGB3					MAKEFOURCC( 'R', 'G', 'B', '3' )		// RGB-24 non standart (3 plans)
#define FCC_RGBT					MAKEFOURCC( 'R', 'G', 'B', 'T' )		// Compatibled with BI_RGB for Windows

#define FCC_YUV3					MAKEFOURCC( 'Y', 'U', 'V', '3' )		// YUV-24 non standart (3 plans)
#define FCC_YUVT					MAKEFOURCC( 'Y', 'U', 'V', 'T' )		// Tripple YUV - non standart

#define FCC_YUY2					MAKEFOURCC( 'Y', 'U', 'Y', '2' )		// Interleaved YUV 4:2:2 (Y0 U0 Y1 V0 ...)
#define FCC_YUYV					MAKEFOURCC( 'Y', 'U', 'Y', 'V' )		// The same as YUY2

#define FCC_cyuv					MAKEFOURCC( 'c', 'y', 'u', 'v' )		// Interleaved YUV 4:2:2 (U0 Y0 V0 Y1 ...)
#define FCC_UYVY					MAKEFOURCC( 'U', 'Y', 'V', 'Y' )		// The same as cyuv

#define FCC_YV12					MAKEFOURCC( 'Y', 'V', '1', '2' )		// Planar YUV 4:2:0 (plans: Y, V, U)
#define FCC_YV16					MAKEFOURCC( 'Y', 'V', '1', '6' )		// Planar YUV 4:2:2 (plans: Y, V, U)

#define FCC_Y411					MAKEFOURCC( 'Y', '4', '1', '1' )		// Interleaved YUV 4:1:1 (U2 Y0 Y1 V2 Y2 Y3 ...)
#define FCC_Y41P					MAKEFOURCC( 'Y', '4', '1', 'P' )		// The same as Y411
#define FCC_IYU1					MAKEFOURCC( 'I', 'Y', 'U', '1' )		// The same as Y411

#define FCC_I420					MAKEFOURCC( 'I', '4', '2', '0' )		// Planar YUV 4:2:0 (plans: Y, U, V)
#define FCC_IYUV					MAKEFOURCC( 'I', 'Y', 'U', 'V' )		// The same as I420

#define FCC_S910					MAKEFOURCC( 'S', '9', '1', '0' )		// Compressed by Sonix 1xxx
#define FCC_BA81					MAKEFOURCC( 'B', 'A', '8', '1' )		// Bayer RGB
#define FCC_MJPG					MAKEFOURCC( 'M', 'J', 'P', 'G' )		// Motion JPEG


//================================================================
//  Image chain parameters
//  Use for:
//		image_chain_info
//----------------------------------------------------------------
//  l   - chain length
//  i   - input FOURCC
//  o   - ouput FOURCC
//  s   - chain status (see CHAIN_STATUS_)
//----------------------------------------------------------------


//================================================================
//  Type definitions
//----------------------------------------------------------------
DECL_HANDLE( HCHAIN );

typedef struct
{
	INT		width;
	INT		height;
	
} VSIZE, *PVSIZE;


typedef struct
{
	VSIZE	size; // must be first!
	INT		step;
	PBYTE	data;
	
} PLAN, *PPLAN;


typedef struct
{
	FOURCC		fcc;
	CONST CHAR	*desc;

} FCCDESC, *PFCCDESC;


typedef struct
{
	FOURCC	fcc;
	INT		n_layers;

	DWORD	buff_size;
	DWORD	actual_size;

	union
	{
		VSIZE	size;
		PLAN	layers[MAX_IMAGE_LAYERS];
	};
	
} IMAGE, *PIMAGE;


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

CONST FCCDESC *image_get_known_fcc( VOID );

BOOL image_is_eq( CONST IMAGE *pic1, CONST IMAGE *pic2 );
BOOL image_fcc_compare( FOURCC fcc1, FOURCC fcc2 );
VOID image_set_prefer_fcc( FOURCC *p_fcc, INT len, FOURCC pref );

VOID image_sort_sizes( VSIZE *p_sizes, INT len );
VOID image_sort_fps( INT *p_fps, INT len );


HCHAIN image_chain_open( FOURCC fcc_in, FOURCC fcc_out );
HCHAIN image_chain_close( HCHAIN p_inst );

BOOL image_chain_alloc( HCHAIN p_inst, VSIZE size );
VOID image_chain_free( HCHAIN p_inst );
BOOL image_chain_process( HCHAIN p_inst, PIMAGE dst, PIMAGE src );

FOURCC image_chain_enum( HCHAIN p_inst, INT idx );
DWORD  image_chain_info( HCHAIN p_inst, CHAR type );

#define image_chain_status( _ic )		image_chain_info( _ic, 's' )


BOOL image_init( PIMAGE p_image, VSIZE size, FOURCC fcc );
BOOL image_template( PIMAGE p_image, CONST IMAGE *p_templ );

DWORD image_buffer_size( CONST IMAGE *p_image );
DWORD image_actual_size( CONST IMAGE *p_image );

BOOL image_memory_map
(
	PIMAGE	p_image,
	
	PBYTE	p_data,
	DWORD	actual_size,

	INT		n_layers,
	INT		*p_ofs,
	INT		*p_steps
);

#define image_memory_map_plain( _im, _d )			image_memory_map( _im, _d, INVALID_SIZE, 0, NULL, NULL )
#define image_memory_map_size( _im, _d, _s )		image_memory_map( _im, _d, _s, 0, NULL, NULL )


BOOL image_white_balance( IMAGE *image );
BOOL image_copy( IMAGE *dst, CONST IMAGE *src );

BOOL image_s910_to_ba81( IMAGE *dst, CONST IMAGE *src );
BOOL image_ba81_to_rgb3( IMAGE *dst, CONST IMAGE *src );
BOOL image_mjpg_to_yv16( IMAGE *dst, CONST IMAGE *src );
WORD libjpeg_version( VOID );

BOOL image_rgb3_to_yuv3( IMAGE *dst, CONST IMAGE *src );
BOOL image_rgb3_to_rgbt( IMAGE *dst, CONST IMAGE *src );

BOOL image_yuv3_to_rgb3( IMAGE *dst, CONST IMAGE *src );
BOOL image_rgbt_to_rgb3( IMAGE *dst, CONST IMAGE *src );

BOOL image_yuv3_to_yuvt( IMAGE *dst, CONST IMAGE *src );
BOOL image_yuvt_to_yuv3( IMAGE *dst, CONST IMAGE *src );

BOOL image_yuyv_uyvy_to_yuv3( IMAGE *dst, CONST IMAGE *src );
BOOL image_i420_yv12_to_yuv3( IMAGE *dst, CONST IMAGE *src );

BOOL image_yuv3_to_yuyv_uyvy( IMAGE *dst, CONST IMAGE *src );
BOOL image_yuv3_to_i420_yv12( IMAGE *dst, CONST IMAGE *src );

BOOL image_i420_yv12_to_yuyv_uyvy( IMAGE *dst, CONST IMAGE *src );
BOOL image_yv12_i420_to_i420_yv12( IMAGE *dst, CONST IMAGE *src );

BOOL image_yuyv_uyvy_to_i420_yv12( IMAGE *dst, CONST IMAGE *src );
BOOL image_yuyv_uyvy_to_uyvy_yuyv( IMAGE *dst, CONST IMAGE *src );

BOOL image_yv16_to_i420_yv12( IMAGE *dst, CONST IMAGE *src );
BOOL image_yv16_to_yuyv_uyvy( IMAGE *dst, CONST IMAGE *src );
BOOL image_yv16_to_yuv3( IMAGE *dst, CONST IMAGE *src );

CDECL_END

#endif // _IMAGE_H_
