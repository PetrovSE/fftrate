#ifndef _MATHMAC_H_
#define _MATHMAC_H_


//================================================================
//  Macros definitions
//----------------------------------------------------------------

//================================================================
//  Fixed point computing
//----------------------------------------------------------------
#define mul_power2( _x, _n )			( (_x) << (_n) )
#define one_fpp( _n )					mul_power2( 1, _n )
#define word_fpp( _n )					mul_power2( (WORD)1, _n )
#define dword_fpp( _n )					mul_power2( (DWORD)1, _n )

#define div_power2( _x, _n )			( (_x) >> (_n) )
#define div_power2r( _x, _n )			( ( (_x) + one_fpp( (_n) - 1 ) ) >> (_n) )

#define mul2( _x )						mul_power2( _x, 1 )
#define div2( _x )						div_power2( _x, 1 )
#define div2r( _x )						div_power2r( _x, 1 )

#define mod( _x, _y )					( ( (_x) + (_y) * ( ( labs(_x) / (_y) ) + 1 ) ) % (_y) )

#define is_pow2( _x )					is_zero( (_x) & ( (_x) - 1 ) )
#define is_odd( _x )					( (_x) & 0x1 )
#define is_even( _x )					( !is_odd( _x ) )

#define clip_byte( _x )					( (BYTE)(_x)  == (_x) ? (_x) : ( more_zero( _x ) ? UCHAR_MAX : 0 ) )
#define clip_short( _x )				( (SHORT)(_x) == (_x) ? (_x) : ( more_zero( _x ) ? INT16_MAX : INT16_MIN ) )
#define clip_long( _x )					( (LONG)(_x)  == (_x) ? (_x) : ( more_zero( _x ) ? INT32_MAX : INT32_MIN ) )


//================================================================
//  Predicate computing
//----------------------------------------------------------------
#define is_zero( _x )					( (_x) == 0 )
#define non_zero( _x )					( (_x) != 0 )

#define less_zero( _x )					( (_x) < 0 )
#define less_eq_zero( _x )				( (_x) <= 0 )

#define more_zero( _x )					( (_x) > 0 )
#define more_eq_zero( _x )				( (_x) >= 0 )


//================================================================
//  Bin mask
//----------------------------------------------------------------
#define flag_init( _x )					(_x)  = 0x0
#define flag_set( _x, _m )				(_x) |= (_m)
#define flag_reset( _x, _m )			(_x) &= ~(_m)
#define flag_mod( _x, _s, _m )			(_x)  = (_s) ? (_x) | (_m) : (_x) & ~(_m)

#define flag_check_bit( _x, _b )		( (_x) & one_fpp( _b ) )
#define flag_check_mask( _x, _m )		( ( (_x) & (_m) ) == (_m) )
#define flag_is_empty( _x )				is_zero( _x )


//================================================================
//  Min & max computing
//----------------------------------------------------------------
#ifndef min
#define min( _x, _y )					( (_x) < (_y) ? (_x) : (_y) )
#endif

#ifndef max
#define max( _x, _y )					( (_x) > (_y) ? (_x) : (_y) )
#endif


//================================================================
//  Byte computing
//----------------------------------------------------------------
#define alignment( _x, _n )				( ( ( (_x) + (_n) - 1 ) / (_n) ) * (_n) )
#define cutofs( _x, _n )				( (_x) - ( ( _x ) % ( _n ) ) )

#define make_word( _h, _l )				( ( (WORD)(_h) << 8 ) | (WORD)(_l) )
#define make_dword( _h, _l )			( ( (DWORD)(_h) << 16 ) | (DWORD)(_l) )
#define make_dword4( _a, _b, _c, _d )	( (_a) | ( (DWORD)(_b) << 8 ) | ( (DWORD)(_c) << 16 ) | ( (DWORD)(_d) << 24 ) )
#define make_ddword( _h, _l )			( ( (LONGLONG)(_h) << 32 ) | (LONGLONG)(_l) )


#define get_low_byte( _w )				(BYTE)( (_w) & 0xff )
#define get_hi_byte( _w )				get_low_byte( (_w) >> 8 )

#define get_low_word( _dw )				(WORD)( (_dw) & 0xffff )
#define get_hi_word( _dw )				get_low_word( (_dw) >> 16 )

#define get_low_dword( _ddw )			(DWORD)( (_ddw) & 0xffffffffL )
#define get_hi_dword( _ddw )			get_low_dword( (_dw) >> 32 )


//================================================================
//  Others computing
//----------------------------------------------------------------
#define sqr( _x )						( (_x) * (_x) )
#define log2d( _x )						( LOG_TO_LOG2 * (DATA)log( D( _x ) ) )

#endif // _MATHMAC_H_

