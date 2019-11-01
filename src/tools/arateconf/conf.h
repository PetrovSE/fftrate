#ifndef _CONF_H_
#define _CONF_H_


//=============================================================================
// Definitions
//-----------------------------------------------------------------------------
#define DEF_RATE				48000
#define DEF_CHANN				2


#define DEV_PLAY				0x0001
#define DEV_REC					0x0002


//=============================================================================
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
	CHAR	name[MAX_STRING];
	CHAR	desc[MAX_STRING];

	INT		dev_out;
	INT		dev_in;

	DWORD	dw_used;
	DWORD	dw_default;

} CARDNAME, *PCARDNAME;


typedef struct
{
	PCARDNAME	p_card;
	BOOL		b_out;
	
} CARDDESC, PCARDDESC;


typedef struct
{
	CONST CHAR	*name;
	DWORD		val;

} FMT, *PFMT;


typedef struct
{
	INT			rate;
	INT			chann;
	INT			period;
	
	INT			max_play_db;
	INT			max_rec_db;
	
	FMT			format;
	CONST CHAR	*convert;

	CARDDESC	desc;
	CARDNAME	*p_card;

	BOOL		buff_resize;
	INT			buff_mul;
	INT			period_mul;

	BOOL		use_convert;
	BOOL		use_expand;
	BOOL		use_asym;
	
	BOOL		use_soft_play;
	BOOL		use_soft_rec;
	
	BOOL		use_dmix;
	BOOL		use_dsnoop;
	
	BOOL		use_phonon;
	BOOL		use_norm;

} CONFPARAMS, *PCONFPARAMS;


//================================================================
//  Function prototypes
//----------------------------------------------------------------
INT calculate_buffer( INT rate, INT period );
INT calculate_period( INT rate, BOOL buff_resize );
INT correct_rate( INT rate );

BOOL test_parameters( CONST CONFPARAMS *param, BOOL b_out );
INT  enum_devices( CONST CARDNAME *p_card, CARDNAME *p_devs, INT n_dev, BOOL b_play );
VOID update_mixer( VOID );

INT  device_load( CONST CHAR *cards, CARDNAME *p_card, INT len );
VOID card_name( CONST CARDNAME *p_card, CHAR *name, INT len );
VOID device_name( CONST CARDNAME *p_card, BOOL b_out, CHAR *name, INT len );

BOOL config_init( CONST CHAR *name );
VOID config_close( VOID );
BOOL config_save( CONST CONFPARAMS *param, BOOL b_single, BOOL b_show );

#endif // _CONF_H_
